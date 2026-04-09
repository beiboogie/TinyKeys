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
int g_current_row = 0;
int g_current_col = 0;

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

int row_starts[4] = {33, 38, 43, 48}; // Default pitches: A1 (33), D2 (38), G2 (43), C3 (48)
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
    if (octave < -1 || octave > 9) octave = 4; // limit range to C-1-C9
    
    return (octave + 1) * 12 + note_val;
}

// Convert MIDI pitch to Note string
void get_note_string(int midi_note, char* buf) {
    int note = midi_note % 12;
    int octave = (midi_note / 12) - 1;
    sprintf(buf, "%s%d", note_names[note], octave);
}

int g_save_status = 0;
unsigned long g_save_status_time = 0;

ConfigEntry g_config_registry[] = {
    {"semitone", &g_semitone, CFG_INT, MENU_SEMITONE, -127.0f, 127.0f, 1.0f},
    {"octave", &g_octave, CFG_INT, MENU_OCTAVE, -10.0f, 10.0f, 1.0f},
    {"show_keyboard", &g_show_keyboard, CFG_BOOL, MENU_SHOW_KEYBOARD, 0.0f, 1.0f, 1.0f},
    {"master_volume", &g_master_volume, CFG_FLOAT, MENU_MASTER_VOLUME, 0.0f, 2.0f, 0.05f},
    {"attack_time", &g_attack, CFG_FLOAT, MENU_ATTACK, 0.0f, 2.0f, 0.01f},
    {"decay_time", &g_decay, CFG_FLOAT, MENU_DECAY, 0.0f, 10.0f, 0.25f},
    {"sustain_level", &g_sustain, CFG_FLOAT, MENU_SUSTAIN, 0.0f, 1.0f, 0.05f},
    {"release_time_ms", &g_release_time, CFG_FLOAT_MS, MENU_RELEASE, 0.0f, 5.0f, 0.25f},
    {"vib_speed", &g_vib_speed, CFG_FLOAT, MENU_VIB_SPEED, 0.0f, 15.0f, 0.1f},
    {"vib_depth", &g_vib_depth, CFG_INT, MENU_VIB_DEPTH, 0.0f, 100.0f, 2.0f},
    {"vib_mode", &g_vib_mode, CFG_BOOL, MENU_VIB_MODE, 0.0f, 1.0f, 1.0f},
    {"rise_time", &g_rise_time, CFG_FLOAT, MENU_RISE_TIME, 0.0f, 5.0f, 0.5f},
    {"trem_speed", &g_trem_speed, CFG_FLOAT, MENU_TREM_SPEED, 0.0f, 15.0f, 0.1f},
    {"trem_depth", &g_trem_depth, CFG_INT, MENU_TREM_DEPTH, 0.0f, 100.0f, 2.0f},
    {"trem_bias", &g_trem_bias, CFG_INT, MENU_TREM_BIAS, 0.0f, 100.0f, 5.0f},
    {"delay_time", &g_delay_time, CFG_FLOAT, MENU_DELAY_TIME, 0.1f, 2.0f, 0.025f},
    {"delay_mix", &g_delay_mix, CFG_INT, MENU_DELAY_MIX, 0.0f, 100.0f, 10.0f},
    {"delay_fb", &g_delay_fb, CFG_INT, MENU_DELAY_FB, 0.0f, 100.0f, 10.0f},
    {"delay_sat", &g_delay_sat, CFG_INT, MENU_DELAY_SAT, 0.0f, 100.0f, 10.0f},
    {"delay_mod_spd", &g_delay_mod_spd, CFG_FLOAT, MENU_DELAY_MOD_SPEED, 0.0f, 15.0f, 0.1f},
    {"delay_mod_dep", &g_delay_mod_dep, CFG_INT, MENU_DELAY_MOD_DEPTH, 0.0f, 100.0f, 1.0f},
    {"row0_start", &row_starts[0], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row1_start", &row_starts[1], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row2_start", &row_starts[2], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row3_start", &row_starts[3], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f}
};

const int g_registry_size = sizeof(g_config_registry) / sizeof(g_config_registry[0]);

static const MenuOption g_global_menu_items[] = {
    MENU_SEMITONE, MENU_OCTAVE, MENU_SHOW_KEYBOARD, MENU_MASTER_VOLUME
};

static const MenuOption g_adsr_menu_items[] = {
    MENU_ATTACK, MENU_DECAY, MENU_SUSTAIN, MENU_RELEASE
};

static const MenuOption g_vib_menu_items[] = {
    MENU_VIB_SPEED, MENU_VIB_DEPTH, MENU_VIB_MODE, MENU_RISE_TIME
};

static const MenuOption g_trem_menu_items[] = {
    MENU_TREM_SPEED, MENU_TREM_DEPTH, MENU_TREM_BIAS
};

static const MenuOption g_echo_menu_items[] = {
    MENU_DELAY_TIME, MENU_DELAY_MIX, MENU_DELAY_FB,
    MENU_DELAY_SAT, MENU_DELAY_MOD_SPEED, MENU_DELAY_MOD_DEPTH
};

const MenuRow g_menu_layout[] = {
    {"Global", g_global_menu_items, sizeof(g_global_menu_items) / sizeof(g_global_menu_items[0])},
    {"ADSR", g_adsr_menu_items, sizeof(g_adsr_menu_items) / sizeof(g_adsr_menu_items[0])},
    {"\"VB-2\":", g_vib_menu_items, sizeof(g_vib_menu_items) / sizeof(g_vib_menu_items[0])},
    {"\"Trelicopter\":", g_trem_menu_items, sizeof(g_trem_menu_items) / sizeof(g_trem_menu_items[0])},
    {"\"RE-20\":", g_echo_menu_items, sizeof(g_echo_menu_items) / sizeof(g_echo_menu_items[0])}
};

const int g_menu_layout_size = sizeof(g_menu_layout) / sizeof(g_menu_layout[0]);

ConfigEntry* get_menu_config_entry(MenuOption option) {
    for (int i = 0; i < g_registry_size; i++) {
        if (g_config_registry[i].menu_option == option) {
            return &g_config_registry[i];
        }
    }
    return NULL;
}

MenuOption get_current_menu_option(void) {
    if (g_current_row < 0 || g_current_row >= g_menu_layout_size) return MENU_OPTION_NONE;
    if (g_current_col < 0 || g_current_col >= g_menu_layout[g_current_row].item_count) return MENU_OPTION_NONE;
    return g_menu_layout[g_current_row].item_enums[g_current_col];
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
            
            for (int i = 0; i < g_registry_size; i++) {
                if (_stricmp(key, g_config_registry[i].key) == 0) {
                    switch (g_config_registry[i].type) {
                        case CFG_INT:
                            *(int*)g_config_registry[i].var_ptr = atoi(val);
                            break;
                        case CFG_FLOAT:
                            *(float*)g_config_registry[i].var_ptr = (float)atof(val);
                            break;
                        case CFG_FLOAT_MS:
                            *(float*)g_config_registry[i].var_ptr = (float)atof(val) / 1000.0f;
                            break;
                        case CFG_BOOL:
                            *(bool*)g_config_registry[i].var_ptr = (atoi(val) != 0 || _stricmp(val, "true") == 0);
                            break;
                        case CFG_NOTE_STRING:
                            *(int*)g_config_registry[i].var_ptr = parse_note(val);
                            break;
                    }
                    break;
                }
            }
        }
    }
    fclose(f);
    return true;
}

// Save config file
bool save_config(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return false;
    
    for (int i = 0; i < g_registry_size; i++) {
        switch (g_config_registry[i].type) {
            case CFG_INT:
                fprintf(f, "%s=%d\n", g_config_registry[i].key, *(int*)g_config_registry[i].var_ptr);
                break;
            case CFG_FLOAT:
                fprintf(f, "%s=%.3f\n", g_config_registry[i].key, *(float*)g_config_registry[i].var_ptr);
                break;
            case CFG_FLOAT_MS:
                fprintf(f, "%s=%.0f\n", g_config_registry[i].key, *(float*)g_config_registry[i].var_ptr * 1000.0f);
                break;
            case CFG_BOOL:
                fprintf(f, "%s=%d\n", g_config_registry[i].key, *(bool*)g_config_registry[i].var_ptr ? 1 : 0);
                break;
            case CFG_NOTE_STRING: {
                char buf[16];
                get_note_string(*(int*)g_config_registry[i].var_ptr, buf);
                fprintf(f, "%s=%s\n", g_config_registry[i].key, buf);
                break;
            }
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
