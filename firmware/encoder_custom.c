// Custom Encoder Implementation for Cheapino with Layer-specific Actions
// Based on original cheapino/encoder.c with modifications for layer support

#include "quantum.h"
#include "gpio.h"

#define COL_SHIFTER ((matrix_row_t)1)
#define ENC_ROW 1
#define ENC_A_COL 5
#define ENC_B_COL 0
#define ENC_BUTTON_COL 4

static uint8_t colABPressed = 0;
static bool encoderPressed = false;

// Handle encoder button click
static void clicked(void) {
    tap_code(KC_MPLY);  // Media Play/Pause
}

// Handle encoder rotation based on active layer
static void turned(bool clockwise) {
    uint8_t layer = get_highest_layer(layer_state);

    switch (layer) {
        case 0:  // Base Layer - Mouse Wheel (as per Vial config)
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

// Process encoder signals from matrix
void fix_encoder_action(matrix_row_t current_matrix[]) {
    static uint8_t prevColABPressed = 0;
    colABPressed = 0;

    // Step 1: Read encoder A/B pins from matrix (for rotation)
    if (current_matrix[ENC_ROW] & (COL_SHIFTER << ENC_A_COL)) {
        colABPressed |= 1;
    }
    if (current_matrix[ENC_ROW] & (COL_SHIFTER << ENC_B_COL)) {
        colABPressed |= 2;
    }

    // Step 2: Read button press status BEFORE clearing matrix
    bool currentPressed = current_matrix[ENC_ROW] & (COL_SHIFTER << ENC_BUTTON_COL);

    // Step 3: NOW clear encoder pins from matrix so they don't register as key presses
    current_matrix[ENC_ROW] &= ~(COL_SHIFTER << ENC_A_COL);
    current_matrix[ENC_ROW] &= ~(COL_SHIFTER << ENC_B_COL);
    current_matrix[ENC_ROW] &= ~(COL_SHIFTER << ENC_BUTTON_COL);

    // Step 4: Detect rotation direction with proper state machine
    // State transitions:
    // Clockwise:     00 -> 10 -> 11 -> 01 -> 00
    // Counter-CW:    00 -> 01 -> 11 -> 10 -> 00
    if (colABPressed != prevColABPressed) {
        if (prevColABPressed == 0 && colABPressed == 2) {
            // Start of clockwise rotation
            turned(true);
        } else if (prevColABPressed == 0 && colABPressed == 1) {
            // Start of counter-clockwise rotation
            turned(false);
        }
    }
    prevColABPressed = colABPressed;

    // Step 5: Handle button press
    if (currentPressed && !encoderPressed) {
        clicked();
    }
    encoderPressed = currentPressed;
}
