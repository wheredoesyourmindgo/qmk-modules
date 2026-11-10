# Copyright 2026 wheredoesyourmindgo
# SPDX-License-Identifier: GPL-2.0-or-later

# Clicks from a pointing device (trackball, trackpad) instead of mouse keys

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
MOUSEKEY_ENABLE = no

VPATH += $(TEST_PATH)/..

SRC += stackable_oneshot_harness.c
