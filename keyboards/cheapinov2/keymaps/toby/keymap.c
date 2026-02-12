// Copyright 2024 Toby
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Enhanced Cheapino Keymap with QMK Modern Features
// Features: Chordal Hold, Combos, Key Overrides, Leader Key, OS Detection
// LED: Non-blocking rgblight layers + deferred OS flash

#include QMK_KEYBOARD_H
#include "os_detection.h"
#include "rgblight.h"
#include "deferred_exec.h"
#include "leader.h"
#include "timer.h"
#include "wait.h"
#include "toby_keycodes.h"

// Guard window to avoid unintended BSPC quick-tap repeat after other keys
#ifndef BSP_QT_GUARD_MS
#define BSP_QT_GUARD_MS 180
#endif

#ifndef DEL_HOLD_VISUAL_MS
#define DEL_HOLD_VISUAL_MS 140
#endif

#ifndef CMD_HOLD_HUE
#define CMD_HOLD_HUE 149
#endif

// (reserved) quick-tap guard timestamp (not used)
// static uint16_t bsp_qt_block_time = 0;

// App switcher toggle state
static bool app_sw_toggled = false;
static uint16_t app_sw_last_tab_time = 0;
static deferred_token app_sw_token = 0;
static uint16_t app_sw_mod_for_os(void) {
    os_variant_t os = detected_host_os();
    if (os == OS_MACOS || os == OS_IOS) return KC_LGUI;   // Cmd
    if (os == OS_WINDOWS) return KC_LALT;                  // Alt
    return LINUX_APP_SWITCH_MOD;                           // Linux configurable
}
static uint32_t app_sw_autorelease_cb(uint32_t t, void *arg) {
    (void)t; (void)arg;
    if (app_sw_toggled && timer_elapsed(app_sw_last_tab_time) >= APP_SW_AUTORELEASE_MS) {
        unregister_code(app_sw_mod_for_os());
        app_sw_toggled = false;
        send_keyboard_report();
        return 0;
    }
    return 200; // keep checking
}

// Command layer helpers (hold DEL_FKY)
static void cmd_copy_os_aware(void) {
    os_variant_t os = detected_host_os();
    if (os == OS_MACOS || os == OS_IOS) {
        tap_code16(LGUI(KC_C));       // Cmd+C
    } else {
        tap_code16(C(S(KC_C)));       // Ctrl+Shift+C (terminal-first)
    }
}

static void cmd_paste_os_aware(void) {
    os_variant_t os = detected_host_os();
    if (os == OS_MACOS || os == OS_IOS) {
        tap_code16(LGUI(KC_V));       // Cmd+V
    } else {
        tap_code16(C(S(KC_V)));       // Ctrl+Shift+V (terminal-first)
    }
}

static void cmd_interrupt(void) {
    tap_code16(C(KC_C));              // Ctrl+C (SIGINT in terminal)
}

// Mouse ACL: use QMK defaults (MS_ACL keys are momentary by default).
// No custom interception here to ensure the built-in accel stages take effect.

// Force NUM layer while BSP_NUM held with second key — handled by BSPC state machine now

// Forward declaration for LED layer application
static void apply_layer_color(layer_state_t state);

// Track thumb LT state (for conditional HOOKP on SPC_NAV)
static bool bsp_num_pressed = false;

// Leader overlay state (white LED during leader timeout)
static bool leader_overlay_active = false;
static uint8_t leader_len = 0;  // track length to auto-complete sequences

// DEL_FKY hybrid state (tap leader, hold command layer)
static bool del_fky_pressed = false;
static bool del_fky_used_as_hold = false;
static bool del_fky_hold_visual = false;
static deferred_token del_fky_hold_visual_token = 0;

static uint32_t del_fky_hold_visual_cb(uint32_t t, void *arg) {
    (void)t;
    (void)arg;
#ifdef RGBLIGHT_LAYERS
    if (del_fky_pressed && !leader_sequence_active()) {
        // Crossing the hold threshold counts as a hold, not a tap-leader.
        del_fky_used_as_hold = true;
        del_fky_hold_visual = true;
        apply_layer_color(layer_state);
    }
#endif
    return 0;
}

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
    _CMD,        // Command hold layer (hold DEL_FKY)
};

// Global state - homerow_mod_active removed (LED was causing lockup)

// Home Row Mods
// Left hand: A=GUI, R=LALT, S=CTRL, T=SHIFT
#define HM_A LGUI_T(KC_A)
#define HM_R LALT_T(KC_R)
#define HM_S LCTL_T(KC_S)
#define HM_T LSFT_T(KC_T)

// Right hand: N=SHIFT, E=CTRL, I=RALT, O=GUI
#define HM_N RSFT_T(KC_N)
#define HM_E RCTL_T(KC_E)
#define HM_I RALT_T(KC_I)
#define HM_O RGUI_T(KC_O)

