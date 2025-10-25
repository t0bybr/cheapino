// Achordion: customize the tap-hold decision
// Copyright 2022-2023 Google LLC
// SPDX-License-Identifier: Apache-2.0
// https://getreuer.info/posts/keyboards/achordion

#pragma once

#include QMK_KEYBOARD_H

// Call this function from `process_record_user()`.
bool process_achordion(uint16_t keycode, keyrecord_t* record);

// (Optional) Call this function from `matrix_scan_user()`.
void achordion_task(void);

// (Optional) Customize the chord timeout.
// Default timeout is 1000ms.
uint16_t achordion_timeout(uint16_t tap_hold_keycode);

// (Optional) Customize which keys are considered "chords".
// Return true if hold should be ignored for this key combination.
bool achordion_chord(uint16_t tap_hold_keycode,
                    keyrecord_t* tap_hold_record,
                    uint16_t other_keycode,
                    keyrecord_t* other_record);

// (Optional) Disable eager mods for these keys.
bool achordion_eager_mod(uint16_t keycode);
