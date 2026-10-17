# Instant One-Shot

One-shot modifiers that press the modifier the moment you press the key.

- **Tap** (under 300 ms): the modifier applies to the next key, then releases.
- **Hold** (300 ms or longer): it acts as a normal modifier and leaves nothing armed when you let go.
- **Idle timeout**: a one-shot you don't use clears itself after 3 seconds.

## How it differs from QMK's `OSM()`

`OSM()` waits to find out whether you tapped or held before acting. Instant One-Shot presses the modifier right away, so:

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
    "modules": ["wheredoesyourmindgo/instant_oneshot"]
}
```

## Keycodes

| Keycode                | Alias     | Modifier    |
|------------------------|-----------|-------------|
| `INSTANT_ONESHOT_LCTL` | `IO_LCTL` | Left Ctrl   |
| `INSTANT_ONESHOT_LSFT` | `IO_LSFT` | Left Shift  |
| `INSTANT_ONESHOT_LALT` | `IO_LALT` | Left Alt    |
| `INSTANT_ONESHOT_LGUI` | `IO_LGUI` | Left GUI    |
| `INSTANT_ONESHOT_RCTL` | `IO_RCTL` | Right Ctrl  |
| `INSTANT_ONESHOT_RSFT` | `IO_RSFT` | Right Shift |
| `INSTANT_ONESHOT_RALT` | `IO_RALT` | Right Alt   |
| `INSTANT_ONESHOT_RGUI` | `IO_RGUI` | Right GUI   |

Tapping several of them stacks the modifiers, e.g. tap Ctrl then Shift, then press a key.

## Options

In your `config.h`:

```c
#define INSTANT_ONESHOT_TAPPING_TERM 300  // ms; hold longer than this to use it as a normal modifier
#define INSTANT_ONESHOT_TIMEOUT 3000      // ms; 0 disables the idle timeout
```

## Cancelling from your own code

`instant_oneshot_cancel()` clears pending one-shot mods and returns `true` if there were any. For example, to make Escape cancel a one-shot before doing anything else:

```c
#include "instant_oneshot.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_ESC && record->event.pressed && instant_oneshot_cancel()) {
        return false;
    }
    return true;
}
```

## License

GPL-2.0-or-later