// Unicode map for German umlauts (CMD layer: DEL hold + A/O/U/S)
enum unicode_names {
    U_AE_L, // ä
    U_AE_U, // Ä
    U_OE_L, // ö
    U_OE_U, // Ö
    U_UE_L, // ü
    U_UE_U, // Ü
    U_SS_L, // ß
    U_SS_U, // ẞ
};

const uint32_t PROGMEM unicode_map[] = {
    [U_AE_L] = 0x00E4, // ä
    [U_AE_U] = 0x00C4, // Ä
    [U_OE_L] = 0x00F6, // ö
    [U_OE_U] = 0x00D6, // Ö
    [U_UE_L] = 0x00FC, // ü
    [U_UE_U] = 0x00DC, // Ü
    [U_SS_L] = 0x00DF, // ß
    [U_SS_U] = 0x1E9E, // ẞ
};

// Thumb keys with layer tap
#define ESC_MED LT(_MEDIA, KC_ESC)
#define SPC_NAV LT(_NAV, KC_SPC)
#define TAB_MOU LT(_MOUSE, KC_TAB)
#define ENT_SYM LT(_SYM_R, KC_ENT)
#define BSP_NUM LT(_NUM, KC_BSPC)
// DEL_FKY hybrid:
// tap = QMK Leader, hold = Command layer, double-tap = app leader trigger
#define DEL_FKY DEL_CMD

// Keymaps
// Source of truth: this userspace keymap is used for builds via overlay_dir=/qmk_userspace.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Physical order (Cheapino split_3x5_3):
    // L00 L01 L02 L03 L04 | R00 R01 R02 R03 R04
    // L10 L11 L12 L13 L14 | R10 R11 R12 R13 R14
    // L20 L21 L22 L23 L24 | R20 R21 R22 R23 R24
    //          L30 L31 L32 | R30 R31 R32
    [_BASE] = LAYOUT_split_3x5_3(
        KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,                      KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT,
        HM_A,    HM_R,    HM_S,    HM_T,    KC_D,                      KC_H,    HM_N,    HM_E,    HM_I,    HM_O,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                      KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                                   ESC_MED, SPC_NAV, TAB_MOU, ENT_SYM, BSP_NUM, DEL_FKY
    ),

    [_MEDIA] = LAYOUT_split_3x5_3(
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        _______, _______, _______, _______, _______,                   KC_LEFT, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   _______, KC_NO,   KC_NO,   KC_MSTP, KC_MPLY, KC_MUTE
    ),

    [_NAV] = LAYOUT_split_3x5_3(
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_PGUP, KC_UP,   KC_PGDN, KC_NO,
        _______, _______, _______, _______, _______,                   KC_HOME, KC_LEFT, KC_DOWN, KC_RGHT, KC_END,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   KC_NO,   _______, KC_NO,   APP_SW_TOG, APP_SW_TAB, APP_SW_PREV
    ),

    [_MOUSE] = LAYOUT_split_3x5_3(
        // Left: S/T modifiers placed on home row positions; right mirrors NAV wheel/cursor
        KC_NO,   KC_NO,   M_TGL_MOD, M_DBL_MOD, KC_NO,                 KC_NO,   MS_WHLU, MS_UP,   MS_WHLD, KC_NO,
        _______, _______, _______,   _______,   _______,               MS_WHLL, MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLR,
        KC_NO,   MS_ACL0, MS_ACL1,   MS_ACL2,   KC_NO,                 KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   KC_NO,   KC_NO,   _______, M_MBTN3, MS_BTN1, MS_BTN2
    ),

    [_SYM_R] = LAYOUT_split_3x5_3(
        S(KC_LBRC), S(KC_7), S(KC_8), S(KC_9), S(KC_RBRC),             KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        S(KC_SCLN), S(KC_4), S(KC_5), S(KC_6), S(KC_EQL),            _______,   _______, _______, _______, _______,
        S(KC_GRV),  S(KC_1), S(KC_2), S(KC_3), S(KC_BSLS),             KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   S(KC_9), S(KC_0), S(KC_MINS), _______, BSP_NUM, DEL_FKY
    ),

    [_NUM] = LAYOUT_split_3x5_3(
        KC_LBRC, KC_7,    KC_8,    KC_9,    KC_RBRC,                   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_SCLN, KC_4,    KC_5,    KC_6,    KC_PEQL,                   _______,   _______, _______, _______, _______,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_BSLS,                   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   QK_REP,  KC_0,    KC_PMNS, ENT_SYM, _______, DEL_FKY
    ),

    [_FKEY] = LAYOUT_split_3x5_3(
        KC_F12,  KC_F7,   KC_F8,   KC_F9,   KC_PSCR,                   KC_NO,   KC_NO,   KC_NO,   AC_TOGG, QK_BOOT,
        KC_F11,  KC_F4,   KC_F5,   KC_F6,   KC_NO,                     _______, _______, _______, _______, _______,
        // Right bottom row (K M , . /): Linux TTY switch
        KC_F10,  KC_F1,   KC_F2,   KC_F3,   KC_NO,                     C(A(KC_F1)), C(A(KC_F2)), C(A(KC_F3)), C(A(KC_F4)), C(A(KC_F5)),
                                   KC_APP,  KC_NO,   QK_REP, _______,  _______, _______
    ),

    [_EXTRA] = LAYOUT_split_3x5_3(
        // Tri-layer: NAV + SYM_R active = EXTRA layer
        // System functions, debugging, RGB controls, etc.
        QK_BOOT, AC_TOGG, KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                   _______, _______, _______, _______, _______, _______
    ),

    [_CMD] = LAYOUT_split_3x5_3(
        // Hold DEL_FKY and press:
        // P=paste, Y=copy, ESC=interrupt
        // A=ä, O=ö, U=ü, S=ß (+ Shift for uppercase)
        KC_NO,           KC_NO,   KC_NO,           CMD_PASTE, KC_NO,        KC_NO,   KC_NO,           UP(U_UE_L,U_UE_U), CMD_COPY,        KC_NO,
        UP(U_AE_L,U_AE_U), _______, UP(U_SS_L,U_SS_U), _______, _______,     _______,  _______,         _______,            _______,         UP(U_OE_L,U_OE_U),
        KC_NO,           KC_NO,   KC_NO,           KC_NO,     KC_NO,        KC_NO,   KC_NO,           KC_NO,              KC_NO,           KC_NO,
                                                   CMD_INTR,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
};

