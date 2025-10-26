// Custom Encoder Implementation for Cheapino
// Layer-specific encoder actions based on Vial config

#include "quantum.h"
#include "gpio.h"

// Encoder pin definitions for Cheapino v2
#define ENC_ROW 1
#define ENC_A_COL 5
#define ENC_B_COL 0
#define ENC_BUTTON_COL 4

// Pin state tracking
static uint8_t colABPressed = 0;
static bool encoderPressed = false;

// Initialize encoder pins
void encoder_init(void) {
    // Encoder pins are already initialized by matrix.c
    // This function is called by keyboard_post_init_user if needed
}

// Handle encoder rotation based on active layer
static void turned(bool clockwise) {
    uint8_t layer = get_highest_layer(layer_state);

    switch (layer) {
        case 0:  // Base Layer - Scroll Wheel
            tap_code(clockwise ? KC_WH_U : KC_WH_D);
            break;

        case 1:  // Media Layer - Volume
            tap_code(clockwise ? KC_VOLU : KC_VOLD);
            break;

        case 2:  // Navigation Layer - Arrow Keys Left/Right
            tap_code(clockwise ? KC_RGHT : KC_LEFT);
            break;

        case 3:  // Mouse Layer - No encoder action
        case 4:  // Symbol Layer - No encoder action
        case 5:  // Number Layer - No encoder action
        case 6:  // F-Key Layer - No encoder action
        default:
            // No action on these layers (as per Vial config)
            break;
    }
}

// Process encoder rotation signals
void fix_encoder_action(bool *matrix, uint8_t row) {
    if (row == ENC_ROW) {
        bool prevColABPressed = colABPressed;
        colABPressed = 0;

        // Read encoder A and B pins
        if (matrix[ENC_A_COL]) {
            colABPressed |= 1;
        }
        if (matrix[ENC_B_COL]) {
            colABPressed |= 2;
        }

        // Detect rotation direction based on state changes
        // State machine: 00 -> 01 -> 11 -> 10 -> 00 (clockwise)
        //                00 -> 10 -> 11 -> 01 -> 00 (counter-clockwise)
        if (prevColABPressed == 0 && colABPressed == 1) {
            // Start of clockwise rotation
            turned(true);
        } else if (prevColABPressed == 0 && colABPressed == 2) {
            // Start of counter-clockwise rotation
            turned(false);
        }
    }
}

// Handle encoder button press (play/pause as default)
bool encoder_button_pressed(void) {
    return encoderPressed;
}

void set_encoder_button_pressed(bool pressed) {
    if (pressed && !encoderPressed) {
        // Button just pressed - send media play/pause
        tap_code(KC_MPLY);
    }
    encoderPressed = pressed;
}
