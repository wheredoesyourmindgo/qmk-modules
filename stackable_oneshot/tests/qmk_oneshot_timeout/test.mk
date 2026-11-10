# Copyright 2026 wheredoesyourmindgo
# SPDX-License-Identifier: GPL-2.0-or-later

# QMK's own ONESHOT_TIMEOUT set, shorter than the module's

VPATH += $(TEST_PATH)/..

SRC += stackable_oneshot_harness.c
