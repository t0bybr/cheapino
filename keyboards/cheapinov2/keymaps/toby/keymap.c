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
#include "timer.h"
#include "wait.h"

// Guard window to avoid unintended BSPC quick-tap repeat after other keys
#ifndef BSP_QT_GUARD_MS
#define BSP_QT_GUARD_MS 180
#endif

// Time of last non-BSPC key press (for BSPC quick-tap guard)
static uint16_t bsp_qt_block_time = 0;
static uint16_t bsp_last_release_time = 0;
static uint16_t bsp_last_press_time = 0;
static bool     bsp_double_ready = false;
static uint16_t bsp_double_expire_time = 0;

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

// =========================
// MOUSE ACL stage toggling
// =========================
static int8_t mouse_acl_stage = -1; // -1 = none, 0/1/2 = held
static uint16_t acl_code_for(int8_t s) {
    return (s == 0) ? MS_ACL0 : (s == 1) ? MS_ACL1 : (s == 2) ? MS_ACL2 : KC_NO;
}
static void mouse_acl_set(int8_t s) {
    if (mouse_acl_stage == s) return;
    if (mouse_acl_stage >= 0) {
        unregister_code(acl_code_for(mouse_acl_stage));
        send_keyboard_report();
    }
    mouse_acl_stage = s;
    if (mouse_acl_stage >= 0) {
        register_code(acl_code_for(mouse_acl_stage));
        send_keyboard_report();
    }
}

// Force NUM layer while BSP_NUM held with second key
static bool bsp_num_down = false;
static bool num_forced_hold = false;

// Forward declaration for LED layer application
static void apply_layer_color(layer_state_t state);

// Track thumb LT state (for conditional HOOKP on SPC_NAV)
static bool bsp_num_pressed = false;

// Leader overlay state (white LED during leader timeout)
static bool leader_overlay_active = false;

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
    APP_SW_TOG,   // Toggle app-switch modifier on/off (Cmd/Super/Alt)
    APP_SW_TAB,   // Send Tab (uses toggle if active, otherwise quick mod+Tab)
    APP_SW_PREV,  // Quick previous app (mod+Shift+Tab)
    M_DBL_MOD,
    M_TGL_MOD,
    M_MBTN3,
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
// Repurpose DEL to Leader key (was: LT(_FKEY, KC_DEL))
#define DEL_FKY QK_LEAD

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
  APP_SW_TOG, APP_SW_TAB, APP_SW_PREV
    ),

    [_MOUSE] = LAYOUT_split_3x5_3(
  // Left: S/T modifiers placed on home row positions; Right: wheel/cursor mirrored to NAV
  KC_NO,   KC_NO,     M_TGL_MOD, M_DBL_MOD, KC_NO,
  KC_NO,   MS_WHLU,     MS_UP,   MS_WHLD,     KC_NO,
  KC_NO,   MS_ACL0,     MS_ACL1, MS_ACL2,     KC_NO,
           MS_WHLL,     MS_LEFT,   MS_DOWN,
  MS_RGHT,   MS_WHLR,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO, KC_NO,   KC_NO, KC_NO,   KC_NO,
  KC_NO,   KC_NO,     _______,
  M_MBTN3, MS_BTN1,   MS_BTN2
    ),

    [_SYM_R] = LAYOUT_split_3x5_3(
  S(KC_LBRC), S(KC_7), S(KC_8), S(KC_9), S(KC_RBRC),
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  S(KC_SCLN), S(KC_4), S(KC_5), S(KC_6), S(KC_EQL),
           KC_NO,   KC_RSFT, KC_RCTL,
  KC_RALT, KC_RGUI,
  S(KC_GRV), S(KC_1), S(KC_2), S(KC_3), S(KC_BSLS),
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  S(KC_9), S(KC_0), S(KC_MINS),    // Left middle thumb = ')'
  _______, BSP_NUM, DEL_FKY        // Ensure BSP_NUM accessible for tri-layer
    ),

    [_NUM] = LAYOUT_split_3x5_3(
  KC_LBRC, KC_7,    KC_8,    KC_9,    KC_RBRC,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_SCLN, KC_4,    KC_5,    KC_6,    KC_PEQL,
           KC_NO,   KC_RSFT, KC_RCTL,
  KC_RALT, KC_RGUI,
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_BSLS,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  QK_REP,  KC_0,    KC_PMNS,   // Move 0 to left thumb (middle)
  ENT_SYM, BSP_NUM, DEL_FKY    // Ensure ENT_SYM/BSP_NUM accessible for tri-layer
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
    // White at brightness 50 while leader is active
    rgblight_sethsv_noeeprom(0, 0, 50);
#endif
}

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

    // Turn off leader overlay and restore layer color
    leader_overlay_active = false;
    apply_layer_color(layer_state);
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
            if (timer_elapsed(bsp_qt_block_time) < BSP_QT_GUARD_MS) {
                return 0;
            }
            return 150;
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
        case _BASE:
        default:     return 0;   // Off (S ignored, V=0)
    }
}

static inline uint8_t hsv_s_for_layer(uint8_t layer) {
    switch (layer) {
        case _BASE:  return 0;   // Off when V=0
        default:     return 255; // Fully saturated
    }
}

