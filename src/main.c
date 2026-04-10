#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

#include "../include/data_config.h"
#include "../include/audio_engine.h"
#include "../include/tui_renderer.h"

bool g_running = true;

#define MOUSE_WHEEL_STEP 120.0f
#define MOUSE_INPUT_BATCH_SIZE 32

static float clampf(float value, float min_value, float max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static bool menu_option_needs_synth_update(MenuOption option) {
    switch (option) {
        case MENU_ATTACK:
        case MENU_DECAY:
        case MENU_SUSTAIN:
        case MENU_RELEASE:
        case MENU_FILTER_CUTOFF:
        case MENU_FILTER_Q:
        case MENU_VIB_SPEED:
        case MENU_VIB_DEPTH:
            return true;
        default:
            return false;
    }
}

static float adjust_logarithmic_value(float current, float min_value, float max_value, float normalized_step) {
    float safe_current = clampf(current, min_value, max_value);
    float log_min = logf(min_value);
    float log_max = logf(max_value);
    float t = (logf(safe_current) - log_min) / (log_max - log_min);
    t = clampf(t + normalized_step, 0.0f, 1.0f);
    return expf(log_min + (log_max - log_min) * t);
}

static void adjust_menu_option(MenuOption option, float delta_units) {
    ConfigEntry* entry = get_menu_config_entry(option);
    if (!entry) return;

    switch (option) {
        case MENU_SHOW_KEYBOARD:
        case MENU_VIB_MODE:
            *(bool*)entry->var_ptr = !(*(bool*)entry->var_ptr);
            break;
        case MENU_WHEEL_ASSIGN: {
            int next_value = *(int*)entry->var_ptr + (delta_units > 0.0f ? 1 : -1);
            if (next_value < 0) next_value = g_wheel_assignment_option_count - 1;
            if (next_value >= g_wheel_assignment_option_count) next_value = 0;
            *(int*)entry->var_ptr = next_value;
            break;
        }
        case MENU_WHEEL_MODE: {
            int next_value = *(int*)entry->var_ptr + (delta_units > 0.0f ? 1 : -1);
            if (next_value < 0) next_value = WHEEL_MODE_COUNT - 1;
            if (next_value >= WHEEL_MODE_COUNT) next_value = 0;
            *(int*)entry->var_ptr = next_value;
            break;
        }
        case MENU_FILTER_CUTOFF: {
            float next_value = adjust_logarithmic_value(
                *(float*)entry->var_ptr,
                entry->min_value,
                entry->max_value,
                entry->step_value * delta_units
            );
            *(float*)entry->var_ptr = clampf(next_value, entry->min_value, entry->max_value);
            break;
        }
        default:
            switch (entry->type) {
                case CFG_INT: {
                    int step_delta = (int)lroundf(entry->step_value * delta_units);
                    int next_value = *(int*)entry->var_ptr + step_delta;
                    next_value = (int)clampf((float)next_value, entry->min_value, entry->max_value);
                    *(int*)entry->var_ptr = next_value;
                    break;
                }
                case CFG_FLOAT:
                case CFG_FLOAT_MS: {
                    float next_value = *(float*)entry->var_ptr + entry->step_value * delta_units;
                    *(float*)entry->var_ptr = clampf(next_value, entry->min_value, entry->max_value);
                    break;
                }
                case CFG_BOOL:
                    *(bool*)entry->var_ptr = !(*(bool*)entry->var_ptr);
                    break;
                case CFG_NOTE_STRING:
                    break;
            }
            break;
    }

    if (menu_option_needs_synth_update(option)) {
        update_synth_params();
    }
}

static void process_mouse_wheel(HANDLE hIn) {
    DWORD event_count = 0;
    INPUT_RECORD records[MOUSE_INPUT_BATCH_SIZE];

    if (!GetNumberOfConsoleInputEvents(hIn, &event_count) || event_count == 0) {
        return;
    }

    while (event_count > 0) {
        DWORD records_to_read = event_count > MOUSE_INPUT_BATCH_SIZE ? MOUSE_INPUT_BATCH_SIZE : event_count;
        DWORD records_read = 0;
        if (!ReadConsoleInput(hIn, records, records_to_read, &records_read) || records_read == 0) {
            break;
        }

        for (DWORD i = 0; i < records_read; i++) {
            INPUT_RECORD* record = &records[i];
            if (record->EventType != MOUSE_EVENT) {
                continue;
            }

            MOUSE_EVENT_RECORD mouse = record->Event.MouseEvent;
            if (mouse.dwEventFlags != MOUSE_WHEELED) {
                continue;
            }

            MenuOption target_option = get_wheel_target_option();
            if (target_option == MENU_OPTION_COUNT) {
                target_option = get_current_menu_option();
            }
            if (target_option == MENU_OPTION_NONE) {
                continue;
            }

            short wheel_delta = (short)HIWORD(mouse.dwButtonState);
            float wheel_steps = ((float)wheel_delta / MOUSE_WHEEL_STEP) * g_wheel_sense;
            if (g_wheel_mode == WHEEL_MODE_PAD) {
                wheel_steps = -wheel_steps;
            }
            if (wheel_steps != 0.0f) {
                adjust_menu_option(target_option, wheel_steps);
                print_tui();
            }
        }

        if (!GetNumberOfConsoleInputEvents(hIn, &event_count)) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(65001);
    // Enable ANSI escape codes on Windows and disable console echo/input
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwOutMode = 0;
    GetConsoleMode(hOut, &dwOutMode);
    SetConsoleMode(hOut, dwOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwInMode = 0;
    GetConsoleMode(hIn, &dwInMode);
    // Disable line editing, keep mouse events, and turn off quick edit so wheel input reaches the app.
    DWORD desiredInMode = dwInMode;
    desiredInMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_QUICK_EDIT_MODE);
    desiredInMode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
    SetConsoleMode(hIn, desiredInMode);

    // 1. Fallback load default config from possible locations
    if (!load_config("config.ini")) {
        if (!load_config("../config.ini")) {
            load_config("IsomorphicKeyboard/config.ini");
        }
    }

    // 2. Parse command line arguments for specific config
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 && i+1 < argc) {
            load_config(argv[++i]);
        }
    }
    
    // 3. Parse command line parameter overrides
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "row0=", 5) == 0) row_starts[0] = parse_note(argv[i]+5);
        else if (strncmp(argv[i], "row1=", 5) == 0) row_starts[1] = parse_note(argv[i]+5);
        else if (strncmp(argv[i], "row2=", 5) == 0) row_starts[2] = parse_note(argv[i]+5);
        else if (strncmp(argv[i], "row3=", 5) == 0) row_starts[3] = parse_note(argv[i]+5);
    }
    init_note_map();
    
    if (!init_audio_engine()) {
        return 1;
    }

    // Clear screen for TUI
    printf("\033[2J");
    print_tui();

    // Use timeBeginPeriod to ensure low latency sleep
    timeBeginPeriod(1);

    // Main loop
    DWORD last_render_time = timeGetTime();
    const DWORD render_interval_ms = 1000 / 60; // 60Hz refresh rate
    
    while (g_running) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            if (g_save_status == 1) {
                g_save_status = 0;
                print_tui();
                Sleep(200); // debounce
                continue;
            } else {
                g_running = false;
                break;
            }
        }
        
        DWORD current_time = timeGetTime();
        process_mouse_wheel(hIn);
        
        // Save preset handling
        bool is_ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool is_s = (GetAsyncKeyState('S') & 0x8000) != 0;
        static bool s_prev = false;

        if (g_save_status == 1) {
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                save_config("config.ini");
                g_save_status = 2;
                g_save_status_time = current_time;
                print_tui();
                Sleep(200); // debounce
            }
        } else {
            if (is_ctrl && is_s && !s_prev) {
                g_save_status = 1;
                print_tui();
            }
        }
        s_prev = is_s;
        
        if (g_save_status == 2 && current_time - g_save_status_time > 2000) {
            g_save_status = 0;
            print_tui();
        }

        // Menu control
        if (g_save_status == 0 || g_save_status == 2) {
            bool is_up = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
            bool is_down = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
            bool is_left = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
            bool is_right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
            bool is_ctrl_nav = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

            static int left_held = 0;
            static int right_held = 0;
            static int up_held = 0;
            static int down_held = 0;

            if (is_left) left_held++; else left_held = 0;
            if (is_right) right_held++; else right_held = 0;
            if (is_up) up_held++; else up_held = 0;
            if (is_down) down_held++; else down_held = 0;

            bool trigger_left = (left_held == 1) || (left_held > 400 && left_held % 30 == 0);
            bool trigger_right = (right_held == 1) || (right_held > 400 && right_held % 30 == 0);
            bool trigger_up = (up_held == 1) || (up_held > 400 && up_held % 30 == 0);
            bool trigger_down = (down_held == 1) || (down_held > 400 && down_held % 30 == 0);
            bool menu_changed = false;

            if (trigger_left) {
                g_current_col--;
                if (g_current_col < 0) {
                    g_current_col = g_menu_layout[g_current_row].item_count - 1;
                }
                menu_changed = true;
            }
            if (trigger_right) {
                g_current_col++;
                if (g_current_col >= g_menu_layout[g_current_row].item_count) {
                    g_current_col = 0;
                }
                menu_changed = true;
            }

            if (trigger_up) {
                if (is_ctrl_nav) {
                    adjust_menu_option(get_current_menu_option(), 1.0f);
                } else {
                    g_current_row--;
                    if (g_current_row < 0) g_current_row = g_menu_layout_size - 1;
                    if (g_current_col >= g_menu_layout[g_current_row].item_count) {
                        g_current_col = g_menu_layout[g_current_row].item_count - 1;
                    }
                }
                menu_changed = true;
            }
            if (trigger_down) {
                if (is_ctrl_nav) {
                    adjust_menu_option(get_current_menu_option(), -1.0f);
                } else {
                    g_current_row++;
                    if (g_current_row >= g_menu_layout_size) g_current_row = 0;
                    if (g_current_col >= g_menu_layout[g_current_row].item_count) {
                        g_current_col = g_menu_layout[g_current_row].item_count - 1;
                    }
                }
                menu_changed = true;
            }

            if (menu_changed) {
                print_tui();
            }
        } // End of Menu control if block

        static bool g_space_prev = false;
        bool is_space = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        if (g_vib_mode == 0) { // Unlatch
            g_vib_enabled = is_space;
        } else { // Latch
            if (is_space && !g_space_prev) {
                g_vib_enabled = !g_vib_enabled;
            }
        }
        g_space_prev = is_space;
        
        static bool g_lshift_prev = false;
        bool is_lshift = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0;
        if (is_lshift && !g_lshift_prev) {
            g_trem_enabled = !g_trem_enabled;
        }
        g_lshift_prev = is_lshift;
        
        static bool g_lalt_prev = false;
        bool is_lalt = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
        if (is_lalt && !g_lalt_prev) {
            g_delay_enabled = !g_delay_enabled;
        }
        g_lalt_prev = is_lalt;
        
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < num_keys[r]; c++) {
                bool is_down = (GetAsyncKeyState(vk_map[r][c]) & 0x8000) != 0;
                
                if (is_down && !key_state[r][c]) {
                    key_state[r][c] = true;
                    
                    int actual_note = note_map[r][c] + g_semitone + (g_octave * 12);
                    if (actual_note < 0) actual_note = 0;
                    if (actual_note > 127) actual_note = 127;
                    active_notes[r][c] = actual_note;
                    
                    note_on(actual_note);
                    
                } else if (!is_down && key_state[r][c]) {
                    key_state[r][c] = false;
                    
                    if (active_notes[r][c] != -1) {
                        note_off(active_notes[r][c]);
                        active_notes[r][c] = -1;
                    }
                }
            }
        }
        
        if (current_time - last_render_time >= render_interval_ms) {
            // Update Rise
            if (g_vib_enabled) {
                if (g_rise_time > 0.001f) {
                    g_current_fade += (render_interval_ms / 1000.0f) / g_rise_time;
                    if (g_current_fade > 1.0f) g_current_fade = 1.0f;
                } else {
                    g_current_fade = 1.0f;
                }
            } else {
                g_current_fade -= (render_interval_ms / 1000.0f) / 0.1f; // Quick 100ms fade out to avoid clicks
                if (g_current_fade < 0.0f) g_current_fade = 0.0f;
            }
            update_synth_params(); // Apply dynamic depth

            print_tui();
            last_render_time = current_time;
        }
        
        Sleep(1); // 1ms sleep for low latency input polling
    }

    timeEndPeriod(1);

    // Show cursor and restore console mode before exit
    printf("\033[?25h\n");
    SetConsoleMode(hIn, dwInMode);

    cleanup_audio_engine();

    return 0;
}
