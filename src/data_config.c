#include "../include/data_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define LAST_STATE_FILENAME "config.ini"
#define PRESET_EXTENSION ".tkp"

// Menu state
int g_semitone = 0;
int g_octave = 0;
bool g_show_keyboard = true;
int g_current_row = 0;
int g_current_col = 0;
char g_current_preset[MAX_PATH] = "";
char g_save_input[MAX_PATH] = "";
int g_save_input_len = 0;

// ADSR Envelope
float g_attack = 0.0f;
float g_decay = 0.0f;
float g_sustain = 1.0f;
float g_release_time = 0.1f; // 100ms default
float g_filter_cutoff_hz = 18000.0f;
float g_filter_q = 0.707f;
int g_pitch_drift = 0;
float g_vol_drift = 0.0f;

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

int g_wheel_assign = WHEEL_ASSIGN_NONE;
int g_wheel_mode = WHEEL_MODE_MOUSE;
float g_wheel_sense = 1.0f;
char** g_preset_files = NULL;
int g_preset_count = 0;
int g_current_preset_index = -1;

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
    {"preset", &g_current_preset, CFG_STRING, MENU_PRESET, 0.0f, 0.0f, 1.0f},
    {"pitch_drift", &g_pitch_drift, CFG_INT, MENU_PITCH_DRIFT, 0.0f, 5.0f, 1.0f},
    {"vol_drift", &g_vol_drift, CFG_FLOAT, MENU_VOL_DRIFT, 0.0f, 10.0f, 0.5f},
    {"attack_time", &g_attack, CFG_FLOAT, MENU_ATTACK, 0.0f, 2.0f, 0.01f},
    {"decay_time", &g_decay, CFG_FLOAT, MENU_DECAY, 0.0f, 10.0f, 0.25f},
    {"sustain_level", &g_sustain, CFG_FLOAT, MENU_SUSTAIN, 0.0f, 1.0f, 0.05f},
    {"release_time_ms", &g_release_time, CFG_FLOAT_MS, MENU_RELEASE, 0.0f, 5.0f, 0.25f},
    {"filter_cutoff_hz", &g_filter_cutoff_hz, CFG_FLOAT, MENU_FILTER_CUTOFF, 40.0f, 18000.0f, 0.08f},
    {"filter_q", &g_filter_q, CFG_FLOAT, MENU_FILTER_Q, 0.2f, 10.0f, 0.1f},
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
    {"wheel_assign", &g_wheel_assign, CFG_INT, MENU_WHEEL_ASSIGN, 0.0f, (float)(WHEEL_ASSIGN_COUNT - 1), 1.0f},
    {"wheel_mode", &g_wheel_mode, CFG_INT, MENU_WHEEL_MODE, 0.0f, (float)(WHEEL_MODE_COUNT - 1), 1.0f},
    {"wheel_sense", &g_wheel_sense, CFG_FLOAT, MENU_WHEEL_SENSE, 0.25f, 8.0f, 0.25f},
    {"row0_start", &row_starts[0], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row1_start", &row_starts[1], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row2_start", &row_starts[2], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f},
    {"row3_start", &row_starts[3], CFG_NOTE_STRING, MENU_OPTION_NONE, 0.0f, 0.0f, 0.0f}
};

const int g_registry_size = sizeof(g_config_registry) / sizeof(g_config_registry[0]);

static const MenuOption g_global_menu_items[] = {
    MENU_SHOW_KEYBOARD, MENU_MASTER_VOLUME, MENU_PRESET
};

static const MenuOption g_tune_menu_items[] = {
    MENU_SEMITONE, MENU_OCTAVE, MENU_PITCH_DRIFT, MENU_VOL_DRIFT
};

