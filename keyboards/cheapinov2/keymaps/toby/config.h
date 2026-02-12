// Copyright 2024 Toby
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Chordal Hold Configuration
#define CHORDAL_HOLD
// Recommended: Use with Permissive Hold for best results
#define PERMISSIVE_HOLD_PER_KEY

// Tapping configuration for dual-function keys
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM_PER_KEY  // Enable per-key quick tap for auto-repeat

// Hold on other key press - allow faster layer access
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// Combos
#define COMBO_COUNT 3  // Leader combo removed (DEL is Leader directly)
#define COMBO_TERM 100  // Time window for combo detection (ms) - increased for easier triggering

// Leader Key
#define LEADER_TIMEOUT 1000  // More time to start; sequences auto-complete on 2nd key
#define LEADER_PER_KEY_TIMING  // Each key in sequence has own timeout

// OS Detection - Debug mode removed (caused EECONFIG_SIZE error on RP2040)
// #define OS_DETECTION_DEBUG_ENABLE  // Disabled - doesn't work on RP2040

// Mouse Keys - Kinetic (Inertia) Mode
#undef MK_COMBINED
#define MK_KINETIC_SPEED
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MOVE_DELTA 10         // Slightly higher base motion for snappier start
#define MOUSEKEY_MAX_SPEED 24         // Slightly lower top speed
#define MOUSEKEY_TIME_TO_MAX 4        // Even faster ramp to max speed
#define MOUSEKEY_FRICTION 4           // Inertia friction (higher = more damping)
// Wheel: make it slow enough to read while holding
#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 80   // Much slower scroll rate
#define MOUSEKEY_WHEEL_MAX_SPEED 5    // Lower peak wheel speed
#define MOUSEKEY_WHEEL_TIME_TO_MAX 120

// Mouse speed multipliers (for ACL0/ACL1/ACL2 keys)
// Note: ACL stages do not affect kinetic mode. Left here for reference only.
#define MK_C_OFFSET_0 16
#define MK_C_INTERVAL_0 12
#define MK_C_OFFSET_1 4    // Medium (ACL1)
#define MK_C_INTERVAL_1 16
#define MK_C_OFFSET_2 16   // Fast (ACL2)
#define MK_C_INTERVAL_2 8

// LED brightness
#define LED_BRIGHTNESS 50
#define LED_BRIGHTNESS_HOMEROW 50

// Unicode: Linux IBus method (Ctrl+Shift+U)
#define UNICODE_SELECTED_MODES UNICODE_MODE_LINUX

// Caps Word - enable via both shifts at keymap level
#ifndef BOTH_SHIFTS_TURNS_ON_CAPS_WORD
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD
#endif

// Repeat Key is built-in, no config needed

// RGB lighting: enable rgblight layers for non-blocking overlays
#define RGBLIGHT_LAYERS

// App switcher configuration (Linux/Windows)
#ifndef LINUX_APP_SWITCH_MOD
#define LINUX_APP_SWITCH_MOD KC_LGUI  // Change to KC_LALT if your WM uses Alt+Tab
#endif

// App switcher auto-release timeout (ms) after last Tab when toggled
#ifndef APP_SW_AUTORELEASE_MS
#define APP_SW_AUTORELEASE_MS 1500
#endif

// App-level leader trigger sent by DEL,DEL (double-tap DEL_FKY)
// Use a robust combo instead of F13-F24 (often intercepted/ignored by host).
// Map this combo in tmux/nvim/emacs to keep one consistent "app leader".
#ifndef APP_LEADER_KEY
#define APP_LEADER_KEY C(A(KC_F12))
#endif

// BSPC timing is managed in keymap (triple-tap window)
// Leader shortcuts are declared in leader_map.h (single source of truth)
