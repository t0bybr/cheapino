// Copyright 2024 Toby
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Enhanced Cheapino Keymap with QMK Modern Features
// Features: Chordal Hold, Combos, Key Overrides, Leader Key, OS Detection
// NOTE: LED functionality disabled - was causing keyboard lockup

#include QMK_KEYBOARD_H
#include "os_detection.h"

// Layer definitions
enum layers {
    _BASE = 0,   // Colemak
    _MEDIA,      // Media controls
    _NAV,        // Navigation
    _MOUSE,      // Mouse keys
    _SYM_R,      // Symbols (right)
    _NUM,        // Numbers
    _FKEY,       // F-keys
    _EXTRA,      // Extra layer (Tri-layer: NAV + SYM)
};

// Custom keycodes
enum custom_keycodes {
    CUSTOM_START = SAFE_RANGE,
};

// Global state - homerow_mod_active removed (LED was causing lockup)

// Home Row Mods
// Left hand: A=GUI, R=RALT(AltGr), S=CTRL, T=SHIFT
#define HM_A LGUI_T(KC_A)
#define HM_R RALT_T(KC_R)  // AltGr for German umlauts
#define HM_S LCTL_T(KC_S)
#define HM_T LSFT_T(KC_T)

// Right hand: N=SHIFT, E=CTRL, I=ALT, O=GUI
#define HM_N RSFT_T(KC_N)
#define HM_E RCTL_T(KC_E)
#define HM_I RALT_T(KC_I)
#define HM_O RGUI_T(KC_O)

// Thumb keys with layer tap
#define ESC_MED LT(_MEDIA, KC_ESC)
#define SPC_NAV LT(_NAV, KC_SPC)
#define TAB_MOU LT(_MOUSE, KC_TAB)
#define ENT_SYM LT(_SYM_R, KC_ENT)
#define BSP_NUM LT(_NUM, KC_BSPC)
#define DEL_FKY LT(_FKEY, KC_DEL)  // Back to normal Del

// Keymaps
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x5_3(
  // Left hand
  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,
  KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT,
  HM_A,    HM_R,    HM_S,    HM_T,    KC_D,
           KC_H,    HM_N,    HM_E,
  HM_I,    HM_O,
  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
  KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
  ESC_MED, SPC_NAV, TAB_MOU,
  ENT_SYM, BSP_NUM, DEL_FKY
    ),

    [_MEDIA] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_LEFT, KC_MPRV, KC_VOLD,
  KC_VOLU, KC_MNXT,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  _______, KC_NO,   KC_NO,
  KC_MSTP, KC_MPLY, KC_MUTE
    ),

    [_NAV] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_PGUP, KC_UP,   KC_PGDN, KC_NO,
  KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, KC_NO,
           KC_HOME, KC_LEFT, KC_DOWN,
  KC_RGHT, KC_END,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   _______, KC_NO,
  ENT_SYM, KC_NO,   KC_NO     // ENT_SYM available for Tri-Layer
    ),

    [_MOUSE] = LAYOUT_split_3x5_3(
  // QMK Mouse Keys - Combined mode with acceleration
  KC_NO,   MS_ACL0, MS_WHLU, MS_ACL2, KC_NO,     // Slow, Wheel Up, Fast
  KC_NO,   MS_BTN1, MS_UP,   MS_BTN2, KC_NO,     // Left click, Mouse Up, Right click
  KC_NO,   MS_WHLL, MS_WHLD, MS_WHLR, KC_NO,     // Wheel Left/Down/Right
           MS_ACL1, MS_LEFT, MS_DOWN,             // Medium speed, Mouse Left, Down
  MS_RGHT, KC_NO,                                  // Mouse Right
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   _______,
  MS_BTN1, MS_BTN2, MS_BTN3  // Left, Right, Middle click
    ),

    [_SYM_R] = LAYOUT_split_3x5_3(
  S(KC_LBRC), S(KC_7), S(KC_8), S(KC_9), S(KC_RBRC),
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  S(KC_SCLN), S(KC_4), S(KC_5), S(KC_6), S(KC_EQL),
           KC_NO,   KC_RSFT, KC_RCTL,
  KC_RALT, KC_RGUI,
  S(KC_GRV), S(KC_1), S(KC_2), S(KC_3), S(KC_BSLS),
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  S(KC_9), SPC_NAV, S(KC_MINS),    // SPC_NAV available for Tri-Layer
  _______, KC_NO,   KC_NO
    ),

    [_NUM] = LAYOUT_split_3x5_3(
  KC_LBRC, KC_7,    KC_8,    KC_9,    KC_RBRC,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_SCLN, KC_4,    KC_5,    KC_6,    KC_PEQL,
           KC_NO,   KC_RSFT, KC_RCTL,
  KC_RALT, KC_RGUI,
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_BSLS,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  QK_REP,  KC_0,    KC_PMNS,
  KC_NO,   _______, KC_NO
    ),

    [_FKEY] = LAYOUT_split_3x5_3(
  KC_F12,  KC_F7,   KC_F8,   KC_F9,   KC_PSCR,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   QK_BOOT,
  KC_F11,  KC_F4,   KC_F5,   KC_F6,   KC_NO,
           KC_NO,   KC_RSFT, KC_RCTL,
  KC_RALT, KC_RGUI,
  KC_F10,  KC_F1,   KC_F2,   KC_F3,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_APP,  KC_NO,   QK_REP,
  KC_NO,   KC_NO,   _______
    ),

    [_EXTRA] = LAYOUT_split_3x5_3(
  // Tri-layer: NAV + SYM_R active = EXTRA layer
  // System functions, debugging, RGB controls, etc.
  QK_BOOT, KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  _______, _______, _______,
  _______, _______, _______
    ),
};

