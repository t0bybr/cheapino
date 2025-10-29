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
#define LEADER_TIMEOUT 800  // Time to complete leader sequence (ms) - increased for more time
#define LEADER_PER_KEY_TIMING  // Each key in sequence has own timeout

// OS Detection - Debug mode removed (caused EECONFIG_SIZE error on RP2040)
// #define OS_DETECTION_DEBUG_ENABLE  // Disabled - doesn't work on RP2040

// Mouse Keys - Kinetic (Inertia) Mode
#undef MK_COMBINED
#define MK_KINETIC_SPEED
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MOVE_DELTA 5         // Finer base motion
#define MOUSEKEY_MAX_SPEED 28         // Comfortable top speed
#define MOUSEKEY_TIME_TO_MAX 60       // Time to accelerate to max
#define MOUSEKEY_FRICTION 8           // Inertia friction (higher = more damping)
// Wheel: make it slow enough to read while holding
#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 100   // Much slower scroll rate
#define MOUSEKEY_WHEEL_MAX_SPEED 6    // Lower peak wheel speed
#define MOUSEKEY_WHEEL_TIME_TO_MAX 100

// Mouse speed multipliers (for ACL0/ACL1/ACL2 keys)
// Note: ACL stages do not affect kinetic mode. Left here for reference only.
// #define MK_C_OFFSET_0 16
// #define MK_C_INTERVAL_0 12
#define MK_C_OFFSET_1 4    // Medium (ACL1)
#define MK_C_INTERVAL_1 16
#define MK_C_OFFSET_2 16   // Fast (ACL2)
#define MK_C_INTERVAL_2 8

// LED brightness
#define LED_BRIGHTNESS 50
#define LED_BRIGHTNESS_HOMEROW 50

// Caps Word - uses both shifts (defined in keyboard config.h)
// BOTH_SHIFTS_TURNS_ON_CAPS_WORD is already active from keyboard/cheapinov2/config.h

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
