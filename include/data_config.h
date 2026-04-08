#ifndef DATA_CONFIG_H
#define DATA_CONFIG_H

#include <stdbool.h>

// Menu state
extern int g_semitone;
extern int g_octave;
extern bool g_show_keyboard;
extern int g_menu_selection;

// ADSR Envelope
extern float g_attack;
extern float g_decay;
extern float g_sustain;
extern float g_release_time;

// Master Volume & Vibrato
extern float g_master_volume;
extern float g_vib_speed;
extern int g_vib_depth;
extern bool g_vib_enabled;
extern bool g_vib_mode;
extern float g_rise_time;
extern float g_current_fade;

// Tremolo Control
extern float g_trem_speed;
extern int g_trem_depth;
extern int g_trem_bias;
extern bool g_trem_enabled;
extern float g_trem_phase;

// Tape Echo Control
#define DELAY_BUFFER_SIZE (44100 * 2 * 2) // 2 seconds stereo at 44100Hz
extern float g_delay_buffer[DELAY_BUFFER_SIZE];
extern int g_delay_write_ptr;
extern float g_delay_time;
extern int g_delay_mix;
extern int g_delay_fb;
extern int g_delay_sat;
extern float g_delay_mod_spd;
extern int g_delay_mod_dep;
extern bool g_delay_enabled;
extern float g_delay_lfo_phase;

// Key layout definitions
extern const int num_keys[4];
extern const int vk_map[4][16];
extern const char* key_display_names[4][16];
extern int row_starts[4];
extern int note_map[4][16];
extern bool key_state[4][16];
extern int active_notes[4][16];
extern const char* note_names[];

bool load_config(const char* filename);
void init_note_map();
int parse_note(const char* note_str);
void get_note_string(int midi_note, char* buf);

#endif // DATA_CONFIG_H