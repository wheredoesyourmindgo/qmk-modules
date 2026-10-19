// Copyright 2026 wheredoesyourmindgo
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file stackable_oneshot.h
 * @brief Stackable One-Shot community module: one-shot mods that press the modifier immediately.
 *
 * See README.md for usage and how it differs from QMK's OSM().
 */

#pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

// Hold longer than this (ms) and the key acts as a plain modifier; tap faster and it stays one-shot.
#ifndef STACKABLE_ONESHOT_TAPPING_TERM
#    define STACKABLE_ONESHOT_TAPPING_TERM 300
#endif

// An unused one-shot is cleared after this many ms. 0 disables the timeout.
#ifndef STACKABLE_ONESHOT_TIMEOUT
#    define STACKABLE_ONESHOT_TIMEOUT 3000
#endif

/**
 * Clears any pending one-shot mods and tells the host right away.
 *
 * Returns true if there was something to clear, so callers can make a key (such as Escape)
 * cancel a one-shot first and do its normal job only when nothing was pending.
 */
bool stackable_oneshot_cancel(void);

#ifdef __cplusplus
}
#endif
