// Declarative Leader map for Cheapino (toby)
// Define sequences once; code is generated in leader_actions.c
// Syntax:
//   LEADER1(K1,               LINUX_ACTION,           MAC_ACTION, WIN_ACTION)  // comment
//   LEADER2(K1, K2,           LINUX_ACTION,           MAC_ACTION, WIN_ACTION)  // comment
//   LEADER3(K1, K2, K3,       LINUX_ACTION,           MAC_ACTION, WIN_ACTION)  // comment
// Use tap_code16-compatible actions (e.g., KC_*, LCTL(KC_X), LGUI(KC_SPC), MAC_WIN_MAXIMIZE, ...)

#pragma once

// One-key sequences (optional)
#define LEADER_MAP_1KEY \
/* Example: Leader + Q to send Escape on both OS */ \
/* LEADER1(KC_Q, KC_ESC, KC_ESC) */

// Two-key sequences
#define LEADER_MAP_2KEY \
/* App launcher: Super on Linux/Win, Spotlight on macOS */ \
LEADER2(KC_S, KC_P,              KC_LGUI,               LGUI(KC_SPC),          KC_LGUI) \
/* Terminal interrupt (Ctrl+C) */ \
LEADER2(KC_T, KC_C,              LCTL(KC_C),            LCTL(KC_C),            LCTL(KC_C)) \
/* Window management */ \
LEADER2(KC_W, KC_C,              LALT(KC_F4),           LGUI(KC_W),            LALT(KC_F4)) \
LEADER2(KC_W, KC_M,              LGUI(KC_H),            LGUI(KC_M),            LGUI(KC_DOWN)) \
LEADER2(KC_W, KC_F,              KC_F11,                 LCTL(LGUI(KC_F)),     KC_F11) \
LEADER2(KC_W, KC_X,              LGUI(KC_UP),           LCTL(LALT(KC_ENT)),    LGUI(KC_UP)) \
LEADER2(KC_W, KC_R,              LGUI(KC_DOWN),         LCTL(LALT(KC_ENT)),    LGUI(KC_DOWN)) \
LEADER2(KC_W, KC_H,              LGUI(KC_LEFT),         LCTL(LALT(KC_LEFT)),   LGUI(KC_LEFT)) \
LEADER2(KC_W, KC_L,              LGUI(KC_RGHT),         LCTL(LALT(KC_RGHT)),   LGUI(KC_RGHT)) \
LEADER2(KC_W, KC_N,              LGUI(KC_PGDN),         LCTL(KC_RGHT),         LCTL(LGUI(KC_RGHT))) \
LEADER2(KC_W, KC_P,              LGUI(KC_PGUP),         LCTL(KC_LEFT),         LCTL(LGUI(KC_LEFT))) \
LEADER2(KC_W, KC_K,              LSFT(LGUI(KC_PGDN)),   LSFT(LCTL(KC_RGHT)),   LSFT(LGUI(KC_RGHT))) \
LEADER2(KC_W, KC_J,              LSFT(LGUI(KC_PGUP)),   LSFT(LCTL(KC_LEFT)),   LSFT(LGUI(KC_LEFT))) \
LEADER2(KC_W, KC_D,              LSFT(LGUI(KC_RGHT)),   LSFT(LCTL(LALT(KC_RGHT))), LSFT(LGUI(KC_RGHT))) \
LEADER2(KC_W, KC_A,              LSFT(LGUI(KC_LEFT)),   LSFT(LCTL(LALT(KC_LEFT))),  LSFT(LGUI(KC_LEFT)))

// Three-key sequences (optional)
#define LEADER_MAP_3KEY \
/* Example: Leader + G + I + T to send 'git status' (add if wanted) */ \
/* LEADER3(KC_G, KC_I, KC_T,     SEND_STRING("git status\n"), SEND_STRING("git status\n"), SEND_STRING("git status\n")) */