// ============================================================================
// COMBOS - OS-aware shortcuts (Leader combo removed; DEL is now Leader)
// ============================================================================

enum combo_events {
    COMBO_COPY,
    COMBO_PASTE,
    COMBO_CUT,
};

// Define combo key sequences
const uint16_t PROGMEM combo_copy[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_paste[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM combo_cut[] = {KC_Z, KC_X, COMBO_END};

// Combo definitions - actions handled in process_combo_event
combo_t key_combos[] = {
    [COMBO_COPY] = COMBO_ACTION(combo_copy),
    [COMBO_PASTE] = COMBO_ACTION(combo_paste),
    [COMBO_CUT] = COMBO_ACTION(combo_cut),
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

void leader_start_user(void) {
#ifdef RGBLIGHT_LAYERS
    leader_overlay_active = true;
    // White at configured brightness while leader is active
    rgblight_sethsv_noeeprom(0, 0, LED_BRIGHTNESS);
#endif
    leader_len = 0;
}

void leader_end_user(void) {
    os_variant_t os = detected_host_os();
    bool is_macos = (os == OS_MACOS || os == OS_IOS);

    // Delegate actual actions to the leader module
    extern void leader_handle_sequences(bool is_macos);
    leader_handle_sequences(is_macos);

    // Turn off leader overlay and restore layer color
    leader_overlay_active = false;
    apply_layer_color(layer_state);
}

bool leader_add_user(uint16_t keycode) {
    leader_len++;
    // If DEL is hit again during leader wait, end immediately and treat as DEL,DEL.
    if (keycode == DEL_FKY) {
        return true;
    }
    // Default: auto-complete after two keys.
    return leader_len >= 2;
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
        case DEL_FKY:
            return true;
        case BSP_NUM:
            // Disable HOOKP for BSPC/NUM to avoid unintended NUM activation after quick BSPC + next key
            return false;
        default:
            return false;
    }
}

// Per-key hold on other key press
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Enable for thumb layer keys for faster layer access
        case ESC_MED:
        case TAB_MOU:
        case ENT_SYM:
        case BSP_NUM:
        case DEL_FKY:
            return true;
        case SPC_NAV:
            // Keep disabled to avoid accidental NAV; tri-layer uses ENT_SYM + BSP_NUM
            return false;
        default:
            return false;
    }
}

// Per-key quick tap term - enables fast repeat for Backspace with guard
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case BSP_NUM:
            // Managed by BSPC state machine; disable quick-tap repeat
            return 0;
        default:
            return 0;
    }
}

// ============================================================================
// RGBLIGHT - Layer colors (non-blocking) + OS flash on boot
// ============================================================================

#ifdef RGBLIGHT_LAYERS
// Per-layer HSV helpers (H:0-255, S:0-255, V:0-255)
static inline uint8_t hsv_h_for_layer(uint8_t layer) {
    switch (layer) {
        // Arrange hues to avoid similar adjacent colors
        case _MEDIA: return 127; // Cyan (~180°)
        case _NAV:   return 21;  // Orange (~30°)
        case _MOUSE: return 85;  // Green (~120°)
        case _SYM_R: return 170; // Blue (~240°)
        case _NUM:   return 212; // Magenta (~300°)
        case _FKEY:  return 43;  // Yellow (~60°)
        case _EXTRA: return 233; // Rose (~330°)
        case _CMD:
        case _BASE:
        default:     return 0;   // Off (S ignored, V=0)
    }
}

