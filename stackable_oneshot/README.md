# Stackable One-Shot

One-shot modifiers you can stack reliably. Chord any number of them (Ctrl+Alt+GUI+Shift, all at once or one after another) and every one applies to the next key, every time.

- **Tap** (under 300 ms): the modifier applies to the next key, then releases.
- **Hold** (300 ms or longer): it acts as a normal modifier and leaves nothing armed when you let go.
- **Idle timeout**: a one-shot you don't use clears itself after 3 seconds.

## How it differs from QMK's `OSM()`

`OSM()` keys wait to find out whether you tapped or held before arming. When you press several together, each press interrupts the one before it, so QMK treats the earlier keys as holds, and those modifiers are gone once you let go. Chording one-shot mods with `OSM()` drops some of them.

Stackable One-Shot presses the modifier and arms the one-shot the moment each key goes down, and each key times its own tap or hold. No key waits on another, so:

- Stacked one-shots always all apply.
- Holding it has no delay, and modifier-clicks with a mouse (Cmd+click, Shift+click) work while the key is held.
- A tapped one-shot also applies to the next mouse click, not just the next key.
- The timeout clears the modifier from the host on time. QMK's `ONESHOT_TIMEOUT` is only checked on the next key event, so until then the host still sees the modifier as held.

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

## License

GPL-2.0-or-later
