#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdbool.h>
#include <windows.h>
#include "../thirdparty/tsf.h"
#include "../thirdparty/miniaudio_io.h"

extern tsf* g_TinySoundFont;
extern CRITICAL_SECTION g_audio_cs;

bool init_audio_engine(void);
void cleanup_audio_engine(void);
void update_synth_params(void);
void begin_audio_fade_out(void);
bool is_audio_fade_out_complete(void);
void begin_audio_fade_in(void);

// Note control functions
void note_on(int actual_note);
void note_off(int actual_note);

#endif // AUDIO_ENGINE_H
