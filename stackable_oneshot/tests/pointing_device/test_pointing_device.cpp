// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

// A quick modifier-click with a pointing device's button (trackball, trackpad) instead of a mouse
// key must not leave the modifier armed either.

#include "keyboard_report_util.hpp"
#include "mouse_report_util.hpp"
#include "test_common.hpp"
#include "test_pointing_device_driver.h"
#include "../stackable_oneshot_harness.h"

using testing::_;
using testing::InSequence;

class StackableOneShotPointing : public TestFixture {
   protected:
    void SetUp() override {
        stackable_oneshot_test_reset();
    }

    void TearDown() override {
        pd_clear_all_buttons();
    }

    KeymapKey lgui  = KeymapKey(0, 0, 0, SOSM_LG);
    KeymapKey key_w = KeymapKey(0, 1, 0, KC_W);
};

TEST_F(StackableOneShotPointing, QuickModifierClickLeavesNothingArmed) {
    TestDriver driver;
    InSequence s;
    set_keymap({lgui, key_w});

    EXPECT_REPORT(driver, (KC_LGUI));
    lgui.press();
    run_one_scan_loop();

    EXPECT_MOUSE_REPORT(driver, (0, 0, 0, 0, 1));
    pd_press_button(POINTING_DEVICE_BUTTON1);
    run_one_scan_loop();
    EXPECT_EMPTY_MOUSE_REPORT(driver);
    pd_release_button(POINTING_DEVICE_BUTTON1);
    run_one_scan_loop();

    EXPECT_EMPTY_REPORT(driver);
    lgui.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_W));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_w);
    VERIFY_AND_CLEAR(driver);
}

// Moving the pointer isn't a click, so a quick tap while it moves is still a one-shot.
TEST_F(StackableOneShotPointing, MovementDoesNotCountAsClick) {
    TestDriver driver;
    // Only the keyboard reports matter here.
    EXPECT_ANY_MOUSE_REPORT(driver).Times(testing::AnyNumber());
    InSequence s;
    set_keymap({lgui, key_w});

    EXPECT_REPORT(driver, (KC_LGUI));
    lgui.press();
    run_one_scan_loop();

    pd_set_x(5);
    run_one_scan_loop();
    pd_clear_movement();
    run_one_scan_loop();

    lgui.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LGUI, KC_W));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_w);
    VERIFY_AND_CLEAR(driver);
}
