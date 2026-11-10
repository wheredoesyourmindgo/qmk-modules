// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

// QMK's ONESHOT_TIMEOUT clears one-shot mods without sending a report. The module sends its
// modifier to the host on press, so it must send the report itself, or the host keeps the
// modifier held. QMK's timeout is set shorter than the module's, so QMK's fires first.

#include "keyboard_report_util.hpp"
#include "test_common.hpp"
#include "../stackable_oneshot_harness.h"

using testing::_;
using testing::InSequence;

class StackableOneShotQmkTimeout : public TestFixture {
   protected:
    void SetUp() override {
        stackable_oneshot_test_reset();
    }

    KeymapKey lsft  = KeymapKey(0, 0, 0, SOSM_LS);
    KeymapKey key_a = KeymapKey(0, 1, 0, KC_A);
};

TEST_F(StackableOneShotQmkTimeout, QmkTimeoutReleasesModifierOnHost) {
    TestDriver driver;
    InSequence s;
    set_keymap({lsft, key_a});

    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);
    VERIFY_AND_CLEAR(driver);

    EXPECT_NO_REPORT(driver);
    idle_for(ONESHOT_TIMEOUT - 50);
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    idle_for(100);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// With QMK's timeout set, one-shots are still used up normally.
TEST_F(StackableOneShotQmkTimeout, TapAppliesToNextKey) {
    TestDriver driver;
    InSequence s;
    set_keymap({lsft, key_a});

    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);
    EXPECT_REPORT(driver, (KC_LSFT, KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}
