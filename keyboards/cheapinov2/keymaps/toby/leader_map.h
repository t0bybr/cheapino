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
/* Double DEL: trigger app-level leader key (tmux/nvim/emacs) */ \
LEADER1(DEL_CMD, APP_LEADER_KEY, APP_LEADER_KEY, APP_LEADER_KEY)

// Two-key sequences
#define LEADER_MAP_2KEY \
/* Launcher */ \
LEADER2(KC_S, KC_P,              LCTL(KC_SPC),          LGUI(KC_SPC),          KC_LGUI) \
/* Fenster: schließen, minimieren, max/Fullscreen, links/rechts, Workspaces, Move Window */ \
LEADER2(KC_W, KC_C,              LALT(KC_F4),           LGUI(KC_W),            LALT(KC_F4)) \
LEADER2(KC_W, KC_M,              LGUI(KC_H),            LGUI(KC_M),            LGUI(KC_DOWN)) \
LEADER2(KC_W, KC_F,              LGUI(KC_UP),           LCTL(LGUI(KC_F)),      LGUI(KC_UP)) \
LEADER2(KC_W, KC_H,              LGUI(KC_LEFT),         LCTL(LALT(KC_LEFT)),   LGUI(KC_LEFT)) \
LEADER2(KC_W, KC_L,              LGUI(KC_RGHT),         LCTL(LALT(KC_RGHT)),   LGUI(KC_RGHT)) \
LEADER2(KC_W, KC_P,              LGUI(KC_PGUP),         LCTL(KC_LEFT),         LCTL(LGUI(KC_LEFT))) \
LEADER2(KC_W, KC_N,              LGUI(KC_PGDN),         LCTL(KC_RGHT),         LCTL(LGUI(KC_RGHT))) \
LEADER2(KC_W, KC_J,              LSFT(LGUI(KC_PGUP)),   LSFT(LCTL(KC_LEFT)),   LSFT(LGUI(KC_LEFT))) \
LEADER2(KC_W, KC_K,              LSFT(LGUI(KC_PGDN)),   LSFT(LCTL(KC_RGHT)),   LSFT(LGUI(KC_RGHT))) \
/* Browser: Tabs, Reload, URL-Bar, Back/Forward */ \
LEADER2(KC_B, KC_T,              LCTL(KC_T),            LGUI(KC_T),            LCTL(KC_T)) \
LEADER2(KC_B, KC_W,              LCTL(KC_W),            LGUI(KC_W),            LCTL(KC_W)) \
LEADER2(KC_B, KC_R,              LCTL(KC_R),            LGUI(KC_R),            LCTL(KC_R)) \
LEADER2(KC_B, KC_A,              LCTL(KC_L),            LGUI(KC_L),            LCTL(KC_L)) \
LEADER2(KC_B, KC_P,              LCTL(LSFT(KC_TAB)),    LGUI(LSFT(KC_LBRC)),   LCTL(LSFT(KC_TAB))) \
LEADER2(KC_B, KC_N,              LCTL(KC_TAB),          LGUI(LSFT(KC_RBRC)),   LCTL(KC_TAB)) \
LEADER2(KC_B, KC_B,              LALT(KC_LEFT),         LGUI(KC_LBRC),         LALT(KC_LEFT)) \
LEADER2(KC_B, KC_F,              LALT(KC_RGHT),         LGUI(KC_RBRC),         LALT(KC_RGHT)) \
/* Terminal: Interrupt, neue Tabs/Windows, Tab-Navigation */ \
LEADER2(KC_T, KC_C,              LCTL(KC_C),            LCTL(KC_C),            LCTL(KC_C)) \
LEADER2(KC_T, KC_T,              LCTL(LSFT(KC_T)),      LGUI(KC_T),            LCTL(LSFT(KC_T))) \
LEADER2(KC_T, KC_N,              LCTL(LSFT(KC_N)),      LGUI(KC_N),            LCTL(LSFT(KC_N))) \
LEADER2(KC_T, KC_W,              LCTL(LSFT(KC_W)),      LGUI(KC_W),            LCTL(LSFT(KC_W))) \
LEADER2(KC_T, KC_H,              LCTL(KC_PGUP),         LGUI(LSFT(KC_LBRC)),   LCTL(KC_PGUP)) \
LEADER2(KC_T, KC_L,              LCTL(KC_PGDN),         LGUI(LSFT(KC_RBRC)),   LCTL(KC_PGDN)) \
/* Dateien: Explorer/Finder, Tabs, Up/Back */ \
LEADER2(KC_F, KC_O,              LGUI(KC_E),            LGUI(LALT(KC_SPC)),    LGUI(KC_E)) \
LEADER2(KC_F, KC_T,              LCTL(KC_T),            LGUI(KC_T),            LCTL(KC_T)) \
LEADER2(KC_F, KC_W,              LCTL(KC_W),            LGUI(KC_W),            LCTL(KC_W)) \
LEADER2(KC_F, KC_U,              LALT(KC_UP),           LGUI(KC_UP),           LALT(KC_UP)) \
LEADER2(KC_F, KC_B,              LALT(KC_LEFT),         LGUI(KC_LBRC),         LALT(KC_LEFT))

// Three-key sequences (optional)
#define LEADER_MAP_3KEY \
/* Example: Leader + G + I + T to send 'git status' (add if wanted) */ \
/* LEADER3(KC_G, KC_I, KC_T,     SEND_STRING("git status\n"), SEND_STRING("git status\n"), SEND_STRING("git status\n")) */
