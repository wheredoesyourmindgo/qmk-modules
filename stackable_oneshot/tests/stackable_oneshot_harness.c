// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

// Builds the module for the tests and wires its hooks in the way QMK's generated
// community_modules.c does: QMK calls process_record_modules() before process_record_kb(), and
// housekeeping_task_modules() and pointing_device_task_modules() from their usual places.

#include <string.h>
#include "stackable_oneshot_harness.h"
#include "stackable_oneshot.c"

__attribute__((weak)) bool process_record_stackable_oneshot_kb(uint16_t keycode, keyrecord_t *record) {
    return true;
}

bool process_record_modules(uint16_t keycode, keyrecord_t *record) {
    return process_record_stackable_oneshot(keycode, record);
}

void housekeeping_task_modules(void) {
    housekeeping_task_stackable_oneshot();
}

#ifdef POINTING_DEVICE_ENABLE
__attribute__((weak)) report_mouse_t pointing_device_task_stackable_oneshot_kb(report_mouse_t mouse_report) {
    return mouse_report;
}

report_mouse_t pointing_device_task_modules(report_mouse_t mouse_report) {
    return pointing_device_task_stackable_oneshot(mouse_report);
}
#endif

void stackable_oneshot_test_reset(void) {
    memset(press_timers, 0, sizeof(press_timers));
    click_done = false;
    armed      = false;
}
