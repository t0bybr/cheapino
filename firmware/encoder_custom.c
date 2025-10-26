// Original Cheapino encoder.c with custom layer-specific actions
// Based on: https://github.com/tompi/qmk_firmware/blob/cheapinov2/keyboards/cheapino/encoder.c

#include "matrix.h"
#include "quantum.h"

#define COL_SHIFTER ((uint16_t)1)

#define ENC_ROW 3
#define ENC_A_COL 2
#define ENC_B_COL 4
#define ENC_BUTTON_COL 0

static bool colABPressed   = false;
static bool encoderPressed = false;

void clicked(void) {
    tap_code(KC_MPLY);
}

// Custom turned() function with layer-specific actions (from Vial config)
void turned(bool clockwise) {
    uint8_t layer = get_highest_layer(layer_state);

    switch (layer) {
        case 0:  // Base Layer - Mouse Wheel (as per Vial config)
            tap_code(clockwise ? KC_WH_U : KC_WH_D);
            break;

        case 1:  // Media Layer - Volume (inverted direction)
            tap_code(clockwise ? KC_VOLD : KC_VOLU);
            break;

        case 2:  // Navigation Layer - Arrow Keys Left/Right (inverted direction)
            tap_code(clockwise ? KC_LEFT : KC_RGHT);
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

// Original fix_encoder_action() - DO NOT MODIFY!
void fix_encoder_action(matrix_row_t current_matrix[]) {
    matrix_row_t encoder_row = current_matrix[ENC_ROW];

    if (encoder_row & (COL_SHIFTER << ENC_BUTTON_COL)) {
        encoderPressed = true;
    } else {
        // Only trigger click on release
        if (encoderPressed) {
            encoderPressed = false;
            clicked();
        }
    }

    // Check which way the encoder is turned:
    bool colA = encoder_row & (COL_SHIFTER << ENC_A_COL);
    bool colB = encoder_row & (COL_SHIFTER << ENC_B_COL);

    if (colA && colB) {
        colABPressed = true;
    } else if (colA) {
        if (colABPressed) {
            // A+B followed by A means clockwise
            colABPressed = false;
            turned(true);
        }
    } else if (colB) {
        if (colABPressed) {
            // A+B followed by B means counter-clockwise
            colABPressed = false;
            turned(false);
        }
    }
    current_matrix[ENC_ROW] = 0;
}
