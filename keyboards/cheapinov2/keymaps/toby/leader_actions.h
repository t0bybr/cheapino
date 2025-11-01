// Leader actions for Cheapino keymap (toby)
// Splits Leader combinations out of keymap.c for clarity.

#pragma once
#include QMK_KEYBOARD_H

// Handle all Leader sequences. Must be called from leader_end_user().
// Pass true for macOS (Cmd/Rectangle), false for Linux (GNOME).
void leader_handle_sequences(bool is_macos);

