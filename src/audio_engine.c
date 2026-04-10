#include "../include/audio_engine.h"
#include "../include/data_config.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define TSF_IMPLEMENTATION
#include "../thirdparty/tsf.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../thirdparty/miniaudio_io.h"

// Minimal SoundFont definition
const static unsigned char MinimalSoundFont[] = {
	#define TEN0 0,0,0,0,0,0,0,0,0,0
	'R','I','F','F',220,1,0,0,'s','f','b','k',
	'L','I','S','T',88,1,0,0,'p','d','t','a',
	'p','h','d','r',76,TEN0,TEN0,TEN0,TEN0,0,0,0,0,TEN0,0,0,0,0,0,0,0,255,0,255,0,1,TEN0,0,0,0,
	'p','b','a','g',8,0,0,0,0,0,0,0,1,0,0,0,'p','m','o','d',10,TEN0,0,0,0,'p','g','e','n',8,0,0,0,41,0,0,0,0,0,0,0,
	'i','n','s','t',44,TEN0,TEN0,0,0,0,0,0,0,0,0,TEN0,0,0,0,0,0,0,0,1,0,
	'i','b','a','g',8,0,0,0,0,0,0,0,2,0,0,0,'i','m','o','d',10,TEN0,0,0,0,
	'i','g','e','n',12,0,0,0,54,0,1,0,53,0,0,0,0,0,0,0,
	's','h','d','r',92,TEN0,TEN0,0,0,0,0,0,0,0,50,0,0,0,0,0,0,0,49,0,0,0,34,86,0,0,57,217,0,0,1,TEN0,TEN0,TEN0,TEN0,0,0,0,0,0,0,0,
	'L','I','S','T',112,0,0,0,'s','d','t','a','s','m','p','l',100,0,0,0,86,0,119,3,31,7,147,10,43,14,169,17,58,21,189,24,73,28,204,31,73,35,249,38,46,42,71,46,250,48,150,53,242,55,126,60,151,63,108,66,126,72,207,
		70,86,83,100,72,74,100,163,39,241,163,59,175,59,179,9,179,134,187,6,186,2,194,5,194,15,200,6,202,96,206,159,209,35,213,213,216,45,220,221,223,76,227,221,230,91,234,242,237,105,241,8,245,118,248,32,252
};

tsf* g_TinySoundFont = NULL;
CRITICAL_SECTION g_audio_cs;
static ma_device device;
static ma_lpf2 g_synth_lpf;
static bool g_synth_lpf_ready = false;

#define AUDIO_SAMPLE_RATE 44100
#define FILTER_MIN_Q 0.2f
#define NOTE_CHANNEL_COUNT 128
#define CONFIG_FADE_MS 5

static float g_transition_gain = 1.0f;
static float g_transition_step = 0.0f;
static int g_transition_target = 1;
static bool g_fade_out_complete = false;

// Convert Hz to cents
static int hz_to_cents(float hz) {
    if (hz <= 0.001f) return -12000;
    return (int)(1200.0f * (logf(hz / 8.176f) / logf(2.0f)));
}

