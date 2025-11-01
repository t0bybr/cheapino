Cheapino v2 – Toby Keymap
==========================

This is my userspace + keymap for Cheapino v2 (RP2040) on QMK. It aims for OS‑consistent behavior, fast editing/navigation, and non‑blocking visual feedback.

Highlights
- OS detection + boot LED: Windows (BSOD blue), macOS (white), Linux (Ubuntu purple).
- Layer LEDs: base off; distinct colors; overlays for Leader/HRM that don’t block.
- App‑Switcher on NAV layer: Toggle / Tab / Previous, OS‑aware (Linux mod configurable).
- Declarative Leader Map: edit one file (`leader_map.h`) for all sequences (Linux/macOS/Windows actions side‑by‑side).
- Backspace flow: LT(_NUM, KC_BSPC) with Triple‑Tap+Hold → BSPC repeat. Clean separation—no timing traps.
- Mouse (Kinetic): very fast ramp, moderate top speed; readable wheel; ACL keys act momentary to adjust speed factors.

Project Layout
- `keyboards/cheapinov2/` – board code
  - `keymaps/toby/keymap.c` – layers, BSPC state machine, LEDs, app‑switcher
  - `keymaps/toby/config.h` – keymap config (mouse, LEDs, timeouts)
  - `keymaps/toby/leader_actions.c/.h` – Leader dispatcher
  - `keymaps/toby/leader_map.h` – single source of Leader sequences (declarative)
  - `keymaps/toby/rules.mk` – features + extra sources

Build & Flash
1) Configure overlay once from this repo root:
   `qmk config user.overlay_dir=$(pwd)`
2) Build in firmware repo:
   `cd ../qmk_firmware && qmk compile -kb cheapinov2 -km toby`
3) Flash (RP2040 UF2): enter bootloader (QK_BOOT/reset) and copy `cheapinov2_toby.uf2` to `RPI-RP2`.

Layers (short)
- BASE (Colemak) – HRMs on A/R/S/T and N/E/I/O; thumbs are LT keys.
- NAV – arrows/navigation; App‑Switcher on right thumbs (Toggle/Tab/Prev).
- MOUSE – cursor + wheel; BTN1: double‑click (T), drag‑toggle (S); ACL0/1/2 = momentary accel stages.
- SYM_R / NUM / FKEY – symbols, numbers; tri‑layer: SYM_R+NUM → FKEY.
- MEDIA / EXTRA – media controls; spare utilities.

Leader Map (edit `leader_map.h`)
- Fully declarative, OS‑aware:
  - `LEADER2(KC_S, KC_P, KC_LGUI, LGUI(KC_SPC), KC_LGUI)` → App launcher (Linux/Spotlight/Windows)
  - `LEADER2(KC_W, KC_C, LALT(KC_F4), LGUI(KC_W), LALT(KC_F4))` → Close window
- Add comments beside entries to document intent. The map is the single source; no per‑OS macros in `config.h` anymore.

Backspace (BSPC)
- BSPC is `LT(_NUM, KC_BSPC)`:
  - Single tap → delete 1 character
  - Hold → momentary NUM (for digits)
  - Triple‑Tap + Hold → BSPC auto‑repeat until release
- Shift + BSPC → Delete (held while pressed)
- Word delete (OS‑aware):
  - Backwards: Option+Backspace (macOS) / Ctrl+Backspace (Linux/Windows)
  - Forwards:  Option+Delete (macOS) / Ctrl+Delete (Linux/Windows)

Mouse
- Kinetic (Inertia) mode tuned for crisp start:
  - `MOUSEKEY_TIME_TO_MAX=6`, `MOUSEKEY_MAX_SPEED=24`, `MOUSEKEY_MOVE_DELTA=10`, `MOUSEKEY_FRICTION=6`
- Wheel: lower speed and slower ramp → comfortable reading while holding.
- ACL keys (MS_ACL0/1/2) are momentary (QMK default): press to adjust acceleration stage on the fly.

LEDs
- Base off; layers use distinct colors; overlays only on events (non‑blocking).
- Leader overlay: white at `LED_BRIGHTNESS` while waiting.
- HRM overlay: random hue at `LED_BRIGHTNESS_HOMEROW` for held HRM.
- Boot LED by OS detection: Windows blue / macOS white / Linux purple.

App‑Switcher
- NAV right thumbs: Toggle / Tab / Previous.
- Linux modifier configurable via `LINUX_APP_SWITCH_MOD` in `keymaps/toby/config.h` (Super by default; change to Alt if your WM uses Alt+Tab).

Notes
- ACL: Friction is compile‑time in QMK; momentary stages still give clear speed differences via acceleration factors.
- OS LED flash waits for detection, then restores layer color via deferred callback.
- RGB: only event‑driven `_noeeprom` calls; avoid LED work in scan loops.

Contributing
- See `AGENTS.md` for contributor guidelines, structure, conventions.

