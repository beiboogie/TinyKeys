#include "../include/data_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

// Menu state
int g_semitone = 0;
int g_octave = 0;
bool g_show_keyboard = true;
int g_menu_selection = 0; // 0: Semitone, 1: Octave, 2: Show Keyboard, 3: Vol, 4: A, 5: D, 6: S, 7: R, 8: Spd, 9: Dep, 10: Mode, 11: Rise, 12: TremSpd, 13: TremDep, 14: TremBias, 15: EchoTime, 16: EchoMix, 17: EchoFB, 18: EchoSat, 19: EchoSpd, 20: EchoDep

// ADSR Envelope
float g_attack = 0.0f;
float g_decay = 0.0f;
float g_sustain = 1.0f;
float g_release_time = 0.1f; // 100ms default

// Master Volume & Vibrato
float g_master_volume = 1.0f;
float g_vib_speed = 1.3f;
int g_vib_depth = 16;
bool g_vib_enabled = false;
bool g_vib_mode = true; // false: Unlatch, true: Latch
float g_rise_time = 1.5f;
float g_current_fade = 0.0f;

// Tremolo Control
float g_trem_speed = 1.3f;
int g_trem_depth = 22;
int g_trem_bias = 55;
bool g_trem_enabled = false;
float g_trem_phase = 0.0f;

// Tape Echo Control
float g_delay_buffer[DELAY_BUFFER_SIZE] = {0};
int g_delay_write_ptr = 0;
float g_delay_time = 0.4f; // 400ms
int g_delay_mix = 20; // 20%
int g_delay_fb = 40; // 40%
int g_delay_sat = 40; // 40%
float g_delay_mod_spd = 0.3f;
int g_delay_mod_dep = 10;
bool g_delay_enabled = false;
float g_delay_lfo_phase = 0.0f;

// Key layout definitions
const int num_keys[4] = {13, 12, 11, 10};
const int vk_map[4][16] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', VK_OEM_MINUS, VK_OEM_PLUS, VK_BACK}, // Row 0: 13 keys
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', VK_OEM_4, VK_OEM_6}, // Row 1: 12 keys
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', VK_OEM_1, VK_OEM_7}, // Row 2: 11 keys
    {'Z', 'X', 'C', 'V', 'B', 'N', 'M', VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_2} // Row 3: 10 keys
};

const char* key_display_names[4][16] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BS"},
    {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'"},
    {"Z", "X", "C", "V", "B", "N", "M", ",", ".", "/"}
};

int row_starts[4] = {72, 60, 48, 36}; // Default pitches (C5, C4, C3, C2)
int note_map[4][16];
bool key_state[4][16] = {false};
int active_notes[4][16]; // To remember which exact MIDI note was triggered for note_off

const char* note_names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// Parse Note name like "C4" to MIDI pitch
int parse_note(const char* note_str) {
    if (!note_str || strlen(note_str) < 2) return -1;
    
    char note_char = toupper(note_str[0]);
    int note_val = 0;
    switch(note_char) {
        case 'C': note_val = 0; break;
        case 'D': note_val = 2; break;
        case 'E': note_val = 4; break;
        case 'F': note_val = 5; break;
        case 'G': note_val = 7; break;
        case 'A': note_val = 9; break;
        case 'B': note_val = 11; break;
        default: return -1;
    }
    
    int ptr = 1;
    if (note_str[ptr] == '#') {
        note_val++;
        ptr++;
    }
    
    int octave = atoi(&note_str[ptr]);
    if (octave < 1 || octave > 9) octave = 4; // limit range to C1-C9
    
    return (octave + 1) * 12 + note_val;
}

// Convert MIDI pitch to Note string
void get_note_string(int midi_note, char* buf) {
    int note = midi_note % 12;
    int octave = (midi_note / 12) - 1;
    sprintf(buf, "%s%d", note_names[note], octave);
}

// Load config file
bool load_config(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return false; // Ignore if config not found
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char key[64], val[64];
        if (sscanf(line, "%[^=]=%s", key, val) == 2) {
            // Trim spaces
            int len = strlen(key);
            while(len > 0 && isspace(key[len-1])) key[--len] = '\0';
            
            if (strcmp(key, "row0_start") == 0) row_starts[0] = parse_note(val);
            else if (strcmp(key, "row1_start") == 0) row_starts[1] = parse_note(val);
            else if (strcmp(key, "row2_start") == 0) row_starts[2] = parse_note(val);
            else if (strcmp(key, "row3_start") == 0) row_starts[3] = parse_note(val);
            else if (strcmp(key, "release_time_ms") == 0) g_release_time = atof(val) / 1000.0f;
        }
    }
    fclose(f);
    return true;
}

// Initialize pitch mapping
void init_note_map() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < num_keys[r]; c++) {
            note_map[r][c] = row_starts[r] + c;
            active_notes[r][c] = -1;
        }
    }
}