CAPS_WORD_ENABLE = yes
CUSTOM_MATRIX = lite
WS2812_DRIVER = vendor
RGBLIGHT_ENABLE = yes
DEFERRED_EXEC_ENABLE = yes

# EEPROM configuration for RP2040
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = rp2040_flash

SRC += encoder.c
SRC += ghosting.c
SRC += matrix.c