static const MenuOption g_adsr_menu_items[] = {
    MENU_ATTACK, MENU_DECAY, MENU_SUSTAIN, MENU_RELEASE, MENU_FILTER_CUTOFF, MENU_FILTER_Q
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

static const MenuOption g_wheel_menu_items[] = {
    MENU_WHEEL_ASSIGN, MENU_WHEEL_MODE, MENU_WHEEL_SENSE
};

const MenuRow g_menu_layout[] = {
    {"System", g_global_menu_items, sizeof(g_global_menu_items) / sizeof(g_global_menu_items[0])},
    {"Tune", g_tune_menu_items, sizeof(g_tune_menu_items) / sizeof(g_tune_menu_items[0])},
    {"SYNTH", g_adsr_menu_items, sizeof(g_adsr_menu_items) / sizeof(g_adsr_menu_items[0])},
    {"\"VB-2\":", g_vib_menu_items, sizeof(g_vib_menu_items) / sizeof(g_vib_menu_items[0])},
    {"\"Trelicopter\":", g_trem_menu_items, sizeof(g_trem_menu_items) / sizeof(g_trem_menu_items[0])},
    {"\"RE-20\":", g_echo_menu_items, sizeof(g_echo_menu_items) / sizeof(g_echo_menu_items[0])},
    {"Wheel", g_wheel_menu_items, sizeof(g_wheel_menu_items) / sizeof(g_wheel_menu_items[0])}
};

const int g_menu_layout_size = sizeof(g_menu_layout) / sizeof(g_menu_layout[0]);

const WheelAssignmentOption g_wheel_assignment_options[] = {
    {"None", MENU_OPTION_NONE},
    {"Any", MENU_OPTION_COUNT},
    {"Master", MENU_MASTER_VOLUME},
    {"Cutoff", MENU_FILTER_CUTOFF}
};

const int g_wheel_assignment_option_count = sizeof(g_wheel_assignment_options) / sizeof(g_wheel_assignment_options[0]);

static const char* g_wheel_mode_labels[] = {
    "Mouse",
    "Pad"
};

static bool is_absolute_path(const char* path) {
    return path && ((strlen(path) > 1 && path[1] == ':') || (path[0] == '\\' && path[1] == '\\'));
}

static void build_app_path(const char* filename, char* out_path, size_t out_size) {
    char module_path[MAX_PATH];
    GetModuleFileNameA(NULL, module_path, MAX_PATH);
    char* last_sep = strrchr(module_path, '\\');
    if (last_sep) {
        last_sep[1] = '\0';
    } else {
        module_path[0] = '\0';
    }

    if (is_absolute_path(filename)) {
        snprintf(out_path, out_size, "%s", filename);
    } else {
        snprintf(out_path, out_size, "%s%s", module_path, filename);
    }
}

static bool has_preset_extension(const char* name) {
    size_t name_len = strlen(name);
    size_t ext_len = strlen(PRESET_EXTENSION);
    return name_len >= ext_len && _stricmp(name + name_len - ext_len, PRESET_EXTENSION) == 0;
}

static void normalize_preset_name(const char* input_name, char* out_name, size_t out_size) {
    if (!input_name || !input_name[0]) {
        out_name[0] = '\0';
        return;
    }

    snprintf(out_name, out_size, "%s", input_name);
    if (!has_preset_extension(out_name)) {
        strncat(out_name, PRESET_EXTENSION, out_size - strlen(out_name) - 1);
    }
}

static void set_current_preset_name(const char* preset_name) {
    if (!preset_name) {
        g_current_preset[0] = '\0';
        return;
    }
    normalize_preset_name(preset_name, g_current_preset, sizeof(g_current_preset));
}

void free_presets(void) {
    for (int i = 0; i < g_preset_count; i++) {
        free(g_preset_files[i]);
    }
    free(g_preset_files);
    g_preset_files = NULL;
    g_preset_count = 0;
    g_current_preset_index = -1;
}

void sync_current_preset_index(void) {
    g_current_preset_index = -1;
    for (int i = 0; i < g_preset_count; i++) {
        if (_stricmp(g_preset_files[i], g_current_preset) == 0) {
            g_current_preset_index = i;
            return;
        }
    }
}

void scan_presets(void) {
    WIN32_FIND_DATAA find_data;
    char search_pattern[MAX_PATH];
    build_app_path("*" PRESET_EXTENSION, search_pattern, sizeof(search_pattern));

    free_presets();

    HANDLE find_handle = FindFirstFileA(search_pattern, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        char** next_list = (char**)realloc(g_preset_files, sizeof(char*) * (g_preset_count + 1));
        if (!next_list) {
            continue;
        }
        g_preset_files = next_list;
        g_preset_files[g_preset_count] = _strdup(find_data.cFileName);
        if (g_preset_files[g_preset_count]) {
            g_preset_count++;
        }
    } while (FindNextFileA(find_handle, &find_data));

    FindClose(find_handle);
    sync_current_preset_index();
}

const char* get_current_preset_label(void) {
    return g_current_preset[0] ? g_current_preset : "Default";
}

bool make_unique_preset_name(const char* base_name, char* out_name, size_t out_size) {
    char normalized[MAX_PATH];
    char stem[MAX_PATH];
    char full_path[MAX_PATH];

    normalize_preset_name(base_name, normalized, sizeof(normalized));
    if (!normalized[0]) {
        return false;
    }

    snprintf(stem, sizeof(stem), "%s", normalized);
    char* ext = strrchr(stem, '.');
    if (ext) *ext = '\0';

    snprintf(out_name, out_size, "%s", normalized);
    build_app_path(out_name, full_path, sizeof(full_path));
    if (GetFileAttributesA(full_path) == INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    for (int index = 1; index < 10000; index++) {
        snprintf(out_name, out_size, "%s_%d%s", stem, index, PRESET_EXTENSION);
        build_app_path(out_name, full_path, sizeof(full_path));
        if (GetFileAttributesA(full_path) == INVALID_FILE_ATTRIBUTES) {
            return true;
        }
    }

    return false;
}

bool load_preset_with_name(const char* preset_name) {
    char normalized[MAX_PATH];
    normalize_preset_name(preset_name, normalized, sizeof(normalized));
    if (!normalized[0]) {
        return false;
    }

    if (!load_config(normalized)) {
        return false;
    }

    set_current_preset_name(normalized);
    init_note_map();
    sync_current_preset_index();
    return true;
}

bool save_preset_with_name(const char* preset_name) {
    char normalized[MAX_PATH];
    normalize_preset_name(preset_name, normalized, sizeof(normalized));
    if (!normalized[0]) {
        return false;
    }

    set_current_preset_name(normalized);
    if (!save_config(normalized)) {
        return false;
    }

    scan_presets();
    sync_current_preset_index();
    return true;
}

bool cycle_current_preset(int direction) {
    if (g_preset_count <= 0) {
        return false;
    }

    if (g_current_preset_index < 0) {
        g_current_preset_index = 0;
    } else {
        g_current_preset_index += direction;
        if (g_current_preset_index < 0) g_current_preset_index = g_preset_count - 1;
        if (g_current_preset_index >= g_preset_count) g_current_preset_index = 0;
    }

    return load_preset_with_name(g_preset_files[g_current_preset_index]);
}

void save_last_state(void) {
    save_config(LAST_STATE_FILENAME);
}

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

MenuOption get_wheel_target_option(void) {
    if (g_wheel_assign < 0 || g_wheel_assign >= g_wheel_assignment_option_count) {
        return MENU_OPTION_NONE;
    }
    return g_wheel_assignment_options[g_wheel_assign].target_option;
}

const char* get_wheel_assignment_label(void) {
    if (g_wheel_assign < 0 || g_wheel_assign >= g_wheel_assignment_option_count) {
        return "None";
    }
    return g_wheel_assignment_options[g_wheel_assign].label;
}

const char* get_wheel_mode_label(void) {
    if (g_wheel_mode < 0 || g_wheel_mode >= WHEEL_MODE_COUNT) {
        return "Mouse";
    }
    return g_wheel_mode_labels[g_wheel_mode];
}

// Load config file
bool load_config(const char* filename) {
    char resolved_path[MAX_PATH];
    build_app_path(filename, resolved_path, sizeof(resolved_path));
    FILE* f = fopen(resolved_path, "r");
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
                        case CFG_STRING:
                            snprintf((char*)g_config_registry[i].var_ptr, MAX_PATH, "%s", val);
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
    char resolved_path[MAX_PATH];
    build_app_path(filename, resolved_path, sizeof(resolved_path));
    FILE* f = fopen(resolved_path, "w");
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
            case CFG_STRING:
                fprintf(f, "%s=%s\n", g_config_registry[i].key, (char*)g_config_registry[i].var_ptr);
                break;
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
