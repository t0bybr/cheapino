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
  _______, KC_NO,   KC_NO
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

#ifdef RGBLIGHT_LAYERS
// Wait until OS detection becomes known, then show OS color briefly
static uint32_t os_flash_wait_cb(uint32_t trigger_time, void *cb_arg) {
    (void)trigger_time;
    static uint8_t attempts = 0;
    attempts++;

    os_variant_t os = detected_host_os();
    if (os == OS_UNSURE) {
        if (attempts < 30) { // wait up to ~6s (200ms * 30)
            return 200; // try again
        } else {
            // give up, apply current layer color
            apply_layer_color(layer_state);
            return 0;
        }
    }

    // Show OS color at brightness 50 for 800ms
    if (os == OS_MACOS || os == OS_IOS) {
        rgblight_sethsv_noeeprom(212, 255, 50); // macOS: magenta
    } else if (os == OS_LINUX || os == OS_WINDOWS) {
        rgblight_sethsv_noeeprom(85, 255, 50);  // Linux/Windows: green
    } else {
        rgblight_sethsv_noeeprom(0, 0, 50);     // fallback: white
    }
    defer_exec(800, os_flash_done_cb, NULL);
    return 0;
}
#endif // RGBLIGHT_LAYERS

// ============================================================================
// TRI-LAYER: SYM_R + NUM = FKEY + LED updates on state change
// ============================================================================

layer_state_t layer_state_set_user(layer_state_t state) {
    // Tri-Layer: When SYM_R and NUM are both active, activate FKEY
    state = update_tri_layer_state(state, _SYM_R, _NUM, _FKEY);

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

// Initialize - enable rgblight and show OS flash briefly, then restore layer color
void keyboard_post_init_user(void) {
#ifdef RGBLIGHT_LAYERS
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    // Attach our layer segments to QMK's rgblight layering system
    rgblight_layers = my_rgb_layers;
    // Defer OS flash until OS is known; otherwise wait and try again
    apply_layer_color(layer_state); // set to current layer (Base off)
    defer_exec(200, os_flash_wait_cb, NULL);
#endif
}

// Matrix scan - keep empty (no LED work here)
void matrix_scan_user(void) {
    // Intentionally empty to avoid frequent LED updates
}
