# QMK Community Modules

[QMK community modules](https://docs.qmk.fm/features/community_modules) by wheredoesyourmindgo.

| Module | What it does |
|---|---|
| [Stackable One-Shot](stackable_oneshot/) | One-shot mods you can stack reliably: chord any number and they all apply to the next key. Holds have no delay and mouse modifier-clicks work |

## Install

From your [external userspace](https://docs.qmk.fm/newbs_external_userspace):

```sh
git submodule add https://github.com/wheredoesyourmindgo/qmk-modules.git modules/wheredoesyourmindgo
git submodule update --init --recursive
```

Then add the modules you want to your keymap's `keymap.json`, e.g. `"modules": ["wheredoesyourmindgo/stackable_oneshot"]`.
