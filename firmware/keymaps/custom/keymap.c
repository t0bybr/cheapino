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
// Left hand: A=GUI, R=RALT(AltGr für Umlaute), S=CTRL, T=SHIFT
#define HM_A LGUI_T(KC_A)
#define HM_R RALT_T(KC_R)  // AltGr für deutsche Umlaute (ä, ö, ü)
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
     * KORREKTE Matrix-Zuordnung basierend auf Test-Ergebnissen
     *
     * Physisch:
     * Links:  Q W F P G  |  A R S T D  |  Z X C V B  |  Esc Spc Tab
     * Rechts: J L U Y '  |  H N E I O  |  K M , . /  |  Ent Bspc Del
     */
    [_BASE] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1
  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,

  // Position 5-9: Rechts Reihe 1
  KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT,

  // Position 10-14: Links Reihe 2 (mit Home Row Mods)
  HM_A,    HM_R,    HM_S,    HM_T,    KC_D,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3 (mit Home Row Mods)
           KC_H,    HM_N,    HM_E,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5 (mit Home Row Mods)
  HM_I,    HM_O,

  // Position 20-24: Links Reihe 3
  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,

  // Position 25-29: Rechts Reihe 3
  KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,

  // Position 30-32: Links Daumen
  ESC_MED, SPC_NAV, TAB_MOU,

  // Position 33-35: Rechts Daumen
  ENT_SYM, BSP_NUM, DEL_FKY
    ),

    /*
     * Media Layer
     * Vial Layer 1 korrekt gemappt
     */
    [_MEDIA] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 10-14: Links Reihe 2
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_LEFT, KC_MPRV, KC_VOLD,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_VOLU, KC_MNXT,

  // Position 20-24: Links Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen (ESC=TRNS weil Layer-Aktivierung, SPC=NO, TAB=NO)
  _______, KC_NO,   KC_NO,

  // Position 33-35: Rechts Daumen
  KC_MSTP, KC_MPLY, KC_MUTE
    ),

    /*
     * Navigation Layer
     * Vial Layer 2 korrekt gemappt
     */
    [_NAV] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_PGUP, KC_UP,   KC_PGDN, KC_NO,

  // Position 10-14: Links Reihe 2 (Modifier auf links, reversed)
  KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, KC_NO,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_HOME, KC_LEFT, KC_DOWN,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_RGHT, KC_END,

  // Position 20-24: Links Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen
  KC_NO,   _______, KC_NO,

  // Position 33-35: Rechts Daumen
  KC_NO,   KC_NO,   KC_NO
    ),

    /*
     * Mouse Layer
     * Vial Layer 3 korrekt gemappt
     */
    [_MOUSE] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_WH_D, KC_MS_U, KC_WH_U, KC_NO,

  // Position 10-14: Links Reihe 2
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_WH_L, KC_MS_L, KC_MS_D,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_MS_R, KC_WH_R,

  // Position 20-24: Links Reihe 3 (Acceleration reversed)
  KC_NO,   KC_ACL0, KC_ACL1, KC_ACL2, KC_NO,

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen
  KC_NO,   KC_NO,   _______,

  // Position 33-35: Rechts Daumen
  KC_BTN2, KC_BTN1, KC_BTN3
    ),

    /*
     * Symbols Layer (Right)
     * Vial Layer 4 korrekt gemappt
     */
    [_SYM_R] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1 (reversed)
  S(KC_LBRC), S(KC_7), S(KC_8), S(KC_9), S(KC_RBRC),

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 10-14: Links Reihe 2 (reversed)
  S(KC_SCLN), S(KC_4), S(KC_5), S(KC_6), S(KC_EQL),

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_NO,   KC_RSFT, KC_RCTL,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_RALT, KC_RGUI,

  // Position 20-24: Links Reihe 3 (reversed)
  S(KC_GRV), S(KC_1), S(KC_2), S(KC_3), S(KC_BSLS),

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen
  S(KC_9), S(KC_0), S(KC_MINS),

  // Position 33-35: Rechts Daumen
  _______, KC_NO,   KC_NO
    ),

    /*
     * Numbers Layer
     * Vial Layer 5 korrekt gemappt
     * Note: QK_REP = hold backspace, then tap again to auto-repeat
     */
    [_NUM] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1 (reversed)
  KC_LBRC, KC_7,    KC_8,    KC_9,    KC_RBRC,

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 10-14: Links Reihe 2 (reversed)
  KC_SCLN, KC_4,    KC_5,    KC_6,    KC_PEQL,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_NO,   KC_RSFT, KC_RCTL,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_RALT, KC_RGUI,

  // Position 20-24: Links Reihe 3 (reversed)
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_BSLS,

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen (QK_REP für Autorepeat-Funktion)
  QK_REP,  KC_0,    KC_PMNS,

  // Position 33-35: Rechts Daumen
  KC_NO,   _______, KC_NO
    ),

    /*
     * Function Keys Layer
     * Vial Layer 6 korrekt gemappt
     * Note: QK_REP = hold delete, then tap again to auto-repeat
     * Note: QK_BOOT (rechts oben) = Enter bootloader for flashing
     */
    [_FKEY] = LAYOUT_split_3x5_3(
  // Position 0-4: Links Reihe 1 (reversed)
  KC_F12,  KC_F7,   KC_F8,   KC_F9,   KC_PSCR,

  // Position 5-9: Rechts Reihe 1
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   QK_BOOT,

  // Position 10-14: Links Reihe 2 (reversed)
  KC_F11,  KC_F4,   KC_F5,   KC_F6,   KC_NO,

  // Position 15-17: Rechts Reihe 2, Tasten 1-3
           KC_NO,   KC_RSFT, KC_RCTL,

  // Position 18-19: Rechts Reihe 2, Tasten 4-5
  KC_RALT, KC_RGUI,

  // Position 20-24: Links Reihe 3 (reversed)
  KC_F10,  KC_F1,   KC_F2,   KC_F3,   KC_NO,

  // Position 25-29: Rechts Reihe 3
  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

  // Position 30-32: Links Daumen (QK_REP für Autorepeat-Funktion)
  KC_APP,  KC_NO,   QK_REP,

  // Position 33-35: Rechts Daumen
  KC_NO,   KC_NO,   _______
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

// LED brightness (0-255, lower = dimmer)
#define LED_BRIGHTNESS 50  // Reduced from default 255
#define LED_BRIGHTNESS_HOMEROW 50  // Same brightness for homerow mod indicator

// Track homerow mod state for LED feedback
static bool homerow_mod_active = false;

// LED Layer Indicator with reduced brightness
// Set LED color based on active layer
layer_state_t layer_state_set_user(layer_state_t state) {
    // Don't change LED if homerow mod is active (white LED)
    if (homerow_mod_active) {
        return state;
    }

    switch (get_highest_layer(state)) {
        case _BASE:
            rgblight_sethsv_noeeprom(0, 0, 0);              // Off - Base layer
            break;
        case _MEDIA:
            rgblight_sethsv_noeeprom(128, 255, LED_BRIGHTNESS);  // Cyan - Media
            break;
        case _NAV:
            rgblight_sethsv_noeeprom(170, 255, LED_BRIGHTNESS);  // Blue - Navigation
            break;
        case _MOUSE:
            rgblight_sethsv_noeeprom(85, 255, LED_BRIGHTNESS);   // Green - Mouse
            break;
        case _SYM_R:
            rgblight_sethsv_noeeprom(213, 255, LED_BRIGHTNESS);  // Magenta - Symbols
            break;
        case _NUM:
            rgblight_sethsv_noeeprom(28, 255, LED_BRIGHTNESS);   // Orange - Numbers
            break;
        case _FKEY:
            rgblight_sethsv_noeeprom(191, 255, LED_BRIGHTNESS);  // Purple - F-Keys
            break;
        default:
            rgblight_sethsv_noeeprom(0, 255, LED_BRIGHTNESS);    // Red - Unknown layer
            break;
    }
    return state;
}

// Process record user
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Process achordion first
    if (!process_achordion(keycode, record)) {
        return false;
    }

    // Track homerow mod activation for LED feedback
    bool is_homerow_mod = false;
    switch (keycode) {
        case HM_A:
        case HM_R:
        case HM_S:
        case HM_T:
        case HM_N:
        case HM_E:
        case HM_I:
        case HM_O:
            is_homerow_mod = true;
            break;
    }

    if (is_homerow_mod) {
        if (record->event.pressed) {
            // Key pressed - wait for it to be held
        } else {
            // Key released - turn off white LED
            if (homerow_mod_active) {
                homerow_mod_active = false;
                // Restore layer color
                layer_state_set_user(layer_state);
            }
        }
    }

    return true;
}

// Check if homerow mod is being held (called periodically)
void matrix_scan_user(void) {
    // Run achordion task for home row mods processing
    achordion_task();

    // Check if any homerow mod is being held as modifier
    static uint16_t hold_timer = 0;
    bool mod_is_held = (get_mods() & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)) != 0;

    if (mod_is_held && !homerow_mod_active) {
        if (hold_timer == 0) {
            hold_timer = timer_read();
        } else if (timer_elapsed(hold_timer) > TAPPING_TERM) {
            // Mod has been held long enough - activate white LED
            homerow_mod_active = true;
            rgblight_sethsv_noeeprom(0, 0, LED_BRIGHTNESS_HOMEROW);  // White LED
        }
    } else if (!mod_is_held) {
        hold_timer = 0;
        if (homerow_mod_active) {
            homerow_mod_active = false;
            // Restore layer color
            layer_state_set_user(layer_state);
        }
    }
}
