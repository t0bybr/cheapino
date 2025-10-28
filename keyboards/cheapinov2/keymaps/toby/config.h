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
#define COMBO_COUNT 4  // Number of combos we define (removed Caps Word combos - using both shifts instead)
#define COMBO_TERM 100  // Time window for combo detection (ms) - increased for easier triggering

// Leader Key
#define LEADER_TIMEOUT 800  // Time to complete leader sequence (ms) - increased for more time
#define LEADER_PER_KEY_TIMING  // Each key in sequence has own timeout

// OS Detection - Debug mode removed (caused EECONFIG_SIZE error on RP2040)
// #define OS_DETECTION_DEBUG_ENABLE  // Disabled - doesn't work on RP2040

// Mouse Keys - Combined Mode (Constant + Acceleration)
#define MK_COMBINED  // Combined mode: constant speed + acceleration
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MOVE_DELTA 5         // Initial constant speed (pixels per update)
#define MOUSEKEY_MAX_SPEED 20         // Maximum speed with acceleration
#define MOUSEKEY_TIME_TO_MAX 60       // Time to reach max speed (updates * interval)
#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 50
#define MOUSEKEY_WHEEL_MAX_SPEED 8
#define MOUSEKEY_WHEEL_TIME_TO_MAX 40

// Mouse speed multipliers (for ACL0/ACL1/ACL2 keys)
#define MK_C_OFFSET_0 1    // Slow (ACL0)
#define MK_C_INTERVAL_0 32
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
