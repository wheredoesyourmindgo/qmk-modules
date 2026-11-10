#!/usr/bin/env bash
# Copyright 2026 wheredoesyourmindgo
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Runs a module's tests with QMK's test framework.
#
#   scripts/test.sh                      # every module that has tests
#   scripts/test.sh stackable_oneshot    # one module
#
# QMK only builds tests from its own tests/ folder (and doesn't follow symlinks there), so this
# copies the module's sources and tests into qmk_firmware/tests/<module>, runs them, and removes
# the copy again. qmk_firmware is found with `qmk env QMK_FIRMWARE`; set QMK_FIRMWARE to override.

set -euo pipefail

repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
qmk_firmware="${QMK_FIRMWARE:-$(qmk env QMK_FIRMWARE 2>/dev/null || true)}"
if [[ -z "$qmk_firmware" || ! -f "$qmk_firmware/tests/test_common/test_fixture.hpp" ]]; then
    echo "Can't find qmk_firmware; set QMK_FIRMWARE to its path." >&2
    exit 1
fi

if [[ $# -gt 0 ]]; then
    modules=("$@")
else
    modules=()
    for dir in "$repo"/*/tests; do
        [[ -d "$dir" ]] && modules+=("$(basename "$(dirname "$dir")")")
    done
fi

status=0
for module in "${modules[@]}"; do
    src="$repo/$module"
    dest="$qmk_firmware/tests/$module"
    if [[ ! -d "$src/tests" ]]; then
        echo "$module has no tests folder." >&2
        exit 1
    fi
    if [[ -e "$dest" && ! -f "$dest/.qmk_modules_test_copy" ]]; then
        echo "$dest already exists and wasn't made by this script; not touching it." >&2
        exit 1
    fi

    rm -rf "$dest"
    cp -R "$src/tests" "$dest"
    cp "$src"/*.c "$src"/*.h "$dest/"
    touch "$dest/.qmk_modules_test_copy"
    trap 'rm -rf "$dest"' EXIT

    # Each folder with a test.mk is its own test build (subfolders test other configs), and
    # `make test:<name>` only matches a folder's exact path.
    while IFS= read -r test_mk; do
        name="${test_mk#"$qmk_firmware/tests/"}"
        name="${name%/test.mk}"
        echo "== $name"
        make -C "$qmk_firmware" "test:$name" || status=1
    done < <(find "$dest" -name test.mk | sort)

    rm -rf "$dest"
    trap - EXIT
done
exit $status
