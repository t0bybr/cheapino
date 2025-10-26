// Custom Cheapino Keymap Configuration
// Enhanced Home Row Mods with latest QMK features

#pragma once

// Tapping configuration for Home Row Mods
// Override the default TAPPING_TERM from cheapino/config.h (230ms)
#undef TAPPING_TERM
#define TAPPING_TERM 200                    // Faster tapping term for home row mods
#define TAPPING_TERM_PER_KEY                // Allow per-key tapping term
#define PERMISSIVE_HOLD_PER_KEY             // Allow per-key permissive hold
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY     // Allow per-key hold on other key press

// Quick Tap Term - prevents accidental holds when typing fast
#define QUICK_TAP_TERM 120

// Achordion settings (requires achordion.c)
#define ACHORDION_STREAK                    // Enable streak detection

// Performance optimizations
#define FORCE_NKRO                          // N-Key Rollover
#define USB_POLLING_INTERVAL_MS 1           // 1000Hz polling rate

// Debounce
#define DEBOUNCE 5

// Bootloader Access:
// Hold BOOTSEL button while plugging in USB to enter bootloader mode
// (Double-tap reset requires a physical reset button, which this board doesn't have)

// RGB/LED (if applicable)
// #define RGB_MATRIX_ENABLE

// Mouse key speed and acceleration
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MOVE_DELTA 8
#define MOUSEKEY_MAX_SPEED 7
#define MOUSEKEY_TIME_TO_MAX 40
#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 50
#define MOUSEKEY_WHEEL_MAX_SPEED 8
#define MOUSEKEY_WHEEL_TIME_TO_MAX 40
