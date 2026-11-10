// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

// Stands in for what QMK generates from qmk_module.json in a real build (community_modules.h):
// QMK's test framework doesn't build community modules, so the tests supply the keycodes and
// hook declarations themselves.

#pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(ver_major, ver_minor, ver_patch) STATIC_ASSERT(1, "")

enum stackable_oneshot_keycodes {
    STACKABLE_ONESHOT_LCTL = QK_COMMUNITY_MODULE,
    STACKABLE_ONESHOT_LSFT,
    STACKABLE_ONESHOT_LALT,
    STACKABLE_ONESHOT_LGUI,
    STACKABLE_ONESHOT_RCTL,
    STACKABLE_ONESHOT_RSFT,
    STACKABLE_ONESHOT_RALT,
    STACKABLE_ONESHOT_RGUI,
};

#define SOSM_LC STACKABLE_ONESHOT_LCTL
#define SOSM_LS STACKABLE_ONESHOT_LSFT
#define SOSM_LA STACKABLE_ONESHOT_LALT
#define SOSM_LG STACKABLE_ONESHOT_LGUI
#define SOSM_RC STACKABLE_ONESHOT_RCTL
#define SOSM_RS STACKABLE_ONESHOT_RSFT
#define SOSM_RA STACKABLE_ONESHOT_RALT
#define SOSM_RG STACKABLE_ONESHOT_RGUI

bool process_record_stackable_oneshot(uint16_t keycode, keyrecord_t *record);
bool process_record_stackable_oneshot_kb(uint16_t keycode, keyrecord_t *record);
void housekeeping_task_stackable_oneshot(void);
#ifdef POINTING_DEVICE_ENABLE
report_mouse_t pointing_device_task_stackable_oneshot(report_mouse_t mouse_report);
report_mouse_t pointing_device_task_stackable_oneshot_kb(report_mouse_t mouse_report);
#endif

// Resets the module's internal state, so each test starts clean.
void stackable_oneshot_test_reset(void);

#ifdef __cplusplus
}
#endif

#include "stackable_oneshot.h"
