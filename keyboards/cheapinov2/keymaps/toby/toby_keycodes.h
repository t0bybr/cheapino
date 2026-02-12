// Shared custom keycodes for toby keymap modules.
#pragma once

#include QMK_KEYBOARD_H

enum custom_keycodes {
    CUSTOM_START = SAFE_RANGE,
    DEL_CMD,      // DEL_FKY hybrid handler (tap/hold/double-tap)
    APP_SW_TOG,   // Toggle app-switch modifier on/off (Cmd/Super/Alt)
    APP_SW_TAB,   // Send Tab (uses toggle if active, otherwise quick mod+Tab)
    APP_SW_PREV,  // Same-app previous window (OS-aware)
    CMD_COPY,     // OS-aware copy (terminal-first)
    CMD_PASTE,    // OS-aware paste (terminal-first)
    CMD_INTR,     // Terminal interrupt (Ctrl+C)
    M_DBL_MOD,
    M_TGL_MOD,
    M_MBTN3,
};