static inline uint8_t hsv_s_for_layer(uint8_t layer) {
    switch (layer) {
        case _CMD:
        case _BASE:  return 0;   // Off when V=0
        default:     return 255; // Fully saturated
    }
}

static inline uint8_t hsv_v_for_layer(uint8_t layer) {
    // Global brightness from config; Base/CMD layers off
    return (layer == _BASE || layer == _CMD) ? 0 : LED_BRIGHTNESS;
}

// RGBlight layer segments (1 LED total). We also set HSV directly for precision.
const rgblight_segment_t PROGMEM layer_base[]  = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_WHITE});
const rgblight_segment_t PROGMEM layer_media[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_CYAN});
const rgblight_segment_t PROGMEM layer_nav[]   = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_GREEN});
const rgblight_segment_t PROGMEM layer_mouse[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_PURPLE});
const rgblight_segment_t PROGMEM layer_sym[]   = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_YELLOW});
const rgblight_segment_t PROGMEM layer_num[]   = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_ORANGE});
const rgblight_segment_t PROGMEM layer_fkey[]  = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_MAGENTA});
const rgblight_segment_t PROGMEM layer_extra[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_RED});

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    layer_base,
    layer_media,
    layer_nav,
    layer_mouse,
    layer_sym,
    layer_num,
    layer_fkey,
    layer_extra
);

static void rgblight_layers_all_off(void) {
    for (uint8_t i = 0; i < 8; i++) {
        rgblight_set_layer_state(i, false);
    }
}

static bool hrm_overlay_active = false;
// HRM overlay state
enum { HRM_A_IDX, HRM_R_IDX, HRM_S_IDX, HRM_T_IDX, HRM_N_IDX, HRM_E_IDX, HRM_I_IDX, HRM_O_IDX, HRM_COUNT };
static bool hrm_pressed[HRM_COUNT] = {0};
static bool hrm_is_hold[HRM_COUNT] = {0};
static uint8_t hrm_active_count = 0;
static deferred_token hrm_tokens[HRM_COUNT] = {0};
static void same_app_window_cycle(bool reverse) {
    os_variant_t os = detected_host_os();
    uint16_t mod_primary = KC_LALT;
    uint16_t key = KC_GRV; // backtick
    bool add_shift = reverse;

    if (os == OS_MACOS || os == OS_IOS) {
        mod_primary = KC_LGUI; // Cmd + `
        key = KC_GRV;
    } else if (os == OS_WINDOWS) {
        // Windows: kein natives same-app cycling → Alt+Esc als globaler Fallback
        mod_primary = KC_LALT;
        key = KC_ESC;
        add_shift = false;
    }

    if (add_shift) register_code(KC_LSFT);
    register_code(mod_primary);
    tap_code(key);
    unregister_code(mod_primary);
    if (add_shift) unregister_code(KC_LSFT);
}

static int hrm_index_for(uint16_t kc) {
    switch (kc) {
        case HM_A: return HRM_A_IDX;
        case HM_R: return HRM_R_IDX;
        case HM_S: return HRM_S_IDX;
        case HM_T: return HRM_T_IDX;
        case HM_N: return HRM_N_IDX;
        case HM_E: return HRM_E_IDX;
        case HM_I: return HRM_I_IDX;
        case HM_O: return HRM_O_IDX;
        default:   return -1;
    }
}

static uint32_t hrm_hold_cb(uint32_t trigger_time, void *cb_arg) {
    (void)trigger_time;
    uintptr_t idx = (uintptr_t)cb_arg;
    if (idx >= HRM_COUNT) return 0;
    if (hrm_pressed[idx] && !hrm_is_hold[idx]) {
        hrm_is_hold[idx] = true;
        if (hrm_active_count == 0) {
            // Activate overlay: random-ish hue, full sat, configured homerow brightness
            uint8_t hue = (uint8_t)((timer_read() * 37u + (uint32_t)idx * 53u) & 0xFFu);
            hrm_overlay_active = true;
            rgblight_sethsv_noeeprom(hue, 255, LED_BRIGHTNESS_HOMEROW);
        }
        hrm_active_count++;
    }
    return 0; // one-shot
}

// --- SHIFT+Backspace → Delete hold support (robust across HRM/Shift timing) ---
static bool hm_t_pressed = false;
static bool hm_n_pressed = false;
// Track HRM GUI/CTRL too for robust chord detection before mod bits latch
static bool hm_a_pressed = false; // LGUI on A
static bool hm_o_pressed = false; // RGUI on O
static bool hm_s_pressed = false; // LCTL on S
static bool hm_e_pressed = false; // RCTL on E
static bool bsp_del_active = false;
static uint8_t bsp_del_saved_mods = 0;

