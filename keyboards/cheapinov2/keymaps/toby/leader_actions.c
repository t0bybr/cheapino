// Copyright 2024
// Leader actions: OS-aware window management and common shortcuts

#include QMK_KEYBOARD_H
#include "leader_actions.h"
#include "os_detection.h"
#include "keyboards/cheapinov2/keymaps/toby/config.h"
#include "leader_map.h"

// Helpers to expand a declarative map into imperative checks
#define APPLY_LEADER1(K1, ACT_LNX, ACT_MAC, ACT_WIN) \
    if (leader_sequence_one_key(K1)) { \
        os_variant_t _os = detected_host_os(); \
        if (_os == OS_MACOS || _os == OS_IOS) tap_code16(ACT_MAC); \
        else if (_os == OS_WINDOWS) tap_code16(ACT_WIN); \
        else tap_code16(ACT_LNX); \
    }

#define APPLY_LEADER2(K1, K2, ACT_LNX, ACT_MAC, ACT_WIN) \
    if (leader_sequence_two_keys(K1, K2)) { \
        os_variant_t _os = detected_host_os(); \
        if (_os == OS_MACOS || _os == OS_IOS) tap_code16(ACT_MAC); \
        else if (_os == OS_WINDOWS) tap_code16(ACT_WIN); \
        else tap_code16(ACT_LNX); \
    }

#define APPLY_LEADER3(K1, K2, K3, ACT_LNX, ACT_MAC, ACT_WIN) \
    if (leader_sequence_three_keys(K1, K2, K3)) { \
        os_variant_t _os = detected_host_os(); \
        if (_os == OS_MACOS || _os == OS_IOS) tap_code16(ACT_MAC); \
        else if (_os == OS_WINDOWS) tap_code16(ACT_WIN); \
        else tap_code16(ACT_LNX); \
    }

void leader_handle_sequences(bool is_macos) {
    // One-key sequences
#ifdef LEADER_MAP_1KEY
#define LEADER1(K1, ACT_LNX, ACT_MAC, ACT_WIN) APPLY_LEADER1(K1, ACT_LNX, ACT_MAC, ACT_WIN)
    LEADER_MAP_1KEY
#undef LEADER1
#endif

    // Two-key sequences
#ifdef LEADER_MAP_2KEY
#define LEADER2(K1, K2, ACT_LNX, ACT_MAC, ACT_WIN) APPLY_LEADER2(K1, K2, ACT_LNX, ACT_MAC, ACT_WIN)
    LEADER_MAP_2KEY
#undef LEADER2
#endif

    // Three-key sequences
#ifdef LEADER_MAP_3KEY
#define LEADER3(K1, K2, K3, ACT_LNX, ACT_MAC, ACT_WIN) APPLY_LEADER3(K1, K2, K3, ACT_LNX, ACT_MAC, ACT_WIN)
    LEADER_MAP_3KEY
#undef LEADER3
#endif
}
