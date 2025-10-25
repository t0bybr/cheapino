# Custom Cheapino Keymap Build Rules

# Enable core features
MOUSEKEY_ENABLE = yes           # Mouse keys
EXTRAKEY_ENABLE = yes           # Audio control and System control
CONSOLE_ENABLE = no             # Console for debug
COMMAND_ENABLE = no             # Commands for debug and configuration
NKRO_ENABLE = yes              # N-Key Rollover
BACKLIGHT_ENABLE = no          # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no           # Enable keyboard RGB underglow
AUDIO_ENABLE = no              # Audio output
# ENCODER_ENABLE = no          # Cheapino uses custom encoder.c from keyboard level
# ENCODER_MAP_ENABLE = yes     # Disabled - Cheapino uses custom encoder.c

# Advanced features
COMBO_ENABLE = yes             # Key combos
REPEAT_KEY_ENABLE = yes        # Repeat key functionality (QMK 0.20+)
KEY_OVERRIDE_ENABLE = yes      # Key overrides

# Space optimization
LTO_ENABLE = yes               # Link Time Optimization
SPACE_CADET_ENABLE = no        # Space cadet shift
GRAVE_ESC_ENABLE = no          # Grave escape

# Custom source files
SRC += features/achordion.c    # Achordion for better home row mods
