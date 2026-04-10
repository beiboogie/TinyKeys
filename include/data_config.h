#ifndef DATA_CONFIG_H
#define DATA_CONFIG_H

#include <stdbool.h>
#include <windows.h>

typedef enum {
    CFG_INT,
    CFG_FLOAT,
    CFG_FLOAT_MS, // Specialized for ms <-> s conversion (like release_time_ms)
    CFG_BOOL,
    CFG_NOTE_STRING,
    CFG_STRING
} ConfigType;

typedef enum {
    MENU_SEMITONE = 0,
    MENU_OCTAVE,
    MENU_SHOW_KEYBOARD,
    MENU_MASTER_VOLUME,
    MENU_PRESET,
    MENU_PITCH_DRIFT,
    MENU_VOL_DRIFT,
    MENU_ATTACK,
    MENU_DECAY,
    MENU_SUSTAIN,
    MENU_RELEASE,
    MENU_FILTER_CUTOFF,
    MENU_FILTER_Q,
    MENU_VIB_SPEED,
    MENU_VIB_DEPTH,
    MENU_VIB_MODE,
    MENU_RISE_TIME,
    MENU_TREM_SPEED,
    MENU_TREM_DEPTH,
    MENU_TREM_BIAS,
    MENU_DELAY_TIME,
    MENU_DELAY_MIX,
    MENU_DELAY_FB,
    MENU_DELAY_SAT,
    MENU_DELAY_MOD_SPEED,
    MENU_DELAY_MOD_DEPTH,
    MENU_WHEEL_ASSIGN,
    MENU_WHEEL_MODE,
    MENU_WHEEL_SENSE,
    MENU_OPTION_COUNT,
    MENU_OPTION_NONE = -1
} MenuOption;

typedef enum {
    WHEEL_ASSIGN_NONE = 0,
    WHEEL_ASSIGN_ANY,
    WHEEL_ASSIGN_MASTER,
    WHEEL_ASSIGN_CUTOFF,
    WHEEL_ASSIGN_COUNT
} WheelAssignment;

typedef enum {
    WHEEL_MODE_MOUSE = 0,
    WHEEL_MODE_PAD,
    WHEEL_MODE_COUNT
} WheelMode;

typedef struct {
    const char* key;
    void* var_ptr;
    ConfigType type;
    MenuOption menu_option;
    float min_value;
    float max_value;
    float step_value;
} ConfigEntry;

typedef struct {
    const char* row_label;
    const MenuOption* item_enums;
    int item_count;
} MenuRow;

typedef struct {
    const char* label;
    MenuOption target_option;
} WheelAssignmentOption;

// Menu state
extern int g_semitone;
extern int g_octave;
extern bool g_show_keyboard;
extern int g_current_row;
extern int g_current_col;
extern char g_current_preset[MAX_PATH];
extern char g_save_input[MAX_PATH];
extern int g_save_input_len;

// ADSR Envelope
extern float g_attack;
extern float g_decay;
extern float g_sustain;
extern float g_release_time;
extern float g_filter_cutoff_hz;
extern float g_filter_q;
extern int g_pitch_drift;
extern float g_vol_drift;

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

// Save Status
extern int g_save_status; // 0: Normal, 1: Confirming, 2: Saved
extern unsigned long g_save_status_time; // DWORD

// Key layout definitions
extern const int num_keys[4];
extern const int vk_map[4][16];
extern const char* key_display_names[4][16];
extern int row_starts[4];
extern int note_map[4][16];
extern bool key_state[4][16];
extern int active_notes[4][16];
extern const char* note_names[];
extern ConfigEntry g_config_registry[];
extern const int g_registry_size;
extern const MenuRow g_menu_layout[];
extern const int g_menu_layout_size;
extern const WheelAssignmentOption g_wheel_assignment_options[];
extern const int g_wheel_assignment_option_count;
extern int g_wheel_assign;
extern int g_wheel_mode;
extern float g_wheel_sense;
extern char** g_preset_files;
extern int g_preset_count;
extern int g_current_preset_index;

bool load_config(const char* filename);
bool save_config(const char* filename);
bool load_preset_with_name(const char* preset_name);
bool save_preset_with_name(const char* preset_name);
void scan_presets(void);
void free_presets(void);
bool cycle_current_preset(int direction);
void sync_current_preset_index(void);
void init_note_map();
int parse_note(const char* note_str);
void get_note_string(int midi_note, char* buf);
ConfigEntry* get_menu_config_entry(MenuOption option);
MenuOption get_current_menu_option(void);
MenuOption get_wheel_target_option(void);
const char* get_wheel_assignment_label(void);
const char* get_wheel_mode_label(void);
const char* get_current_preset_label(void);
void save_last_state(void);
bool make_unique_preset_name(const char* base_name, char* out_name, size_t out_size);

#endif // DATA_CONFIG_H