static float clampf(float value, float min_value, float max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static void refresh_synth_filter(void) {
    ma_lpf2_config filter_config = ma_lpf2_config_init(
        ma_format_s16,
        2,
        AUDIO_SAMPLE_RATE,
        clampf(g_filter_cutoff_hz, 40.0f, 18000.0f),
        clampf(g_filter_q, FILTER_MIN_Q, 10.0f)
    );

    if (!g_synth_lpf_ready) {
        if (ma_lpf2_init(&filter_config, NULL, &g_synth_lpf) == MA_SUCCESS) {
            g_synth_lpf_ready = true;
        }
        return;
    }

    ma_lpf2_reinit(&filter_config, &g_synth_lpf);
}

// Update Synth Parameters (ADSR, Vibrato, Volume)
void update_synth_params() {
    if (!g_TinySoundFont) return;
    EnterCriticalSection(&g_audio_cs);
    float effective_depth = g_vib_depth * g_current_fade;
    for (int i = 0; i < g_TinySoundFont->presetNum; i++) {
        for (int j = 0; j < g_TinySoundFont->presets[i].regionNum; j++) {
            g_TinySoundFont->presets[i].regions[j].ampenv.attack = g_attack;
            g_TinySoundFont->presets[i].regions[j].ampenv.decay = g_decay;
            g_TinySoundFont->presets[i].regions[j].ampenv.sustain = g_sustain;
            g_TinySoundFont->presets[i].regions[j].ampenv.release = g_release_time;
            
            g_TinySoundFont->presets[i].regions[j].freqVibLFO = hz_to_cents(g_vib_speed);
            g_TinySoundFont->presets[i].regions[j].vibLfoToPitch = (int)effective_depth;
        }
    }
    tsf_set_volume(g_TinySoundFont, 1.0f);
    refresh_synth_filter();
    LeaveCriticalSection(&g_audio_cs);
}

void begin_audio_fade_out(void) {
    EnterCriticalSection(&g_audio_cs);
    g_transition_target = 0;
    g_transition_step = 1.0f / (AUDIO_SAMPLE_RATE * CONFIG_FADE_MS / 1000.0f);
    g_fade_out_complete = false;
    LeaveCriticalSection(&g_audio_cs);
}

bool is_audio_fade_out_complete(void) {
    bool result;
    EnterCriticalSection(&g_audio_cs);
    result = g_fade_out_complete;
    LeaveCriticalSection(&g_audio_cs);
    return result;
}

void begin_audio_fade_in(void) {
    EnterCriticalSection(&g_audio_cs);
    g_transition_target = 1;
    g_transition_step = 1.0f / (AUDIO_SAMPLE_RATE * CONFIG_FADE_MS / 1000.0f);
    g_fade_out_complete = false;
    LeaveCriticalSection(&g_audio_cs);
}

// Audio thread callback
static void AudioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    EnterCriticalSection(&g_audio_cs);
    tsf_render_short(g_TinySoundFont, (short*)pOutput, (int)frameCount, 0);
    if (g_synth_lpf_ready) {
        ma_lpf2_process_pcm_frames(&g_synth_lpf, pOutput, pOutput, frameCount);
    }

    if (g_gain != 1.0f) {
        short* samples = (short*)pOutput;
        for (ma_uint32 i = 0; i < frameCount * 2; i++) {
            float scaled = samples[i] * g_gain;
            if (scaled > 32767.0f) scaled = 32767.0f;
            if (scaled < -32768.0f) scaled = -32768.0f;
            samples[i] = (short)scaled;
        }
    }
    
    if (g_trem_enabled && g_trem_depth > 0) {
        short* samples = (short*)pOutput;
        float phase_inc = g_trem_speed / AUDIO_SAMPLE_RATE;
        float depth_f = g_trem_depth / 100.0f;
        float B = g_trem_bias / 100.0f;
        if (B < 0.01f) B = 0.01f;
        if (B > 0.99f) B = 0.99f;
        
        for (ma_uint32 i = 0; i < frameCount; i++) {
            float p = g_trem_phase;
            float mapped_p;
            if (p < B) {
                mapped_p = 0.5f * (p / B);
            } else {
                mapped_p = 0.5f + 0.5f * ((p - B) / (1.0f - B));
            }
            
            float wave = 0.5f - 0.5f * cosf(mapped_p * 2.0f * 3.14159265358979323846f);
            float mult = 1.0f - depth_f * wave;
            
            samples[i*2] = (short)(samples[i*2] * mult);
            samples[i*2+1] = (short)(samples[i*2+1] * mult);
            
            g_trem_phase += phase_inc;
            if (g_trem_phase >= 1.0f) g_trem_phase -= 1.0f;
        }
    } else {
        g_trem_phase += (g_trem_speed / AUDIO_SAMPLE_RATE) * frameCount;
        g_trem_phase = fmodf(g_trem_phase, 1.0f);
    }
    
    // Tape Echo Processing
    if (g_delay_enabled) {
        short* samples = (short*)pOutput;
        float phase_inc = g_delay_mod_spd / AUDIO_SAMPLE_RATE;
        float base_delay_samples = g_delay_time * AUDIO_SAMPLE_RATE;
        float mod_depth_samples = (g_delay_mod_dep / 1200.0f) * base_delay_samples; // Approximate cents to samples delay modulation
        
        float fb_gain = g_delay_fb / 100.0f;
        float mix_gain = g_delay_mix / 100.0f;
        float sat_amount = g_delay_sat / 100.0f;
        
        for (ma_uint32 i = 0; i < frameCount; i++) {
            float in_l = samples[i*2] / 32768.0f;
            float in_r = samples[i*2+1] / 32768.0f;
            
            // LFO for modulation
            float lfo_val = sinf(g_delay_lfo_phase * 2.0f * 3.14159265358979323846f);
            float current_delay_samples = base_delay_samples + mod_depth_samples * lfo_val;
            
            // Read from delay buffer with linear interpolation
            float read_ptr_float = g_delay_write_ptr - current_delay_samples * 2.0f;
            
            while (read_ptr_float < 0.0f) read_ptr_float += DELAY_BUFFER_SIZE;
            
            int read_ptr_int = (int)read_ptr_float;
            // Ensure read pointer is even (aligned to left channel)
            read_ptr_int = (read_ptr_int / 2) * 2;
            
            float frac = (read_ptr_float - read_ptr_int) / 2.0f; // 0.0 to 1.0 between sample frames
            
            int next_ptr_int = (read_ptr_int + 2) % DELAY_BUFFER_SIZE;
            
            float delay_l = g_delay_buffer[read_ptr_int] * (1.0f - frac) + g_delay_buffer[next_ptr_int] * frac;
            float delay_r = g_delay_buffer[read_ptr_int+1] * (1.0f - frac) + g_delay_buffer[next_ptr_int+1] * frac;
            
            // Soft clipping saturation on feedback
            float fb_l = delay_l * fb_gain;
            float fb_r = delay_r * fb_gain;
            
            if (sat_amount > 0.0f) {
                // Simple soft clip: x - x^3/3, scaled by sat_amount
                float sat_factor = sat_amount * 2.0f; // Scale up for more effect
                fb_l = fb_l * (1.0f + sat_factor) / (1.0f + sat_factor * fabsf(fb_l));
                fb_r = fb_r * (1.0f + sat_factor) / (1.0f + sat_factor * fabsf(fb_r));
            }
            
            // Write to buffer
            g_delay_buffer[g_delay_write_ptr] = in_l + fb_l;
            g_delay_buffer[g_delay_write_ptr+1] = in_r + fb_r;
            
            g_delay_write_ptr = (g_delay_write_ptr + 2) % DELAY_BUFFER_SIZE;
            
            // Mix output
            float out_l = in_l + delay_l * mix_gain;
            float out_r = in_r + delay_r * mix_gain;
            
            // Hard clip output to prevent wrapping
            if (out_l > 1.0f) out_l = 1.0f; else if (out_l < -1.0f) out_l = -1.0f;
            if (out_r > 1.0f) out_r = 1.0f; else if (out_r < -1.0f) out_r = -1.0f;
            
            samples[i*2] = (short)(out_l * 32767.0f);
            samples[i*2+1] = (short)(out_r * 32767.0f);
            
            g_delay_lfo_phase += phase_inc;
            if (g_delay_lfo_phase >= 1.0f) g_delay_lfo_phase -= 1.0f;
        }
    } else {
        // Keep LFO running and buffer writing even when bypassed so turning it on is smooth
        float phase_inc = g_delay_mod_spd / AUDIO_SAMPLE_RATE;
        short* samples = (short*)pOutput;
        for (ma_uint32 i = 0; i < frameCount; i++) {
            g_delay_buffer[g_delay_write_ptr] = samples[i*2] / 32768.0f;
            g_delay_buffer[g_delay_write_ptr+1] = samples[i*2+1] / 32768.0f;
            g_delay_write_ptr = (g_delay_write_ptr + 2) % DELAY_BUFFER_SIZE;
            
            g_delay_lfo_phase += phase_inc;
            if (g_delay_lfo_phase >= 1.0f) g_delay_lfo_phase -= 1.0f;
        }
    }

    if (g_master_volume != 1.0f) {
        short* samples = (short*)pOutput;
        for (ma_uint32 i = 0; i < frameCount * 2; i++) {
            float scaled = samples[i] * g_master_volume;
            if (scaled > 32767.0f) scaled = 32767.0f;
            if (scaled < -32768.0f) scaled = -32768.0f;
            samples[i] = (short)scaled;
        }
    }

    if (g_transition_gain != 1.0f || g_transition_target == 0) {
        short* samples = (short*)pOutput;
        for (ma_uint32 frame = 0; frame < frameCount; frame++) {
            if (g_transition_target == 0 && g_transition_gain > 0.0f) {
                g_transition_gain -= g_transition_step;
                if (g_transition_gain <= 0.0f) {
                    g_transition_gain = 0.0f;
                    g_fade_out_complete = true;
                }
            } else if (g_transition_target == 1 && g_transition_gain < 1.0f) {
                g_transition_gain += g_transition_step;
                if (g_transition_gain >= 1.0f) {
                    g_transition_gain = 1.0f;
                }
            }

            for (int ch = 0; ch < 2; ch++) {
                int idx = (int)(frame * 2 + ch);
                float scaled = samples[idx] * g_transition_gain;
                if (scaled > 32767.0f) scaled = 32767.0f;
                if (scaled < -32768.0f) scaled = -32768.0f;
                samples[idx] = (short)scaled;
            }
        }
    }
    
    LeaveCriticalSection(&g_audio_cs);
}

