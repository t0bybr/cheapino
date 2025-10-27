# Custom Cheapino Keymap Build Rules

# Enable core features
MOUSEKEY_ENABLE = no            # Standard Mouse Keys disabled (using Orbital Mouse)
MOUSE_ENABLE = yes              # Mouse Reports required for Orbital Mouse
EXTRAKEY_ENABLE = yes           # Audio control and System control
CONSOLE_ENABLE = no             # Console for debug
COMMAND_ENABLE = no             # Commands for debug and configuration
NKRO_ENABLE = yes              # N-Key Rollover
BACKLIGHT_ENABLE = no          # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = yes          # Enable keyboard RGB underglow (required by cheapino.c)
AUDIO_ENABLE = no              # Audio output
# ENCODER_ENABLE = no          # Cheapino uses custom encoder.c from keyboard level
# ENCODER_MAP_ENABLE = yes     # Disabled - Cheapino uses custom encoder.c

# Advanced features
COMBO_ENABLE = no              # Key combos (disabled - not used in this keymap)
REPEAT_KEY_ENABLE = yes        # Repeat key functionality (QMK 0.20+)
KEY_OVERRIDE_ENABLE = no       # Key overrides (disabled - not used in this keymap)

# Space optimization
LTO_ENABLE = yes               # Link Time Optimization
SPACE_CADET_ENABLE = no        # Space cadet shift
GRAVE_ESC_ENABLE = no          # Grave escape

# Custom source files
SRC += features/achordion.c    # Achordion for better home row mods

COMMUNITY_MODULES += getreuer/orbital_mouse