static inline uint8_t hsv_v_for_layer(uint8_t layer) {
    // Global brightness = 50; Base layer off
    return (layer == _BASE) ? 0 : 50;
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
            // Activate overlay: random-ish hue, full sat, V=50
            uint8_t hue = (uint8_t)((timer_read() * 37u + (uint32_t)idx * 53u) & 0xFFu);
            hrm_overlay_active = true;
            rgblight_sethsv_noeeprom(hue, 255, 50);
        }
        hrm_active_count++;
    }
    return 0; // one-shot
}

// --- SHIFT+Backspace → Delete hold support (robust across HRM/Shift timing) ---
static bool hm_t_pressed = false;
static bool hm_n_pressed = false;
static bool bsp_del_active = false;
static uint8_t bsp_del_saved_mods = 0;

// --- BSPC Quick-Tap guard to avoid accidental repeat when typing a digit after BSPC ---

static void apply_layer_color(layer_state_t state) {
    if (leader_overlay_active) {
        // Leader overlay has highest priority: white at V=50 while waiting
        rgblight_sethsv_noeeprom(0, 0, 50);
        return;
    }
    if (hrm_overlay_active) {
        // Preserve HRM overlay color until HRM released
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
    // Show OS color for 800ms at brightness 50, then restore
    if (is_macos) {
        rgblight_sethsv_noeeprom(212, 255, 50); // macOS: magenta
    } else {
        rgblight_sethsv_noeeprom(85, 255, 50);  // Linux/Windows: green
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

    // MOUSE ACL default stage on enter, clear on exit
    if (layer_state_cmp(state, _MOUSE)) {
        mouse_acl_set(1); // default ACL1 when entering MOUSE
    } else {
        mouse_acl_set(-1); // clear when leaving
    }

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
    // Guard: any non-BSPC key press disables BSPC quick-tap repeat briefly
    if (record->event.pressed && keycode != BSP_NUM) {
        bsp_qt_block_time = timer_read();
    }
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
            // Left HRMs: A,R,S,T; Right HRMs: N,E,I,O
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
                    rgblight_sethsv_noeeprom(hue, 255, 50);
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
    }

    // Robust SHIFT+Backspace → Delete Hold
    // Hold KC_DEL while BSP_NUM is held and SHIFT is effectively down (as mod or HRM key pressed)
    // SHIFT + BSP_NUM → Delete (press=down, release=up), independent of LT
    if (keycode == BSP_NUM) {
        bool shift_effective = ((get_mods() & MOD_MASK_SHIFT) != 0) || hm_t_pressed || hm_n_pressed;
        if (shift_effective) {
            if (record->event.pressed) {
                // Save current mods and temporarily clear SHIFT to send plain DEL
                bsp_del_saved_mods = get_mods();
                if (bsp_del_saved_mods & MOD_MASK_SHIFT) {
                    unregister_mods(MOD_MASK_SHIFT);
                    send_keyboard_report();
                }
                register_code(KC_DEL);
                bsp_del_active = true;
                return false; // suppress LT processing
            } else {
                if (bsp_del_active) {
                    unregister_code(KC_DEL);
                    send_keyboard_report();
                    set_mods(bsp_del_saved_mods);
                    send_keyboard_report();
                    bsp_del_active = false;
                    return false;
                }
            }
        }
        // Track down state for NUM forced hold logic
        if (record->event.pressed) {
            // Double-press detection for BSPC intent (e.g., BSPC -> BSPC -> SPC_NAV for '0')
            if (timer_elapsed(bsp_last_press_time) < 120) {
                bsp_double_ready = true;
                bsp_double_expire_time = timer_read();
            } else {
                bsp_double_ready = false;
            }
            bsp_last_press_time = timer_read();
            bsp_num_down = true;
        } else {
            bsp_num_down = false;
            bsp_last_release_time = timer_read();
            if (num_forced_hold) { layer_off(_NUM); num_forced_hold = false; }
        }
    } else if (record->event.pressed) {
        // Any other key pressed while BSP_NUM held → force NUM layer
        if (bsp_num_down && !num_forced_hold) {
            layer_on(_NUM);
            num_forced_hold = true;
        }
        // If BSPC was double-pressed and SPC_NAV follows immediately, treat as '0' + latch NUM
        if (keycode == SPC_NAV && bsp_double_ready && timer_elapsed(bsp_double_expire_time) < 160) {
            if (!num_forced_hold) { layer_on(_NUM); num_forced_hold = true; }
            tap_code(KC_0);
            bsp_double_ready = false;
            return false;
        }
        // Any other key press cancels the BSPC double intent window
        if (keycode != SPC_NAV && keycode != BSP_NUM) {
            bsp_double_ready = false;
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
                    register_code(mod); register_code(KC_LSFT); tap_code(KC_TAB); unregister_code(KC_LSFT); unregister_code(mod);
                }
                return false;
        }
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

    // Toggle ACL stages with MS_ACL keys (select stage; no need to hold)
    if (keycode == MS_ACL0 || keycode == MS_ACL1 || keycode == MS_ACL2) {
        if (record->event.pressed && layer_state_cmp(layer_state, _MOUSE)) {
            int8_t stage = (keycode == MS_ACL0) ? 0 : (keycode == MS_ACL1) ? 1 : 2;
            mouse_acl_set(stage);
        }
        return false;
    }

    // Get current OS and mods
    os_variant_t os = detected_host_os();
    bool is_macos = (os == OS_MACOS || os == OS_IOS);
    uint8_t mods = get_mods();

    // Homerow mod LED removed - was causing keyboard lockup
    // LED calls in process_record_user() can block even with _noeeprom()

    // OS-aware modifier handling for Backspace (word delete)
    if (keycode == BSP_NUM) {
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
