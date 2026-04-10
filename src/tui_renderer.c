#include "../include/tui_renderer.h"
#include "../include/data_config.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>

static bool is_selected(MenuOption option) {
    return get_current_menu_option() == option;
}

static void append_plain_option(char** ptr, const char* label, const char* value) {
    *ptr += sprintf(*ptr, "%s%s", label, value);
}

static void append_reversed_option(char** ptr, const char* label_bg, const char* label, const char* value) {
    *ptr += sprintf(*ptr, "%s\033[30m%s\033[47;30m%s\033[0m", label_bg, label, value);
}

static void append_colored_option(char** ptr, MenuOption option, const char* color_fg, const char* color_bg, const char* label, const char* value) {
    if (is_selected(option)) {
        append_reversed_option(ptr, color_bg, label, value);
    } else {
        *ptr += sprintf(*ptr, "%s%s\033[37m%s\033[0m", color_fg, label, value);
    }
}

static void append_menu_option(char** ptr, MenuOption option) {
    char value_buf[32];

    switch (option) {
        case MENU_SEMITONE:
            sprintf(value_buf, "%+d", g_semitone);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Semitone: ", value_buf);
            } else {
                append_plain_option(ptr, "Semitone: ", value_buf);
            }
            break;
        case MENU_OCTAVE:
            sprintf(value_buf, "%+d", g_octave);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Octave: ", value_buf);
            } else {
                append_plain_option(ptr, "Octave: ", value_buf);
            }
            break;
        case MENU_SHOW_KEYBOARD:
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "ShowKbd: ", g_show_keyboard ? "ON " : "OFF");
            } else {
                append_plain_option(ptr, "ShowKbd: ", g_show_keyboard ? "ON " : "OFF");
            }
            break;
        case MENU_MASTER_VOLUME:
            sprintf(value_buf, "%d%%", (int)(g_master_volume * 100 + 0.5f));
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Master: ", value_buf);
            } else {
                append_plain_option(ptr, "Master: ", value_buf);
            }
            break;
        case MENU_PITCH_DRIFT:
            sprintf(value_buf, "%dc", g_pitch_drift);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "P-Drift:", value_buf);
            } else {
                append_plain_option(ptr, "P-Drift:", value_buf);
            }
            break;
        case MENU_VOL_DRIFT:
            sprintf(value_buf, "%.1f%%", g_vol_drift);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "V-Drift:", value_buf);
            } else {
                append_plain_option(ptr, "V-Drift:", value_buf);
            }
            break;
        case MENU_ATTACK:
            sprintf(value_buf, "%.2fs", g_attack);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "A:", value_buf);
            } else {
                append_plain_option(ptr, "A:", value_buf);
            }
            break;
        case MENU_DECAY:
            sprintf(value_buf, "%.2fs", g_decay);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "D:", value_buf);
            } else {
                append_plain_option(ptr, "D:", value_buf);
            }
            break;
        case MENU_SUSTAIN:
            sprintf(value_buf, "%.2f", g_sustain);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "S:", value_buf);
            } else {
                append_plain_option(ptr, "S:", value_buf);
            }
            break;
        case MENU_RELEASE:
            sprintf(value_buf, "%.2fs", g_release_time);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "R:", value_buf);
            } else {
                append_plain_option(ptr, "R:", value_buf);
            }
            break;
        case MENU_FILTER_CUTOFF:
            sprintf(value_buf, "%.0fHz", g_filter_cutoff_hz);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "LP:", value_buf);
            } else {
                append_plain_option(ptr, "LP:", value_buf);
            }
            break;
        case MENU_FILTER_Q:
            sprintf(value_buf, "%.2f", g_filter_q);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Q:", value_buf);
            } else {
                append_plain_option(ptr, "Q:", value_buf);
            }
            break;
        case MENU_VIB_SPEED:
            sprintf(value_buf, "%.1fHz", g_vib_speed);
            append_colored_option(ptr, option, "\033[96m", "\033[106m", "Spd:", value_buf);
            break;
        case MENU_VIB_DEPTH:
            sprintf(value_buf, "%dc", g_vib_depth);
            append_colored_option(ptr, option, "\033[96m", "\033[106m", "Dep:", value_buf);
            break;
        case MENU_VIB_MODE:
            append_colored_option(ptr, option, "\033[96m", "\033[106m", "Mode:", !g_vib_mode ? "Unlatch" : "Latch  ");
            break;
        case MENU_RISE_TIME:
            sprintf(value_buf, "%.1fs", g_rise_time);
            append_colored_option(ptr, option, "\033[96m", "\033[106m", "Rise:", value_buf);
            break;
        case MENU_TREM_SPEED:
            sprintf(value_buf, "%.1fHz", g_trem_speed);
            append_colored_option(ptr, option, "\033[93m", "\033[103m", "Spd:", value_buf);
            break;
        case MENU_TREM_DEPTH:
            sprintf(value_buf, "%dc", g_trem_depth);
            append_colored_option(ptr, option, "\033[93m", "\033[103m", "Dep:", value_buf);
            break;
        case MENU_TREM_BIAS:
            sprintf(value_buf, "%d%%", g_trem_bias);
            append_colored_option(ptr, option, "\033[93m", "\033[103m", "Bias:", value_buf);
            break;
        case MENU_DELAY_TIME:
            sprintf(value_buf, "%dms", (int)(g_delay_time * 1000));
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "T:", value_buf);
            break;
        case MENU_DELAY_MIX:
            sprintf(value_buf, "%d%%", g_delay_mix);
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "M:", value_buf);
            break;
        case MENU_DELAY_FB:
            sprintf(value_buf, "%d%%", g_delay_fb);
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "FB:", value_buf);
            break;
        case MENU_DELAY_SAT:
            sprintf(value_buf, "%d%%", g_delay_sat);
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "Sa:", value_buf);
            break;
        case MENU_DELAY_MOD_SPEED:
            sprintf(value_buf, "%.1fHz", g_delay_mod_spd);
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "Sp:", value_buf);
            break;
        case MENU_DELAY_MOD_DEPTH:
            sprintf(value_buf, "%dc", g_delay_mod_dep);
            append_colored_option(ptr, option, "\033[92m", "\033[102m", "Dp:", value_buf);
            break;
        case MENU_WHEEL_ASSIGN:
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Assign:", get_wheel_assignment_label());
            } else {
                append_plain_option(ptr, "Assign:", get_wheel_assignment_label());
            }
            break;
        case MENU_WHEEL_MODE:
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Mode:", get_wheel_mode_label());
            } else {
                append_plain_option(ptr, "Mode:", get_wheel_mode_label());
            }
            break;
        case MENU_WHEEL_SENSE:
            sprintf(value_buf, "%.2f", g_wheel_sense);
            if (is_selected(option)) {
                append_reversed_option(ptr, "\033[47m", "Sense:", value_buf);
            } else {
                append_plain_option(ptr, "Sense:", value_buf);
            }
            break;
        case MENU_OPTION_COUNT:
        case MENU_OPTION_NONE:
            break;
    }
}

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
    
    ptr += sprintf(ptr, "\033[2K  ------------------SettingsMenu------------------\n");
    for (int row = 0; row < g_menu_layout_size; row++) {
        const MenuRow* menu_row = &g_menu_layout[row];
        ptr += sprintf(ptr, "\033[2K");
        if (strcmp(menu_row->row_label, "\"VB-2\":") == 0) {
            ptr += sprintf(ptr, "\033[96m%s\033[0m ", menu_row->row_label);
        } else if (strcmp(menu_row->row_label, "\"Trelicopter\":") == 0) {
            ptr += sprintf(ptr, "\033[93m%s\033[0m ", menu_row->row_label);
        } else if (strcmp(menu_row->row_label, "\"RE-20\":") == 0) {
            ptr += sprintf(ptr, "\033[92m%s\033[0m ", menu_row->row_label);
        } else {
            ptr += sprintf(ptr, "%s: ", menu_row->row_label);
        }

        for (int col = 0; col < menu_row->item_count; col++) {
            append_menu_option(&ptr, menu_row->item_enums[col]);
            if (col < menu_row->item_count - 1) {
                ptr += sprintf(ptr, "  ");
            }
        }
        ptr += sprintf(ptr, "\n");
    }

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
    } else {
        ptr += sprintf(ptr, "\033[0J");
    }
    
    ptr += sprintf(ptr, "\n\n\033[2K");
    if (g_save_status == 1) {
        ptr += sprintf(ptr, "\033[103;30m [!] SAVE PRESET \033[0m\033[93m Press ENTER to confirm saving to config.ini, or ESC to cancel.\033[0m\n");
    } else if (g_save_status == 2) {
        ptr += sprintf(ptr, "\033[102;30m [+] SAVED \033[0m\033[92m Successfully saved to config.ini\033[0m\n");
    } else {
        ptr += sprintf(ptr, "\033[90mNAV:↕↔  ADJ:Ctrl+↕  MOD:Wheel  SAV:Ctrl+S  ESC:Exit\033[0m\n");
    }
    
    ptr += sprintf(ptr, "\033[0J"); // Clear all leftover lines below this point to handle resize/toggle
    
    // Output all at once to prevent flickering
    fwrite(out_buf, 1, ptr - out_buf, stdout);
    fflush(stdout);
}
