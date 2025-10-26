// Custom Cheapino Keymap
// Converted from Vial config with enhanced QMK features
// Features: Achordion, Repeat Key, Auto-repeat Backspace

#include QMK_KEYBOARD_H
#include "features/achordion.h"

// Layer definitions
enum layers {
    _BASE = 0,   // Colemak
    _MEDIA,      // Media controls
    _NAV,        // Navigation
    _MOUSE,      // Mouse keys
    _SYM_R,      // Symbols (right)
    _NUM,        // Numbers
    _FKEY,       // F-keys
    _EXTRA,      // Extra layer
};

// Custom keycodes (currently unused, reserved for future features)
enum custom_keycodes {
    CUSTOM_START = SAFE_RANGE,
};

// Home Row Mods (using standard mod-tap for better Achordion compatibility)
// Left hand: D=GUI, R=ALT, S=CTRL, T=SHIFT
#define HM_A LGUI_T(KC_A)
#define HM_R LALT_T(KC_R)
#define HM_S LCTL_T(KC_S)
#define HM_T LSFT_T(KC_T)

// Right hand: N=SHIFT, E=CTRL, I=ALT, O=GUI
#define HM_N RSFT_T(KC_N)
#define HM_E RCTL_T(KC_E)
#define HM_I RALT_T(KC_I)
#define HM_O RGUI_T(KC_O)

// Thumb keys with layer tap
// From left to right: Esc, Spc, Tab, Ent, Bspc, Del
// With layers:        Media, Nav, Mouse, Symbol, Num, Fkeys
#define ESC_MED LT(_MEDIA, KC_ESC)
#define SPC_NAV LT(_NAV, KC_SPC)
#define TAB_MOU LT(_MOUSE, KC_TAB)
#define ENT_SYM LT(_SYM_R, KC_ENT)
#define BSP_NUM LT(_NUM, KC_BSPC)
#define DEL_FKY LT(_FKEY, KC_DEL)

// Keymaps
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Base Layer: Colemak with Home Row Mods
     * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
     * │ Q │ W │ F │ P │ G │       │ J │ L │ U │ Y │ ' │
     * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
     * │ A │ R │ S │ T │ D │       │ H │ N │ E │ I │ O │
     * │Gui│Alt│Ctl│Sft│   │       │   │Sft│Ctl│Alt│Gui│
     * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
     * │ Z │ X │ C │ V │ B │       │ K │ M │ , │ . │ / │
     * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
     *       ┌─────┬─────┬─────┐ ┌─────┬─────┬─────┐
     *       │ Esc │ Spc │ Tab │ │ Ent │Bspc │ Del │
     *       │Media│ Nav │Mouse│ │ Sym │ Num │Fkey │
     *       └─────┴─────┴─────┘ └─────┴─────┴─────┘
     */
    [_BASE] = LAYOUT_split_3x5_3(
  // Left Hand - Colemak
  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,     // Reihe 1: Q W F P G
  HM_A,    HM_R,    HM_S,    HM_T,    KC_D,     // Reihe 2: A R S T D (mit Mods)
  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     // Reihe 3: Z X C V B
           ESC_MED, SPC_NAV, TAB_MOU,             // Daumen: Esc Spc Tab

  // Right Hand - Colemak
  KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT,  // Reihe 1: J L U Y '
  KC_H,    HM_N,    HM_E,    HM_I,    HM_O,     // Reihe 2: H N E I O (mit Mods)
  KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  // Reihe 3: K M , . /
  ENT_SYM, BSP_NUM, DEL_FKY                      // Daumen: Ent Bspc Del
    ),

    /*
     * Media Layer
     */
    [_MEDIA] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_LEFT, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           _______, KC_MPLY, KC_MUTE,

  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_MSTP, _______, _______
    ),

    /*
     * Navigation Layer
     */
    [_NAV] = LAYOUT_split_3x5_3(
  KC_NO,   KC_PGUP, KC_UP,   KC_PGDN, KC_NO,
  KC_HOME, KC_LEFT, KC_DOWN, KC_RGHT, KC_END,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_NO,   _______, _______,

  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  _______, _______, _______
    ),

    /*
     * Mouse Layer
     */
    [_MOUSE] = LAYOUT_split_3x5_3(
  KC_NO,   KC_WH_D, KC_MS_U, KC_WH_U, KC_BTN2,
  KC_WH_L, KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN1,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_BTN3,
           KC_NO,   _______, _______,

  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_ACL2, KC_ACL1, KC_ACL0, KC_NO,
  _______, _______, _______
    ),

    /*
     * Symbols Layer (Right)
     */
    [_SYM_R] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_NO,   _______, _______,

  S(KC_RBRC), S(KC_9), S(KC_8), S(KC_7), S(KC_LBRC),
  S(KC_EQL),  S(KC_6), S(KC_5), S(KC_4), S(KC_SCLN),
  S(KC_BSLS), S(KC_3), S(KC_2), S(KC_1), S(KC_GRV),
  _______, _______, S(KC_MINS)
    ),

    /*
     * Numbers Layer
     * Note: QK_REP = hold backspace, then tap again to auto-repeat
     */
    [_NUM] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_NO,   _______, QK_REP,

  KC_RBRC, KC_9,    KC_8,    KC_7,    KC_LBRC,
  KC_PEQL, KC_6,    KC_5,    KC_4,    KC_SCLN,
  KC_BSLS, KC_3,    KC_2,    KC_1,    KC_GRV,
  KC_PMNS, KC_0,    _______
    ),

    /*
     * Function Keys Layer
     * Note: QK_REP = hold delete, then tap again to auto-repeat
     */
    [_FKEY] = LAYOUT_split_3x5_3(
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
  KC_NO,   KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI,
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
           KC_NO,   _______, QK_REP,

  KC_PSCR, KC_F9,   KC_F8,   KC_F7,   KC_F12,
  KC_NO,   KC_F6,   KC_F5,   KC_F4,   KC_F11,
  KC_NO,   KC_F3,   KC_F2,   KC_F1,   KC_F10,
  KC_NO,   KC_APP,  _______
    ),

    /*
     * Extra Layer (for future expansion)
     */
    [_EXTRA] = LAYOUT_split_3x5_3(
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
         KC_NO, _______, _______,

  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
  _______, _______, _______
    ),
};

