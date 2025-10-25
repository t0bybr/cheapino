// Achordion: customize the tap-hold decision
// Copyright 2022-2023 Google LLC
// SPDX-License-Identifier: Apache-2.0
// https://getreuer.info/posts/keyboards/achordion

#include "achordion.h"

// Default timeout: 1000ms
#ifndef ACHORDION_TIMEOUT
#define ACHORDION_TIMEOUT 1000
#endif

static struct {
    uint16_t keycode;
    uint16_t timer;
    keyrecord_t record;
} achordion_state = {KC_NO, 0, {0}};

__attribute__((weak)) uint16_t achordion_timeout(uint16_t tap_hold_keycode) {
    return ACHORDION_TIMEOUT;
}

__attribute__((weak)) bool achordion_chord(uint16_t tap_hold_keycode,
                                          keyrecord_t* tap_hold_record,
                                          uint16_t other_keycode,
                                          keyrecord_t* other_record) {
    // Default: opposite hands
    return tap_hold_record->event.key.row % (MATRIX_ROWS / 2) !=
           other_record->event.key.row % (MATRIX_ROWS / 2);
}

__attribute__((weak)) bool achordion_eager_mod(uint16_t keycode) {
    return false;
}

void achordion_task(void) {
    if (achordion_state.keycode != KC_NO &&
        timer_elapsed(achordion_state.timer) >
        achordion_timeout(achordion_state.keycode)) {
        achordion_state.keycode = KC_NO;
    }
}

bool process_achordion(uint16_t keycode, keyrecord_t* record) {
    const uint16_t tap_hold_keycode = achordion_state.keycode;

    if (tap_hold_keycode == KC_NO) {
        // No tap-hold key is active
        if (record->event.pressed) {
            switch (keycode) {
                case QK_MOD_TAP ... QK_MOD_TAP_MAX:
                case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
                    // A new tap-hold key was pressed
                    achordion_state.keycode = keycode;
                    achordion_state.timer = timer_read();
                    achordion_state.record = *record;
                    break;
            }
        }
        return true;
    }

    // A tap-hold key is currently active
    if (keycode == tap_hold_keycode && !record->event.pressed) {
        // The active tap-hold key was released
        achordion_state.keycode = KC_NO;
        return true;
    }

    if (!record->event.pressed) {
        return true; // Key release, do nothing
    }

    // Another key was pressed while tap-hold is active
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            // Another tap-hold key was pressed
            if (achordion_chord(tap_hold_keycode, &achordion_state.record,
                              keycode, record)) {
                // Allow chord
                achordion_state.keycode = keycode;
                achordion_state.timer = timer_read();
                achordion_state.record = *record;
            } else {
                // Prevent hold for same-hand combinations
                achordion_state.keycode = KC_NO;
            }
            break;

        default:
            // Regular key was pressed
            if (!achordion_chord(tap_hold_keycode, &achordion_state.record,
                               keycode, record)) {
                // Same-hand key: prevent hold
                achordion_state.keycode = KC_NO;
            }
            // Check for eager mod
            else if (achordion_eager_mod(keycode)) {
                // Eager mod: immediately activate the hold
                achordion_state.keycode = KC_NO;
            }
            break;
    }

    return true;
}
