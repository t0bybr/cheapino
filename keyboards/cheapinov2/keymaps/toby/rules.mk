# Copyright 2024 Toby
# SPDX-License-Identifier: GPL-2.0-or-later

# Core QMK Features
CAPS_WORD_ENABLE = yes         # Caps Word (both shifts or combo)
REPEAT_KEY_ENABLE = yes        # QK_REP for repeat last key
MOUSEKEY_ENABLE = yes          # Built-in mouse keys
KEY_OVERRIDE_ENABLE = yes      # Key overrides (Shift+Bspc = Del)
COMBO_ENABLE = yes             # Combos for shortcuts
UNICODEMAP_ENABLE = yes        # Unicode pairs for umlauts (CMD layer)
LEADER_ENABLE = yes            # Leader key sequences
OS_DETECTION_ENABLE = yes      # Enabled - works fine, LED was the problem
AUTOCORRECT_ENABLE = yes       # Built-in typo autocorrect engine

# Advanced Features
TRI_LAYER_ENABLE = yes         # Enabled - now works with Layer-Tap (trigger keys on both layers)

# Size optimization
LTO_ENABLE = yes               # Link Time Optimization
CONSOLE_ENABLE = no            # Disable console for size
COMMAND_ENABLE = no            # Disable command for size

# Extra sources for this keymap
SRC += leader_actions.c