// --- BSPC State Machine (tap/hold, double-tap->hold, triple-tap->repeat) ---
static bool     bsp_pressed = false;
// legacy flag removed; rely on LT for hold
static uint8_t  bsp_tap_count = 0;
static uint16_t bsp_last_tap_time = 0;
static bool     bsp_repeat_active = false;
#ifndef BSPC_TRIPLE_HOLD_MS
#define BSPC_TRIPLE_HOLD_MS 80
#endif
static bool     bsp_triple_pending = false;
static deferred_token bsp_triple_hold_token = 0;
static deferred_token bsp_repeat_token = 0;

#ifndef BSPC_HOLD_MS
#define BSPC_HOLD_MS TAPPING_TERM
#endif
#ifndef BSPC_REPEAT_INTERVAL_MS
#define BSPC_REPEAT_INTERVAL_MS 35
#endif
#ifndef BSPC_TRIPLE_TERM_MS
#define BSPC_TRIPLE_TERM_MS 400
#endif

// static uint32_t bsp_hold_cb(uint32_t t, void *arg) { return 0; }

static uint32_t bsp_repeat_cb(uint32_t t, void *arg) {
    (void)t; (void)arg;
    if (bsp_repeat_active && bsp_pressed) {
        tap_code(KC_BSPC);
        return BSPC_REPEAT_INTERVAL_MS;
    }
    return 0;
}

static uint32_t bsp_triple_hold_cb(uint32_t t, void *arg) {
	(void)t; (void)arg;
	if (bsp_triple_pending && bsp_pressed && !bsp_repeat_active) {
		bsp_repeat_active = true;
		bsp_repeat_token = defer_exec(BSPC_REPEAT_INTERVAL_MS, bsp_repeat_cb, NULL);
		bsp_triple_pending = false;
	}
	return 0;
}

static void apply_layer_color(layer_state_t state) {
    if (leader_overlay_active) {
        // Leader overlay has highest priority: white while waiting
        rgblight_sethsv_noeeprom(0, 0, LED_BRIGHTNESS);
        return;
    }
    if (hrm_overlay_active) {
        // Preserve HRM overlay color until HRM released
        return;
    }
    if (del_fky_hold_visual) {
        // DEL hold-command feedback color
        rgblight_sethsv_noeeprom(CMD_HOLD_HUE, 255, LED_BRIGHTNESS);
        return;
    }
    uint8_t top = get_highest_layer(state);
    rgblight_sethsv_noeeprom(hsv_h_for_layer(top), hsv_s_for_layer(top), hsv_v_for_layer(top));

    rgblight_layers_all_off();
    switch (top) {
        case _MEDIA: rgblight_set_layer_state(1, true); break;
        case _NAV:   rgblight_set_layer_state(2, true); break;
        case _MOUSE: rgblight_set_layer_state(3, true); break;
        case _SYM_R: rgblight_set_layer_state(4, true); break;
        case _NUM:   rgblight_set_layer_state(5, true); break;
        case _FKEY:  rgblight_set_layer_state(6, true); break;
        case _EXTRA: rgblight_set_layer_state(7, true); break;
        case _CMD:
        case _BASE:
        default:     /* base off */ break;
    }
}

static uint32_t os_flash_done_cb(uint32_t trigger_time, void *cb_arg) {
    (void)trigger_time;
    (void)cb_arg;
    apply_layer_color(layer_state);
    return 0; // stop
}
#endif // RGBLIGHT_LAYERS

// Wait until OS detection becomes known, then configure modifier swap and LED flash
static uint32_t os_setup_wait_cb(uint32_t trigger_time, void *cb_arg) {
    (void)trigger_time;
    (void)cb_arg;
    static uint8_t attempts = 0;
    attempts++;

    os_variant_t os = detected_host_os();
    if (os == OS_UNSURE) {
        if (attempts < 30) {
            return 200; // retry
        } else {
            // Give up; just apply current layer color
#ifdef RGBLIGHT_LAYERS
            apply_layer_color(layer_state);
#endif
            return 0;
        }
    }

    bool is_macos = (os == OS_MACOS || os == OS_IOS);
    keymap_config.swap_lctl_lgui = is_macos;
    keymap_config.swap_rctl_rgui = is_macos;

#ifdef RGBLIGHT_LAYERS
    // Show OS color for 800ms at configured brightness, then restore
    // Windows: BSOD blue, macOS: white, Linux: Ubuntu purple
    if (os == OS_WINDOWS) {
        rgblight_sethsv_noeeprom(170, 255, LED_BRIGHTNESS); // Deep blue
    } else if (is_macos) {
        rgblight_sethsv_noeeprom(0, 0, LED_BRIGHTNESS);     // White
    } else {
        rgblight_sethsv_noeeprom(197, 255, LED_BRIGHTNESS); // Ubuntu purple (~280°)
    }
    defer_exec(800, os_flash_done_cb, NULL);
#endif
    return 0;
}

// ============================================================================
// TRI-LAYER: SYM_R + NUM = FKEY + LED updates on state change
// ============================================================================

