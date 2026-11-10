# Stackable One-Shot

One-shot modifiers you can stack reliably. Chord any number of them (Ctrl+Alt+GUI+Shift, all at once or one after another) and every one applies to the next key or click, every time.

- **Tap** (under 300 ms): the modifier applies to the next key or click, then releases.
- **Hold** (300 ms or longer): it acts as a normal modifier and leaves nothing armed when you let go.
- **Idle timeout**: a one-shot you don't use clears itself after 3 seconds.

## How it differs from QMK's `OSM()`

`OSM()` keys wait to find out whether you tapped or held before arming. When you press several together, each press interrupts the one before it, so QMK treats the earlier keys as holds, and those modifiers are gone once you let go. Chording one-shot mods with `OSM()` drops some of them.

Stackable One-Shot presses the modifier and arms the one-shot the moment each key goes down, and each key times its own tap or hold. No key waits on another, so:

- Stacked one-shots always all apply.
- Holding it has no delay, and modifier-clicks with a mouse (Cmd+click, Shift+click) work while the key is held.
- A tapped one-shot also applies to a mouse click, not just the next key. `OSM()` doesn't tell the host about the modifier until the next key, so a click never sees it.

A click uses up the one-shot like a key does: tap Cmd, click, and you get one Cmd+click. The modifier is released after the mouse button comes up, so the whole click, or a Cmd+drag, has it.

### Trade-off: the host sees the modifier while it's armed

To make mouse clicks work, the modifier is sent to the host as soon as you press it. It stays held there until a key or click uses it, you cancel it, or the idle timeout clears it. That has some side effects `OSM()` doesn't have:

- Other things that react to a held modifier react to it too, e.g. Ctrl + scroll wheel zooms in most apps.
- If an armed GUI or Alt times out unused, the host sees a lone tap of it. On Windows and Linux that can open the Start menu or launcher (GUI) or focus the menu bar (Alt).

Lower `STACKABLE_ONESHOT_TIMEOUT` if these get in the way.

### Clicks from a separate mouse

A click only uses up the one-shot when the keyboard sees it: a mouse key (`MS_BTN1` and so on) or a pointing device built into the keyboard. A separate USB mouse talks to the host directly, so the keyboard never knows. With one, a tapped one-shot applies to every click until a key uses it, it times out, or you cancel it (for example with an Escape key wired to `stackable_oneshot_cancel()`, below). Holding the key past the tapping term instead works like a normal modifier with any mouse.

## Install

Add this repo to your [external userspace](https://docs.qmk.fm/newbs_external_userspace):

```sh
git submodule add https://github.com/wheredoesyourmindgo/qmk-modules.git modules/wheredoesyourmindgo
git submodule update --init --recursive
```

Then list the module in your keymap's `keymap.json`:

```json
{
    "modules": ["wheredoesyourmindgo/stackable_oneshot"]
}
```

## Keycodes

| Keycode                  | Alias     | Modifier    |
|--------------------------|-----------|-------------|
| `STACKABLE_ONESHOT_LCTL` | `SOSM_LC` | Left Ctrl   |
| `STACKABLE_ONESHOT_LSFT` | `SOSM_LS` | Left Shift  |
| `STACKABLE_ONESHOT_LALT` | `SOSM_LA` | Left Alt    |
| `STACKABLE_ONESHOT_LGUI` | `SOSM_LG` | Left GUI    |
| `STACKABLE_ONESHOT_RCTL` | `SOSM_RC` | Right Ctrl  |
| `STACKABLE_ONESHOT_RSFT` | `SOSM_RS` | Right Shift |
| `STACKABLE_ONESHOT_RALT` | `SOSM_RA` | Right Alt   |
| `STACKABLE_ONESHOT_RGUI` | `SOSM_RG` | Right GUI   |

Tapping several of them stacks the modifiers, e.g. tap Ctrl then Shift, then press a key.

## Options

In your `config.h`:

```c
#define STACKABLE_ONESHOT_TAPPING_TERM 300  // ms; hold longer than this to use it as a normal modifier
#define STACKABLE_ONESHOT_TIMEOUT 3000      // ms; 0 disables the idle timeout
```

QMK's own `ONESHOT_TIMEOUT` also applies to these one-shots if you set it. Whichever timeout is shorter wins.

## Cancelling from your own code

`stackable_oneshot_cancel()` clears pending one-shot mods and returns `true` if there were any. For example, to make Escape cancel a one-shot before doing anything else:

```c
#include "stackable_oneshot.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_ESC && record->event.pressed && stackable_oneshot_cancel()) {
        return false;
    }
    return true;
}
```

## Tests

The tests use QMK's test framework, so they run against a QMK checkout (found with `qmk env QMK_FIRMWARE`, or set `QMK_FIRMWARE`). From this repo:

```sh
scripts/test.sh stackable_oneshot
```

They cover tapping, chorded and rolled stacks, holds, the idle timeout, QMK's `ONESHOT_TIMEOUT`, cancelling, and clicks with mouse keys and a pointing device using one-shots up (including drags, and quick modifier-clicks while the key is held).

## License

GPL-2.0-or-later
