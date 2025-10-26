// TEST KEYMAP - Jede Taste hat einen eindeutigen Buchstaben/Zahl
// Damit kannst du herausfinden, welche physische Taste wo im Layout ist

#include QMK_KEYBOARD_H

enum layers {
    _TEST = 0,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * TEST Layout
     * Jede Position hat einen eindeutigen Code
     *
     * Links sollte sein:          Rechts sollte sein:
     * 1  2  3  4  5               6  7  8  9  0
     * A  B  C  D  E               F  G  H  I  J
     * Q  W  E  R  T               Y  U  I  O  P
     *    L1 L2 L3                 R1 R2 R3
     *
     * L = Left Thumb, R = Right Thumb
     */
    [_TEST] = LAYOUT_split_3x5_3(
  // Left Hand - sollte 1,2,3,4,5 / A,B,C,D,E / Q,W,E,R,T sein
  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,
  KC_A,    KC_B,    KC_C,    KC_D,    KC_E,
  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
           KC_F1,   KC_F2,   KC_F3,   // L1=F1, L2=F2, L3=F3

  // Right Hand - sollte 6,7,8,9,0 / F,G,H,I,J / Y,U,I,O,P sein
  KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
  KC_F,    KC_G,    KC_H,    KC_I,    KC_J,
  KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
  KC_F4,   KC_F5,   KC_F6    // R1=F4, R2=F5, R3=F6
    ),
};