// ============================================================================
// COMBOS - OS-aware shortcuts
// ============================================================================

enum combo_events {
    COMBO_COPY,
    COMBO_PASTE,
    COMBO_CUT,
    COMBO_LEADER,      // ESC + DEL (outer thumbs) = Leader
};

// Define combo key sequences
const uint16_t PROGMEM combo_copy[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_paste[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_cut[] = {KC_Z, KC_X, COMBO_END};
const uint16_t PROGMEM combo_leader[] = {ESC_MED, DEL_FKY, COMBO_END};

// Combo definitions - actions handled in process_combo_event
combo_t key_combos[] = {
    [COMBO_COPY] = COMBO_ACTION(combo_copy),
    [COMBO_PASTE] = COMBO_ACTION(combo_paste),
    [COMBO_CUT] = COMBO_ACTION(combo_cut),
    [COMBO_LEADER] = COMBO(combo_leader, QK_LEAD),
};

// Combo actions - OS-aware using QMK OS Detection
void process_combo_event(uint16_t combo_index, bool pressed) {
    os_variant_t os = detected_host_os();
    bool is_macos = (os == OS_MACOS || os == OS_IOS);

    // Determine which modifier to use
    uint8_t mod = is_macos ? KC_LGUI : KC_LCTL;

    switch (combo_index) {
        case COMBO_COPY:
            if (pressed) {
                register_code(mod);
                register_code(KC_C);
            } else {
                unregister_code(KC_C);
                unregister_code(mod);
            }
            break;

        case COMBO_PASTE:
            if (pressed) {
                register_code(mod);
                register_code(KC_V);
            } else {
                unregister_code(KC_V);
                unregister_code(mod);
            }
            break;

        case COMBO_CUT:
            if (pressed) {
                register_code(mod);
                register_code(KC_X);
            } else {
                unregister_code(KC_X);
                unregister_code(mod);
            }
            break;
    }
}

// ============================================================================
// KEY OVERRIDES
// ============================================================================

// Shift + Backspace = Delete
const key_override_t shift_bspc_delete = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

// Shift + Esc = Tilde
const key_override_t shift_esc_tilde = ko_make_basic(MOD_MASK_SHIFT, KC_ESC, KC_TILDE);

// Array of all key overrides
const key_override_t *key_overrides[] = {
    &shift_bspc_delete,
    &shift_esc_tilde,
    NULL  // Terminator
};

// ============================================================================
// LEADER KEY - Shortcuts
// ============================================================================

void leader_end_user(void) {
    os_variant_t os = detected_host_os();
    bool is_macos = (os == OS_MACOS || os == OS_IOS);

    // Leader + S + P = App Launcher (OS-aware)
    if (leader_sequence_two_keys(KC_S, KC_P)) {
        if (is_macos) {
            tap_code16(LGUI(KC_SPC));  // Cmd+Space (Spotlight)
        } else {
            tap_code(KC_LGUI);  // Super (Linux launcher)
        }
    }

    // Leader + T + C = Terminal Ctrl+C
    if (leader_sequence_two_keys(KC_T, KC_C)) {
        tap_code16(LCTL(KC_C));
    }

    // TODO: Add more leader sequences as needed
    // Examples:
    // - Leader + G + I + T = "git status"
    // - Leader + E + M = Your email
    // - Leader + W + Q = Close window
}

// ============================================================================
// CHORDAL HOLD - Per-key configuration
// ============================================================================

// Chordal Hold is built into QMK - no external library needed!
// It automatically handles same-hand vs opposite-hand detection.
// We just need to configure per-key tapping behavior.

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Home row mods - standard tapping term
        case HM_A:
        case HM_O:
            return TAPPING_TERM + 30;  // GUI keys get slightly more time
        case HM_R:
        case HM_S:
        case HM_T:
        case HM_N:
        case HM_E:
        case HM_I:
            return TAPPING_TERM;
        // Thumb keys - longer tapping term for comfort
        case ESC_MED:
        case SPC_NAV:
        case TAB_MOU:
        case ENT_SYM:
        case BSP_NUM:
        case DEL_FKY:
            return TAPPING_TERM + 50;
        default:
            return TAPPING_TERM;
    }
}

