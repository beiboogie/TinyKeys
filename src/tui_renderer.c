#include "../include/tui_renderer.h"
#include "../include/data_config.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>

// Update TUI display
void print_tui() {
    char out_buf[8192];
    char* ptr = out_buf;
    
    // Hide cursor and clear/move
    ptr += sprintf(ptr, "\033[?25l\033[1;1H");
    
    const char* colors[] = {"\033[91m", "\033[93m", "\033[96m", "\033[92m"};
    
    ptr += sprintf(ptr, "\033[2K  -------------------NowPlaying-------------------\n");
    for (int r = 0; r < 4; r++) {
        ptr += sprintf(ptr, "\033[2K%sRow %d: ", colors[r], r);
        for (int c = 0; c < num_keys[r]; c++) {
            if (key_state[r][c]) {
                char buf[16];
                int preview_note = note_map[r][c] + g_semitone + (g_octave * 12);
                if (preview_note < 0) preview_note = 0;
                if (preview_note > 127) preview_note = 127;
                get_note_string(preview_note, buf);
                ptr += sprintf(ptr, "%s ", buf);
            }
        }
        ptr += sprintf(ptr, "\033[0m\n");
    }
    
    ptr += sprintf(ptr, "\033[2K  ------------------SettingsMenu------------------\n\033[2K");
    if (g_menu_selection == 0) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "Semitone: %+d", g_semitone);
    if (g_menu_selection == 0) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "  ");
    
    if (g_menu_selection == 1) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "Octave: %+d", g_octave);
    if (g_menu_selection == 1) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "  ");

    if (g_menu_selection == 2) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "ShowKbd: %s", g_show_keyboard ? "ON " : "OFF");
    if (g_menu_selection == 2) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "  ");

    if (g_menu_selection == 3) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "Master: %d%%", (int)(g_master_volume * 100 + 0.5f));
    if (g_menu_selection == 3) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "\n\033[2K");

    if (g_menu_selection == 4) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "A:%.2fs", g_attack);
    if (g_menu_selection == 4) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "       ");

    if (g_menu_selection == 5) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "D:%.2fs", g_decay);
    if (g_menu_selection == 5) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "     ");

    if (g_menu_selection == 6) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "S:%.2f", g_sustain);
    if (g_menu_selection == 6) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "       ");

    if (g_menu_selection == 7) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "R:%.2fs", g_release_time);
    if (g_menu_selection == 7) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "\n\033[2K");
    
    ptr += sprintf(ptr, "\033[96m\"VB-2\":\033[0m ");
    if (g_menu_selection == 8) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[96mSpd:\033[0m");
    if (g_menu_selection == 8) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%.1fHz", g_vib_speed);
    if (g_menu_selection == 8) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 9) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[96mDep:\033[0m");
    if (g_menu_selection == 9) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%dc", g_vib_depth);
    if (g_menu_selection == 9) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "   ");

    if (g_menu_selection == 10) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[96mMode:\033[0m");
    if (g_menu_selection == 10) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%s", !g_vib_mode ? "Unlatch" : "Latch  ");
    if (g_menu_selection == 10) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 11) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[96mRise:\033[0m");
    if (g_menu_selection == 11) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%.1fs", g_rise_time);
    if (g_menu_selection == 11) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "\n\033[2K");

    ptr += sprintf(ptr, "\033[93m\"Trelicopter\":\033[0m    ");
    if (g_menu_selection == 12) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[93mSpd:\033[0m");
    if (g_menu_selection == 12) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%.1fHz", g_trem_speed);
    if (g_menu_selection == 12) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 13) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[93mDep:\033[0m");
    if (g_menu_selection == 13) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%dc", g_trem_depth);
    if (g_menu_selection == 13) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "      ");

    if (g_menu_selection == 14) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[93mBias:\033[0m");
    if (g_menu_selection == 14) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%d%%", g_trem_bias);
    if (g_menu_selection == 14) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "\n\033[2K");
    
    ptr += sprintf(ptr, "\033[92m\"RE-20\":\033[0m ");
    if (g_menu_selection == 15) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mT:\033[0m");
    if (g_menu_selection == 15) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%dms", (int)(g_delay_time * 1000));
    if (g_menu_selection == 15) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 16) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mM:\033[0m");
    if (g_menu_selection == 16) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%d%%", g_delay_mix);
    if (g_menu_selection == 16) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 17) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mFB:\033[0m");
    if (g_menu_selection == 17) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%d%%", g_delay_fb);
    if (g_menu_selection == 17) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 18) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mSa:\033[0m");
    if (g_menu_selection == 18) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%d%%", g_delay_sat);
    if (g_menu_selection == 18) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");
    
    if (g_menu_selection == 19) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mSp:\033[0m");
    if (g_menu_selection == 19) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%.1fHz", g_delay_mod_spd);
    if (g_menu_selection == 19) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, " ");

    if (g_menu_selection == 20) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "\033[92mDp:\033[0m");
    if (g_menu_selection == 20) ptr += sprintf(ptr, "\033[7m");
    ptr += sprintf(ptr, "%dc", g_delay_mod_dep);
    if (g_menu_selection == 20) ptr += sprintf(ptr, "\033[0m");
    ptr += sprintf(ptr, "\n\033[2K");

    // Visualizers
    // Vibrato Visualizer
    if (g_vib_enabled) {
        float effective_depth = g_current_fade; // 0.0 to 1.0
        // LFO value from -1.0 to 1.0. We don't have exact phase here, so we simulate it with time for visualizer.
        // Actually, let's use timeGetTime to drive the visualizer independently to keep it simple, or we can share phase.
        // Let's share the time-based phase for VB-2 since we don't have it exported.
        DWORD time = timeGetTime();
        float phase = fmodf((float)time / 1000.0f * g_vib_speed, 1.0f);
        float lfo_val = sinf(phase * 2.0f * 3.14159265f); // -1.0 to 1.0
        float pos = lfo_val * effective_depth; // -1.0 to 1.0
        int pos_int = (int)(pos * 6.0f); // -6 to +6
        ptr += sprintf(ptr, "\033[96m[");
        for (int i = -6; i <= 6; i++) {
            if (i == pos_int) ptr += sprintf(ptr, "|");
            else ptr += sprintf(ptr, "-");
        }
        ptr += sprintf(ptr, "]\033[0m   ");
    } else {
        ptr += sprintf(ptr, "\033[90m[-------------]\033[0m   ");
    }

    // Tremolo Visualizer
    if (g_trem_enabled) {
        float p = g_trem_phase;
        float B = g_trem_bias / 100.0f;
        if (B < 0.01f) B = 0.01f;
        if (B > 0.99f) B = 0.99f;
        
        float mapped_p;
        if (p < B) {
            mapped_p = 0.5f * (p / B);
        } else {
            mapped_p = 0.5f + 0.5f * ((p - B) / (1.0f - B));
        }
        float wave = 0.5f - 0.5f * cosf(mapped_p * 2.0f * 3.14159265f); // 0.0 to 1.0
        int pos_int = (int)(wave * 12.0f); // 0 to 12
        ptr += sprintf(ptr, "\033[93m[");
        for (int i = 0; i <= 12; i++) {
            if (i == pos_int) ptr += sprintf(ptr, "|");
            else ptr += sprintf(ptr, "-");
        }
        ptr += sprintf(ptr, "]\033[0m   ");
    } else {
        ptr += sprintf(ptr, "\033[90m[-------------]\033[0m   ");
    }

    // Delay Visualizer
    if (g_delay_enabled) {
        int pos_int = (int)(g_delay_lfo_phase * 12.0f); // 0 to 12
        ptr += sprintf(ptr, "\033[92m[");
        for (int i = 0; i <= 12; i++) {
            if (i == pos_int) ptr += sprintf(ptr, ">");
            else ptr += sprintf(ptr, "-");
        }
        ptr += sprintf(ptr, "]\033[0m\n");
    } else {
        ptr += sprintf(ptr, "\033[90m[-------------]\033[0m\n");
    }
    
    if (g_show_keyboard) {
        ptr += sprintf(ptr, "\033[2K  ------------------------------------------------\n");
        const char* indents[4] = {"", "  ", "   ", "    "};
        for (int r = 0; r < 4; r++) {
            ptr += sprintf(ptr, "\033[2K");
            if (r == 3) {
                if (g_trem_enabled) {
                    ptr += sprintf(ptr, "\033[93m[SH]\033[0m ");
                } else {
                    ptr += sprintf(ptr, "\033[90m[SH]\033[0m ");
                }
            } else {
                ptr += sprintf(ptr, "%s", indents[r]);
            }
            
            for (int c = 0; c < num_keys[r]; c++) {
                if (key_state[r][c]) {
                    ptr += sprintf(ptr, "%s[%s]\033[0m ", colors[r], key_display_names[r][c]);
                } else {
                    ptr += sprintf(ptr, "\033[90m[%s]\033[0m ", key_display_names[r][c]); // Gray color for unpressed keys
                }
            }
            ptr += sprintf(ptr, "\n");
        }
        
        ptr += sprintf(ptr, "\033[2K");
        if (g_delay_enabled) {
            ptr += sprintf(ptr, "\033[92m[ALT]\033[0m     ");
        } else {
            ptr += sprintf(ptr, "\033[90m[ALT]\033[0m     ");
        }
        if (g_vib_enabled) {
            ptr += sprintf(ptr, "\033[96m[          SPACE          ]\033[0m\n");
        } else {
            ptr += sprintf(ptr, "\033[90m[          SPACE          ]\033[0m\n");
        }
    }
    
    ptr += sprintf(ptr, "\n\n\033[2K");
    if (g_save_status == 1) {
        ptr += sprintf(ptr, "\033[103;30m [!] SAVE PRESET \033[0m\033[93m Press ENTER to confirm saving to config.ini, or ESC to cancel.\033[0m\n");
    } else if (g_save_status == 2) {
        ptr += sprintf(ptr, "\033[102;30m [+] SAVED \033[0m\033[92m Successfully saved to config.ini\033[0m\n");
    } else {
        ptr += sprintf(ptr, "\033[90mPress CTRL+S to save preset. Press ESC to exit.\033[0m\n");
    }
    
    ptr += sprintf(ptr, "\033[0J"); // Clear all leftover lines below this point to handle resize/toggle
    
    // Output all at once to prevent flickering
    fwrite(out_buf, 1, ptr - out_buf, stdout);
    fflush(stdout);
}