layer_state_t layer_state_set_user(layer_state_t state) {
    // Tri-Layer: When SYM_R and NUM are both active, activate FKEY
    state = update_tri_layer_state(state, _SYM_R, _NUM, _FKEY);

    // No custom ACL handling here (use QMK defaults)

#ifdef RGBLIGHT_LAYERS
    apply_layer_color(state);
#endif

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
    // DEL_FKY hybrid:
    // tap => start QMK Leader, hold => _CMD layer while held.
    // While DEL is held, any other pressed key marks it as hold-use and
    // immediately enables hold-color feedback.
    if (del_fky_pressed && keycode != DEL_FKY && record->event.pressed) {
        if (!del_fky_used_as_hold) {
            del_fky_used_as_hold = true;
            del_fky_hold_visual = true;
            cancel_deferred_exec(del_fky_hold_visual_token);
#ifdef RGBLIGHT_LAYERS
            apply_layer_color(layer_state);
#endif
        }
    }

    if (keycode == DEL_FKY) {
        // If Leader is already active, pass DEL through so Leader can record DEL,DEL.
        if (leader_sequence_active()) {
            return true;
        }

        if (record->event.pressed) {
            del_fky_pressed = true;
            del_fky_used_as_hold = false;
            del_fky_hold_visual = false;
            layer_on(_CMD);
            cancel_deferred_exec(del_fky_hold_visual_token);
            del_fky_hold_visual_token = defer_exec(DEL_HOLD_VISUAL_MS, del_fky_hold_visual_cb, NULL);
        } else {
            // DEL press may have been consumed by active Leader processing.
            if (!del_fky_pressed) {
                return false;
            }
            cancel_deferred_exec(del_fky_hold_visual_token);
            del_fky_hold_visual = false;
            layer_off(_CMD);
            del_fky_pressed = false;
            if (!del_fky_used_as_hold) {
                leader_start();
            }
        }
        return false;
    }

    // BSPC quick-tap guard disabled (state machine handles behavior now)
    // Track BSP_NUM pressed state (for conditional HOOKP on SPC_NAV)
    if (keycode == BSP_NUM) {
        bsp_num_pressed = record->event.pressed;
    }

    // HRM overlay handling: random color at 50 when HRM is held
    int hrm_idx = hrm_index_for(keycode);
    if (hrm_idx >= 0) {
        if (record->event.pressed) {
            hrm_pressed[hrm_idx] = true;
            // schedule hold detection after approx tapping term
            uint16_t delay = TAPPING_TERM + 35; // close to per-key config
            hrm_tokens[hrm_idx] = defer_exec(delay, hrm_hold_cb, (void *)(uintptr_t)hrm_idx);
        } else {
            cancel_deferred_exec(hrm_tokens[hrm_idx]);
            hrm_pressed[hrm_idx] = false;
            if (hrm_is_hold[hrm_idx]) {
                hrm_is_hold[hrm_idx] = false;
                if (hrm_active_count > 0) {
                    hrm_active_count--;
                    if (hrm_active_count == 0) {
                        hrm_overlay_active = false;
                        apply_layer_color(layer_state);
                    }
                }
            }
        }
    }

    // If any HRM is currently pressed, and another key is pressed on the
    // opposing hand, emulate chordal-hold instant commit by activating the overlay
    if (record->event.pressed) {
        // Determine side of current key by row heuristic (based on matrix.c)
        uint8_t row = record->event.key.row;
        bool key_is_right = (row <= 2);
        bool key_is_left  = (row >= 4 && row <= 6);

        if (key_is_left || key_is_right) {
            // Promote opposing-hand HRMs to hold immediately for LED overlay
            // Left HRMs: A,R,S,T,D; Right HRMs: N,E,I,O,H
            bool any_promoted = false;
            if (key_is_left) {
                // Promote right-hand HRMs
                int rights[] = {HRM_N_IDX, HRM_E_IDX, HRM_I_IDX, HRM_O_IDX};
                for (uint8_t i = 0; i < 4; i++) {
                    int idx = rights[i];
                    if (hrm_pressed[idx] && !hrm_is_hold[idx]) {
                        hrm_is_hold[idx] = true;
                        any_promoted = true;
                        cancel_deferred_exec(hrm_tokens[idx]);
                        hrm_tokens[idx] = 0;
                    }
                }
            } else if (key_is_right) {
                // Promote left-hand HRMs
                int lefts[] = {HRM_A_IDX, HRM_R_IDX, HRM_S_IDX, HRM_T_IDX};
                for (uint8_t i = 0; i < 4; i++) {
                    int idx = lefts[i];
                    if (hrm_pressed[idx] && !hrm_is_hold[idx]) {
                        hrm_is_hold[idx] = true;
                        any_promoted = true;
                        cancel_deferred_exec(hrm_tokens[idx]);
                        hrm_tokens[idx] = 0;
                    }
                }
            }

            if (any_promoted) {
                if (hrm_active_count == 0) {
                    uint8_t hue = (uint8_t)((timer_read() * 37u + 123u) & 0xFFu);
                    hrm_overlay_active = true;
                    rgblight_sethsv_noeeprom(hue, 255, LED_BRIGHTNESS_HOMEROW);
                }
                // Count all newly-promoted HRMs
                // We can't know exactly how many just now; recompute count:
                hrm_active_count = 0;
                // Lefts
                int lefts[] = {HRM_A_IDX, HRM_R_IDX, HRM_S_IDX, HRM_T_IDX};
                for (uint8_t i = 0; i < 4; i++) if (hrm_is_hold[lefts[i]]) hrm_active_count++;
                // Rights
                int rights2[] = {HRM_N_IDX, HRM_E_IDX, HRM_I_IDX, HRM_O_IDX};
                for (uint8_t i = 0; i < 4; i++) if (hrm_is_hold[rights2[i]]) hrm_active_count++;
            }
        }
    }

    if (keycode == HM_T) {
        hm_t_pressed = record->event.pressed;
    } else if (keycode == HM_N) {
        hm_n_pressed = record->event.pressed;
    } else if (keycode == HM_A) {
        hm_a_pressed = record->event.pressed;
    } else if (keycode == HM_O) {
        hm_o_pressed = record->event.pressed;
    } else if (keycode == HM_S) {
        hm_s_pressed = record->event.pressed;
    } else if (keycode == HM_E) {
        hm_e_pressed = record->event.pressed;
    }

    // BSP_NUM custom handling: only triple-tap+hold = Backspace repeat.
    // Otherwise, let LT(_NUM, KC_BSPC) handle tap/hold semantics.
    if (keycode == BSP_NUM) {
        uint8_t mods_now = get_mods();
        bool shift_effective = ((mods_now & MOD_MASK_SHIFT) != 0) || hm_t_pressed || hm_n_pressed;
        bool gui_effective = ((mods_now & (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI))) != 0) || hm_a_pressed || hm_o_pressed;
        bool ctrl_effective = ((mods_now & MOD_MASK_CTRL) != 0) || hm_s_pressed || hm_e_pressed;
        bool is_macos = (detected_host_os() == OS_MACOS || detected_host_os() == OS_IOS);
        bool wordmod_effective = is_macos ? gui_effective : ctrl_effective;

        if (record->event.pressed) {
            // Word-delete chords (OS-aware) take precedence
            if (wordmod_effective && shift_effective) {
                uint8_t saved = get_mods();
                clear_mods(); send_keyboard_report();
                if (is_macos) tap_code16(LALT(KC_DEL)); else tap_code16(LCTL(KC_DEL));
                set_mods(saved); send_keyboard_report();
                return false;
            }
            if (wordmod_effective && !shift_effective) {
                uint8_t saved = get_mods();
                clear_mods(); send_keyboard_report();
                if (is_macos) tap_code16(LALT(KC_BSPC)); else tap_code16(LCTL(KC_BSPC));
                set_mods(saved); send_keyboard_report();
                return false;
            }
            // Shift+Backspace -> hold Delete until release
            if (shift_effective) {
                bsp_del_saved_mods = mods_now;
                if (bsp_del_saved_mods & MOD_MASK_SHIFT) { unregister_mods(MOD_MASK_SHIFT); send_keyboard_report(); }
                register_code(KC_DEL);
                bsp_del_active = true;
                return false;
            }

            // Triple-tap detection to start BSPC auto-repeat; otherwise let LT handle
            bsp_pressed = true;
            if (timer_elapsed(bsp_last_tap_time) < BSPC_TRIPLE_TERM_MS) {
                bsp_tap_count++;
            } else {
                bsp_tap_count = 1;
            }
            bsp_last_tap_time = timer_read();

			if (bsp_tap_count >= 3) {
				bsp_triple_pending = true;
				bsp_repeat_active = false;
				cancel_deferred_exec(bsp_triple_hold_token);
				bsp_triple_hold_token = defer_exec(BSPC_TRIPLE_HOLD_MS, bsp_triple_hold_cb, NULL);
				return false; // warten auf Hold, kein sofortiger Repeat
			}

			return true; // LT(_NUM, KC_BSPC) macht Tap/Hold normal
        } else { // release
            // Release delete-hold
            if (bsp_del_active) {
                unregister_code(KC_DEL);
                send_keyboard_report();
                set_mods(bsp_del_saved_mods);
                send_keyboard_report();
                bsp_del_active = false;
                return false;
            }

			if (bsp_triple_pending && !bsp_repeat_active) {
				cancel_deferred_exec(bsp_triple_hold_token);
				bsp_triple_pending = false;
				bsp_tap_count = 0;
				tap_code(KC_BSPC);

				return false;
            }

            // Stop repeat if it was active
            if (bsp_repeat_active) {
                bsp_repeat_active = false;
                cancel_deferred_exec(bsp_repeat_token);
                bsp_tap_count = 0;
                return false;
            }
            return true; // normal LT behavior
        }
    } else {
        // Cancel repeat when other key is pressed
        if (record->event.pressed && bsp_repeat_active) {
            bsp_repeat_active = false;
            cancel_deferred_exec(bsp_repeat_token);
        }
    }

    // OS-aware App Switcher keys (toggle/step/last)
    if (keycode == APP_SW_TOG || keycode == APP_SW_TAB || keycode == APP_SW_PREV) {
        uint16_t mod = app_sw_mod_for_os();
        switch (keycode) {
            case APP_SW_TOG:
                if (record->event.pressed) {
                    if (!app_sw_toggled) {
                        register_code(mod);
                        app_sw_toggled = true;
                        app_sw_last_tab_time = timer_read();
                        if (app_sw_token) cancel_deferred_exec(app_sw_token);
                        app_sw_token = defer_exec(300, app_sw_autorelease_cb, NULL);
                    } else {
                        unregister_code(mod);
                        app_sw_toggled = false;
                        send_keyboard_report();
                        if (app_sw_token) cancel_deferred_exec(app_sw_token);
                    }
                }
                return false;
            case APP_SW_TAB:
                if (record->event.pressed) {
                    if (app_sw_toggled) {
                        tap_code(KC_TAB);
                        app_sw_last_tab_time = timer_read();
                    } else {
                        register_code(mod); tap_code(KC_TAB); unregister_code(mod);
                    }
                }
                return false;
            case APP_SW_PREV:
                if (record->event.pressed) {
                    same_app_window_cycle(true);
                }
                return false;
        }
    }

    // Hold-DEL command layer actions
    if (keycode == CMD_COPY || keycode == CMD_PASTE || keycode == CMD_INTR) {
        if (record->event.pressed) {
            switch (keycode) {
                case CMD_COPY:
                    cmd_copy_os_aware();
                    break;
                case CMD_PASTE:
                    cmd_paste_os_aware();
                    break;
                case CMD_INTR:
                    cmd_interrupt();
                    break;
            }
        }
        return false;
    }

    // Mouse modifier keys (MOUSE layer only)
    static bool m_dbl_mod_active = false;
    static bool m_tgl_mod_active = false;
    static bool m_drag_locked = false;
    if (keycode == M_DBL_MOD) {
        m_dbl_mod_active = record->event.pressed;
        return false;
    }
    if (keycode == M_TGL_MOD) {
        m_tgl_mod_active = record->event.pressed;
        return false;
    }

    // Middle click with hold threshold (>200ms)
    static bool mbtn3_pressed = false;
    static bool mbtn3_active = false;
    static deferred_token mbtn3_token = 0;
    uint32_t mbtn3_hold_cb(uint32_t t, void *arg) {
        (void)t; (void)arg;
        if (mbtn3_pressed && !mbtn3_active) {
            register_code(MS_BTN3);
            mbtn3_active = true;
        }
        return 0;
    }
    if (keycode == M_MBTN3) {
        if (record->event.pressed) {
            mbtn3_pressed = true;
            mbtn3_token = defer_exec(200, mbtn3_hold_cb, NULL);
        } else {
            mbtn3_pressed = false;
            cancel_deferred_exec(mbtn3_token);
            if (mbtn3_active) {
                unregister_code(MS_BTN3);
                send_keyboard_report();
                mbtn3_active = false;
            }
        }
        return false;
    }

    // BTN1 intercepts: double-click and drag-toggle
    if (keycode == MS_BTN1) {
        if (record->event.pressed) {
            if (m_dbl_mod_active) {
                tap_code(MS_BTN1);
                wait_ms(30);
                tap_code(MS_BTN1);
                return false;
            }
            if (m_tgl_mod_active) {
                if (!m_drag_locked) {
                    register_code(MS_BTN1);
                    m_drag_locked = true;
                } else {
                    unregister_code(MS_BTN1);
                    send_keyboard_report();
                    m_drag_locked = false;
                }
                return false;
            }
        } else {
            // If drag is locked, ignore BTN1 release
            if (m_drag_locked) return false;
        }
    }

    // Let MS_ACL0/1/2 fall through to QMK default handling (momentary accel)

    return true;
}

// Initialize - enable rgblight and show OS flash briefly, then restore layer color
void keyboard_post_init_user(void) {
#ifdef RGBLIGHT_LAYERS
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    // Attach our layer segments to QMK's rgblight layering system
    rgblight_layers = my_rgb_layers;
    apply_layer_color(layer_state); // set to current layer (Base off)
#endif
    // Configure OS-dependent behavior and (optionally) LED flash once OS is known
    defer_exec(200, os_setup_wait_cb, NULL);
}

// Matrix scan - keep empty (no LED work here)
void matrix_scan_user(void) {
    // Intentionally empty to avoid frequent LED updates
}
