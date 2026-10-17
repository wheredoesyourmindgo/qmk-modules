// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

#include "instant_oneshot.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#ifdef NO_ACTION_ONESHOT
#    error "Instant One-Shot needs QMK's one-shot support; remove NO_ACTION_ONESHOT."
#endif

// Press time of each held key, indexed by its position in the keycode list.
_Static_assert(INSTANT_ONESHOT_RGUI - INSTANT_ONESHOT_LCTL == 7, "Instant One-Shot keycodes must be contiguous");
static uint16_t press_timers[8];

#if INSTANT_ONESHOT_TIMEOUT > 0
// When the most recent one-shot was armed; only checked while timeout_armed is set.
static uint16_t armed_at      = 0;
static bool     timeout_armed = false;
#endif

bool instant_oneshot_cancel(void) {
    if (!get_oneshot_mods()) {
        return false;
    }
    clear_oneshot_mods();
    send_keyboard_report();
    return true;
}

#if INSTANT_ONESHOT_TIMEOUT > 0
// Unlike QMK's ONESHOT_TIMEOUT, which is only checked on the next key event, this clears the
// modifier from the host on time, so a later mouse click doesn't pick it up.
void housekeeping_task_instant_oneshot(void) {
    if (!timeout_armed) {
        return;
    }
    if (!get_oneshot_mods()) {
        timeout_armed = false; // Used or cancelled
    } else if (timer_elapsed(armed_at) > INSTANT_ONESHOT_TIMEOUT) {
        timeout_armed = false;
        clear_oneshot_mods();
        send_keyboard_report();
    }
}
#endif

bool process_record_instant_oneshot(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_instant_oneshot_kb(keycode, record)) {
        return false;
    }

    uint8_t mod_bit;
    switch (keycode) {
        case INSTANT_ONESHOT_LCTL: mod_bit = MOD_BIT(KC_LCTL); break;
        case INSTANT_ONESHOT_LSFT: mod_bit = MOD_BIT(KC_LSFT); break;
        case INSTANT_ONESHOT_LALT: mod_bit = MOD_BIT(KC_LALT); break;
        case INSTANT_ONESHOT_LGUI: mod_bit = MOD_BIT(KC_LGUI); break;
        case INSTANT_ONESHOT_RCTL: mod_bit = MOD_BIT(KC_RCTL); break;
        case INSTANT_ONESHOT_RSFT: mod_bit = MOD_BIT(KC_RSFT); break;
        case INSTANT_ONESHOT_RALT: mod_bit = MOD_BIT(KC_RALT); break;
        case INSTANT_ONESHOT_RGUI: mod_bit = MOD_BIT(KC_RGUI); break;
        default: return true;
    }
    uint16_t *press_timer = &press_timers[keycode - INSTANT_ONESHOT_LCTL];

    if (record->event.pressed) {
        // Press the modifier now and arm the one-shot, so it works whether this is a tap or a hold
        register_mods(mod_bit);
        add_oneshot_mods(mod_bit);
        *press_timer = timer_read();
#if INSTANT_ONESHOT_TIMEOUT > 0
        armed_at      = *press_timer;
        timeout_armed = true;
#endif
        send_keyboard_report();
    } else {
        unregister_mods(mod_bit);
        // A hold was a plain modifier, so leave nothing armed once it's released
        if (timer_elapsed(*press_timer) > INSTANT_ONESHOT_TAPPING_TERM) {
            del_oneshot_mods(mod_bit);
            send_keyboard_report();
        }
    }
    return false;
}
