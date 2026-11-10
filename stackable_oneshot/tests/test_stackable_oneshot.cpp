// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

#include "keyboard_report_util.hpp"
#include "mouse_report_util.hpp"
#include "test_common.hpp"
#include "stackable_oneshot_harness.h"

using testing::_;
using testing::InSequence;

class StackableOneShot : public TestFixture {
   protected:
    void SetUp() override {
        stackable_oneshot_test_reset();
    }

    KeymapKey lctl = KeymapKey(0, 0, 0, SOSM_LC);
    KeymapKey lsft = KeymapKey(0, 1, 0, SOSM_LS);
    KeymapKey lgui = KeymapKey(0, 2, 0, SOSM_LG);
    KeymapKey key_a = KeymapKey(0, 3, 0, KC_A);
    KeymapKey key_w = KeymapKey(0, 4, 0, KC_W);
    KeymapKey btn1 = KeymapKey(0, 5, 0, MS_BTN1);

    void use_all_keys() {
        set_keymap({lctl, lsft, lgui, key_a, key_w, btn1});
    }

    // Taps the mouse button, expecting the click and its release in the mouse reports.
    void click(TestDriver &driver) {
        EXPECT_MOUSE_REPORT(driver, (0, 0, 0, 0, 1));
        EXPECT_EMPTY_MOUSE_REPORT(driver);
        tap_key(btn1);
    }
};

// The basic one-shot: tap the modifier, and it applies to the next key only.
TEST_F(StackableOneShot, TapAppliesToNextKeyOnly) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    // The modifier goes to the host on press; the release leaves it armed.
    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LSFT, KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);

    // Used up: the next key is unmodified.
    EXPECT_REPORT(driver, (KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// Chording several at once arms them all. This is what OSM() gets wrong.
TEST_F(StackableOneShot, ChordedTapsAllApply) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LCTL));
    EXPECT_REPORT(driver, (KC_LCTL, KC_LSFT));
    tap_combo({lctl, lsft});
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LCTL, KC_LSFT, KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// Tapping them one after another stacks them too.
TEST_F(StackableOneShot, RolledTapsAllApply) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LCTL));
    tap_key(lctl);
    EXPECT_REPORT(driver, (KC_LCTL, KC_LSFT));
    tap_key(lsft);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LCTL, KC_LSFT, KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// Held past the tapping term, it's a plain modifier and leaves nothing armed.
TEST_F(StackableOneShot, HoldLeavesNothingArmed) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LSFT));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(lsft, STACKABLE_ONESHOT_TAPPING_TERM + 1);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// Holding it with a key works like a normal modifier.
TEST_F(StackableOneShot, HoldWithKeyIsNormalModifier) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LSFT));
    lsft.press();
    run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_LSFT, KC_A));
    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(key_a);
    EXPECT_EMPTY_REPORT(driver);
    lsft.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

// Holding the key a one-shot applied to keeps the modifier, so the host repeats the shifted key.
TEST_F(StackableOneShot, HeldKeyKeepsModifierUntilReleased) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);

    EXPECT_REPORT(driver, (KC_LSFT, KC_A));
    key_a.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_NO_REPORT(driver);
    idle_for(500);
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    key_a.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

// An unused one-shot is released on the host when the idle timeout runs out, without a key press.
TEST_F(StackableOneShot, IdleTimeoutReleasesModifier) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);
    VERIFY_AND_CLEAR(driver);

    EXPECT_NO_REPORT(driver);
    idle_for(STACKABLE_ONESHOT_TIMEOUT - 50);
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    idle_for(100);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_A));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_a);
    VERIFY_AND_CLEAR(driver);
}

TEST_F(StackableOneShot, CancelClearsArmedModifier) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LSFT));
    tap_key(lsft);
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    EXPECT_TRUE(stackable_oneshot_cancel());
    VERIFY_AND_CLEAR(driver);

    // Nothing left to cancel.
    EXPECT_NO_REPORT(driver);
    EXPECT_FALSE(stackable_oneshot_cancel());
    VERIFY_AND_CLEAR(driver);
}

// A quick modifier-click: hold GUI, click, let go of GUI within the tapping term. The press was
// used for the click, so it must not leave GUI armed for the next key (here W, which would make
// GUI+W and close a tab).
TEST_F(StackableOneShot, QuickModifierClickLeavesNothingArmed) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LGUI));
    lgui.press();
    run_one_scan_loop();
    click(driver);
    EXPECT_EMPTY_REPORT(driver);
    lgui.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_W));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_w);
    VERIFY_AND_CLEAR(driver);
}

// After a modifier-click, the next tap of the same key is an ordinary one-shot again.
TEST_F(StackableOneShot, TapAfterModifierClickIsOneShot) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LGUI));
    lgui.press();
    run_one_scan_loop();
    click(driver);
    EXPECT_EMPTY_REPORT(driver);
    lgui.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LGUI));
    tap_key(lgui);
    EXPECT_REPORT(driver, (KC_LGUI, KC_W));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_w);
    VERIFY_AND_CLEAR(driver);
}

// A click after a tap (key already released) doesn't use up the one-shot, as the README says:
// the click gets the modifier, and so does the next key.
TEST_F(StackableOneShot, ClickAfterTapKeepsOneShotArmed) {
    TestDriver driver;
    InSequence s;
    use_all_keys();

    EXPECT_REPORT(driver, (KC_LGUI));
    tap_key(lgui);
    click(driver);
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_LGUI, KC_W));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(key_w);
    VERIFY_AND_CLEAR(driver);
}
