#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <mmsystem.h>

#include "../include/data_config.h"
#include "../include/audio_engine.h"
#include "../include/tui_renderer.h"

bool g_running = true;

int main(int argc, char *argv[]) {
    // Enable ANSI escape codes on Windows and disable console echo/input
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwOutMode = 0;
    GetConsoleMode(hOut, &dwOutMode);
    SetConsoleMode(hOut, dwOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwInMode = 0;
    GetConsoleMode(hIn, &dwInMode);
    // Disable ENABLE_ECHO_INPUT and ENABLE_LINE_INPUT so keystrokes aren't printed to the terminal
    SetConsoleMode(hIn, dwInMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

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
            g_running = false;
            break;
        }
        
        // Menu control
        bool is_up = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
        bool is_down_menu = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
        bool is_left = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
        bool is_right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

        static int left_held = 0;
        static int right_held = 0;
        if (is_left) left_held++; else left_held = 0;
        if (is_right) right_held++; else right_held = 0;

        bool trigger_left = (left_held == 1) || (left_held > 400 && left_held % 30 == 0);
        bool trigger_right = (right_held == 1) || (right_held > 400 && right_held % 30 == 0);

        if (trigger_left) {
            g_menu_selection--;
            if (g_menu_selection < 0) g_menu_selection = 20;
            print_tui(); // Force UI update on selection change
        }
        if (trigger_right) {
            g_menu_selection++;
            if (g_menu_selection > 20) g_menu_selection = 0;
            print_tui(); // Force UI update on selection change
        }

        static int up_held = 0;
        static int down_held = 0;
        if (is_up) up_held++; else up_held = 0;
        if (is_down_menu) down_held++; else down_held = 0;

        bool trigger_up = (up_held == 1) || (up_held > 400 && up_held % 30 == 0);
        bool trigger_down = (down_held == 1) || (down_held > 400 && down_held % 30 == 0);

        if (trigger_up) {
            if (g_menu_selection == 0) g_semitone++;
            else if (g_menu_selection == 1) g_octave++;
            else if (g_menu_selection == 2) g_show_keyboard = !g_show_keyboard;
            else if (g_menu_selection == 3) { g_master_volume += 0.05f; if (g_master_volume > 2.0f) g_master_volume = 2.0f; update_synth_params(); }
            else if (g_menu_selection == 4) { g_attack += 0.01f; if (g_attack > 2.0f) g_attack = 2.0f; update_synth_params(); }
            else if (g_menu_selection == 5) { g_decay += 0.25f; if (g_decay > 10.0f) g_decay = 10.0f; update_synth_params(); }
            else if (g_menu_selection == 6) { g_sustain += 0.05f; if (g_sustain > 1.0f) g_sustain = 1.0f; update_synth_params(); }
            else if (g_menu_selection == 7) { g_release_time += 0.25f; if (g_release_time > 5.0f) g_release_time = 5.0f; update_synth_params(); }
            else if (g_menu_selection == 8) { g_vib_speed += 0.1f; if (g_vib_speed > 15.0f) g_vib_speed = 15.0f; update_synth_params(); }
            else if (g_menu_selection == 9) { g_vib_depth += 2; if (g_vib_depth > 100) g_vib_depth = 100; update_synth_params(); }
            else if (g_menu_selection == 10) { g_vib_mode = !g_vib_mode; }
            else if (g_menu_selection == 11) { g_rise_time += 0.5f; if (g_rise_time > 5.0f) g_rise_time = 5.0f; }
            else if (g_menu_selection == 12) { g_trem_speed += 0.1f; if (g_trem_speed > 15.0f) g_trem_speed = 15.0f; }
            else if (g_menu_selection == 13) { g_trem_depth += 2; if (g_trem_depth > 100) g_trem_depth = 100; }
            else if (g_menu_selection == 14) { g_trem_bias += 5; if (g_trem_bias > 100) g_trem_bias = 100; }
            else if (g_menu_selection == 15) { g_delay_time += 0.025f; if (g_delay_time > 2.0f) g_delay_time = 2.0f; }
            else if (g_menu_selection == 16) { g_delay_mix += 10; if (g_delay_mix > 100) g_delay_mix = 100; }
            else if (g_menu_selection == 17) { g_delay_fb += 10; if (g_delay_fb > 100) g_delay_fb = 100; }
            else if (g_menu_selection == 18) { g_delay_sat += 10; if (g_delay_sat > 100) g_delay_sat = 100; }
            else if (g_menu_selection == 19) { g_delay_mod_spd += 0.1f; if (g_delay_mod_spd > 15.0f) g_delay_mod_spd = 15.0f; }
            else if (g_menu_selection == 20) { g_delay_mod_dep += 1; if (g_delay_mod_dep > 100) g_delay_mod_dep = 100; }
            print_tui(); // Force UI update on menu change
        }
        if (trigger_down) {
            if (g_menu_selection == 0) g_semitone--;
            else if (g_menu_selection == 1) g_octave--;
            else if (g_menu_selection == 2) g_show_keyboard = !g_show_keyboard;
            else if (g_menu_selection == 3) { g_master_volume -= 0.05f; if (g_master_volume < 0.0f) g_master_volume = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 4) { g_attack -= 0.01f; if (g_attack < 0.0f) g_attack = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 5) { g_decay -= 0.25f; if (g_decay < 0.0f) g_decay = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 6) { g_sustain -= 0.05f; if (g_sustain < 0.0f) g_sustain = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 7) { g_release_time -= 0.25f; if (g_release_time < 0.0f) g_release_time = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 8) { g_vib_speed -= 0.1f; if (g_vib_speed < 0.0f) g_vib_speed = 0.0f; update_synth_params(); }
            else if (g_menu_selection == 9) { g_vib_depth -= 2; if (g_vib_depth < 0) g_vib_depth = 0; update_synth_params(); }
            else if (g_menu_selection == 10) { g_vib_mode = !g_vib_mode; }
            else if (g_menu_selection == 11) { g_rise_time -= 0.5f; if (g_rise_time < 0.0f) g_rise_time = 0.0f; }
            else if (g_menu_selection == 12) { g_trem_speed -= 0.1f; if (g_trem_speed < 0.0f) g_trem_speed = 0.0f; }
            else if (g_menu_selection == 13) { g_trem_depth -= 2; if (g_trem_depth < 0) g_trem_depth = 0; }
            else if (g_menu_selection == 14) { g_trem_bias -= 5; if (g_trem_bias < 0) g_trem_bias = 0; }
            else if (g_menu_selection == 15) { g_delay_time -= 0.025f; if (g_delay_time < 0.1f) g_delay_time = 0.1f; }
            else if (g_menu_selection == 16) { g_delay_mix -= 10; if (g_delay_mix < 0) g_delay_mix = 0; }
            else if (g_menu_selection == 17) { g_delay_fb -= 10; if (g_delay_fb < 0) g_delay_fb = 0; }
            else if (g_menu_selection == 18) { g_delay_sat -= 10; if (g_delay_sat < 0) g_delay_sat = 0; }
            else if (g_menu_selection == 19) { g_delay_mod_spd -= 0.1f; if (g_delay_mod_spd < 0.0f) g_delay_mod_spd = 0.0f; }
            else if (g_menu_selection == 20) { g_delay_mod_dep -= 1; if (g_delay_mod_dep < 0) g_delay_mod_dep = 0; }
            print_tui(); // Force UI update on menu change
        }

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
        
        static bool g_lctrl_prev = false;
        bool is_lctrl = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
        if (is_lctrl && !g_lctrl_prev) {
            g_delay_enabled = !g_delay_enabled;
        }
        g_lctrl_prev = is_lctrl;
        
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
        
        DWORD current_time = timeGetTime();
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