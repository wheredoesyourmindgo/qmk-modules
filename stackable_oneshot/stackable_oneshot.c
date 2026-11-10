// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stackable_oneshot.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#ifdef NO_ACTION_ONESHOT
#    error "Stackable One-Shot needs QMK's one-shot support; remove NO_ACTION_ONESHOT."
#endif

// Press time of each held key, indexed by its position in the keycode list.
_Static_assert(STACKABLE_ONESHOT_RGUI - STACKABLE_ONESHOT_LCTL == 7, "Stackable One-Shot keycodes must be contiguous");
static uint16_t press_timers[8];

// Set while a one-shot from this module may still be visible to the host.
static bool armed = false;
#if STACKABLE_ONESHOT_TIMEOUT > 0
// When the most recent one-shot was armed; only checked while armed is set.
static uint16_t armed_at = 0;
#endif

bool stackable_oneshot_cancel(void) {
    if (!get_oneshot_mods()) {
        return false;
    }
    clear_oneshot_mods();
    send_keyboard_report();
    return true;
}

// This module sends a one-shot to the host as soon as it is pressed, so the host keeps
// seeing it until a report goes out without it. This makes sure one does.
void housekeeping_task_stackable_oneshot(void) {
    if (!armed) {
        return;
    }
    if (!get_oneshot_mods()) {
        // Used by a key, cancelled, or cleared elsewhere. QMK's own ONESHOT_TIMEOUT (and other
        // code calling clear_oneshot_mods()) clears it without sending a report, which would leave
        // the modifier held on the host. While a key is still held, skip this: the one-shot was
        // used by that key, and its release sends a report anyway. Sending one now would drop the
        // modifier early and change what the host repeats.
        if (!has_anykey()) {
            armed = false;
            send_keyboard_report();
        }
    }
#if STACKABLE_ONESHOT_TIMEOUT > 0
    else if (timer_elapsed(armed_at) > STACKABLE_ONESHOT_TIMEOUT) {
        armed = false;
        clear_oneshot_mods();
        send_keyboard_report();
    }
#endif
}

bool process_record_stackable_oneshot(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_stackable_oneshot_kb(keycode, record)) {
        return false;
    }

    uint8_t mod_bit;
    switch (keycode) {
        case STACKABLE_ONESHOT_LCTL: mod_bit = MOD_BIT(KC_LCTL); break;
        case STACKABLE_ONESHOT_LSFT: mod_bit = MOD_BIT(KC_LSFT); break;
        case STACKABLE_ONESHOT_LALT: mod_bit = MOD_BIT(KC_LALT); break;
        case STACKABLE_ONESHOT_LGUI: mod_bit = MOD_BIT(KC_LGUI); break;
        case STACKABLE_ONESHOT_RCTL: mod_bit = MOD_BIT(KC_RCTL); break;
        case STACKABLE_ONESHOT_RSFT: mod_bit = MOD_BIT(KC_RSFT); break;
        case STACKABLE_ONESHOT_RALT: mod_bit = MOD_BIT(KC_RALT); break;
        case STACKABLE_ONESHOT_RGUI: mod_bit = MOD_BIT(KC_RGUI); break;
        default: return true;
    }
    uint16_t *press_timer = &press_timers[keycode - STACKABLE_ONESHOT_LCTL];

    if (record->event.pressed) {
        // Press the modifier now and arm the one-shot, so it works whether this is a tap or a hold
        register_mods(mod_bit);
        add_oneshot_mods(mod_bit);
        *press_timer = timer_read();
        armed        = true;
#if STACKABLE_ONESHOT_TIMEOUT > 0
        armed_at = *press_timer;
#endif
        send_keyboard_report();
    } else {
        unregister_mods(mod_bit);
        // A hold was a plain modifier, so leave nothing armed once it's released
        if (timer_elapsed(*press_timer) > STACKABLE_ONESHOT_TAPPING_TERM) {
            del_oneshot_mods(mod_bit);
            send_keyboard_report();
        }
    }
    return false;
}