bool init_audio_engine(void) {
    InitializeCriticalSection(&g_audio_cs);
    srand((unsigned int)time(NULL));
    
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = AUDIO_SAMPLE_RATE;
    deviceConfig.dataCallback = AudioCallback;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        fprintf(stderr, "Could not initialize audio hardware or driver\n");
        return false;
    }

    g_TinySoundFont = tsf_load_memory(MinimalSoundFont, sizeof(MinimalSoundFont));
    if (!g_TinySoundFont) {
        fprintf(stderr, "Could not load SoundFont\n");
        ma_device_uninit(&device);
        return false;
    }

    for (int channel = 0; channel < NOTE_CHANNEL_COUNT; channel++) {
        tsf_channel_set_presetindex(g_TinySoundFont, channel, 0);
        tsf_channel_set_volume(g_TinySoundFont, channel, 1.0f);
        tsf_channel_set_tuning(g_TinySoundFont, channel, 0.0f);
    }
    
    tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, (int)deviceConfig.sampleRate, 0);
    update_synth_params();

    if (ma_device_start(&device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to start playback device.\n");
        ma_device_uninit(&device);
        tsf_close(g_TinySoundFont);
        return false;
    }
    
    return true;
}

void cleanup_audio_engine(void) {
    ma_device_uninit(&device);
    if (g_TinySoundFont) tsf_close(g_TinySoundFont);
    if (g_synth_lpf_ready) {
        ma_lpf2_uninit(&g_synth_lpf, NULL);
        g_synth_lpf_ready = false;
    }
    DeleteCriticalSection(&g_audio_cs);
}

void note_on(int actual_note) {
    float pitch_offset_semitones = 0.0f;
    float velocity = 1.0f;

    if (g_pitch_drift > 0) {
        float max_cents = (float)g_pitch_drift;
        float random_unit = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        pitch_offset_semitones = (random_unit * max_cents) / 100.0f;
    }

    if (g_vol_drift > 0.0f) {
        float drift_ratio = g_vol_drift / 100.0f;
        float random_unit = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        velocity = 1.0f + random_unit * drift_ratio;
        if (velocity < 0.0f) velocity = 0.0f;
    }

    EnterCriticalSection(&g_audio_cs);
    tsf_channel_set_tuning(g_TinySoundFont, actual_note, pitch_offset_semitones);
    tsf_channel_set_volume(g_TinySoundFont, actual_note, velocity);
    tsf_channel_note_on(g_TinySoundFont, actual_note, actual_note, 1.0f);
    LeaveCriticalSection(&g_audio_cs);
}

void note_off(int actual_note) {
    EnterCriticalSection(&g_audio_cs);
    tsf_channel_note_off(g_TinySoundFont, actual_note, actual_note);
    LeaveCriticalSection(&g_audio_cs);
}