// Encoder configuration:
// Cheapino uses a custom encoder implementation via encoder.c at the keyboard level.
// The encoder is handled through the matrix and does not use QMK's standard ENCODER_ENABLE.
//
// The default behavior from cheapino/encoder.c is:
// - Click: Media Play/Pause
// - Rotate: Page Up/Down (or layer-specific actions)
//
// To customize encoder behavior per layer, you would need to modify
// keyboards/cheapino/encoder.c in the main QMK firmware.

// Per-key tapping term
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Home row mods - faster tapping term
        case HM_A:
        case HM_O:
            return TAPPING_TERM + 30; // GUI keys get slightly more time
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
        // Enable for thumb keys only
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

// Achordion configuration
bool achordion_chord(uint16_t tap_hold_keycode,
                    keyrecord_t* tap_hold_record,
                    uint16_t other_keycode,
                    keyrecord_t* other_record) {
    // Allow hold for opposite hands only
    // Cheapino v2 uses split 3x5+3 layout

    // Use row to determine which hand (split keyboard matrix)
    // Rows 0-3 = left hand, rows 4-7 = right hand
    uint8_t tap_hold_row = tap_hold_record->event.key.row;
    uint8_t other_row = other_record->event.key.row;

    bool tap_hold_is_left = tap_hold_row < 4;
    bool other_is_left = other_row < 4;

    // Allow chord only if keys are on opposite hands
    if (tap_hold_is_left != other_is_left) {
        return true;
    }

    // Exception: allow with thumb keys on same side
    // Thumb row is row 3 on left, row 7 on right (last row on each side)
    if (tap_hold_row == 3 || tap_hold_row == 7 ||
        other_row == 3 || other_row == 7) {
        return true;
    }

    return false;
}

// Achordion eager mods (for common shortcuts)
bool achordion_eager_mod(uint16_t keycode) {
    switch (keycode) {
        // Allow eager mods for common shortcuts
        case KC_C:
        case KC_V:
        case KC_X:
        case KC_Z:
        case KC_Y:
        case KC_F:
        case KC_S:
        case KC_TAB:
            return true;
        default:
            return false;
    }
}

// Process record user
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Process achordion first
    if (!process_achordion(keycode, record)) {
        return false;
    }

    // Add custom keycode handling here if needed in the future

    return true;
}

// Matrix scan user
void matrix_scan_user(void) {
    // Run achordion task for home row mods processing
    achordion_task();
}