// Per-key permissive hold
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Enable for thumb keys only (recommended with Chordal Hold)
        case ESC_MED:
        case SPC_NAV:
        case TAB_MOU:
        case ENT_SYM:
        case BSP_NUM:
        case DEL_FKY:
            return true;
        default:
            return false;
    }
}

// Per-key hold on other key press
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Enable for thumb layer keys for faster layer access
        // EXCEPT SPC_NAV - disabled to prevent accidental layer activation during fast typing
        case ESC_MED:
        case TAB_MOU:
        case ENT_SYM:
        case BSP_NUM:
        case DEL_FKY:
            return true;
        case SPC_NAV:
            return false;  // Disabled - prevents NAV layer activation when typing quickly
        default:
            return false;
    }
}

// Per-key quick tap term - enables fast repeat for Backspace
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Enable quick tap for Backspace - allows fast repeated taps
        case BSP_NUM:
            return QUICK_TAP_TERM;  // Use global value (default 100ms)
        default:
            return 0;  // Disabled for other keys
    }
}

// ============================================================================
// TRI-LAYER: NAV + SYM_R = EXTRA
// ============================================================================

layer_state_t layer_state_set_user(layer_state_t state) {
    // Tri-Layer: When NAV and SYM_R are both active, activate EXTRA
    state = update_tri_layer_state(state, _NAV, _SYM_R, _EXTRA);

    // LED functionality removed - was causing keyboard lockup

    return state;
}

// ============================================================================
// OS DETECTION - Active and working!
// ============================================================================
// OS Detection works fine - LED functions were the problem
// All shortcuts are now OS-aware (macOS vs Linux)

// ============================================================================
// MAIN PROCESSING
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Get current OS and mods
    os_variant_t os = detected_host_os();
    bool is_macos = (os == OS_MACOS || os == OS_IOS);
    uint8_t mods = get_mods();

    // Homerow mod LED removed - was causing keyboard lockup
    // LED calls in process_record_user() can block even with _noeeprom()

    // OS-aware modifier handling for Backspace
    if (keycode == BSP_NUM) {
        // Shift + Backspace = Delete with autorepeat
        if (mods & MOD_MASK_SHIFT) {
            if (record->event.pressed) {
                unregister_mods(MOD_MASK_SHIFT);
                register_code(KC_DEL);
            } else {
                unregister_code(KC_DEL);
                set_mods(mods);
            }
            return false;
        }

        // Ctrl + Backspace = Delete word backward (OS-aware)
        // Linux: Ctrl+Backspace, macOS: Option+Backspace
        if (mods & MOD_MASK_CTRL) {
            if (record->event.pressed) {
                // Clear all mods, send the correct sequence, restore mods
                clear_mods();
                send_keyboard_report();

                if (is_macos) {
                    tap_code16(LALT(KC_BSPC));  // Option+Backspace on macOS
                } else {
                    tap_code16(LCTL(KC_BSPC));  // Ctrl+Backspace on Linux
                }

                set_mods(mods);
                send_keyboard_report();
            }
            return false;  // Don't process BSP_NUM as layer-tap
        }
    }

    // OS-aware modifier handling for Delete
    if (keycode == DEL_FKY) {
        // Ctrl + Delete = Delete word forward (OS-aware)
        // Linux: Ctrl+Delete, macOS: Option+Delete
        if (mods & MOD_MASK_CTRL) {
            if (record->event.pressed) {
                // Clear all mods, send the correct sequence, restore mods
                clear_mods();
                send_keyboard_report();

                if (is_macos) {
                    tap_code16(LALT(KC_DEL));  // Option+Delete on macOS
                } else {
                    tap_code16(LCTL(KC_DEL));  // Ctrl+Delete on Linux
                }

                set_mods(mods);
                send_keyboard_report();
            }
            return false;  // Don't process DEL_FKY as layer-tap
        }
    }

    return true;
}

// Initialize - LED functionality removed
void keyboard_post_init_user(void) {
    // LED disabled - was causing keyboard lockup
}

// Matrix scan - LED functionality removed (was blocking keyboard)
void matrix_scan_user(void) {
    // Empty - LED was causing lockup
}
