# Repository Guidelines

## Project Structure & Module Organization
- `keyboards/cheapinov2/` – board code.
  - `keymaps/toby/keymap.c` – layers, BSPC state‑machine, LEDs, app‑switcher.
  - `keymaps/toby/config.h` – mouse/LED/timeouts (kinetic settings).
  - `keymaps/toby/leader_map.h` – Leader Map (single source of truth).
  - `keymaps/toby/leader_actions.c` – map dispatcher.
  - `keymaps/toby/rules.mk` – features + extra sources.

## Build, Test, and Development
- Overlay: `qmk config user.overlay_dir=$(pwd)`
- Build: `cd ../qmk_firmware && qmk compile -kb cheapinov2 -km toby`
- Flash: copy UF2 to `RPI-RP2` (RP2040). Force‑NKRO deprecation warning is harmless.

## Coding Style & Conventions
- C, close to QMK style; keep patches small and focused.
- LEDs: only `_noeeprom` + `defer_exec()`; no updates in `matrix_scan_user()`.
- Use constants: `LED_BRIGHTNESS`, `LED_BRIGHTNESS_HOMEROW`, `MOUSEKEY_*`, `BSPC_TRIPLE_*`.
- OS‑aware behavior lives in code paths that check `detected_host_os()`.

## Testing Guidelines
- Ensure clean compile, then test on hardware.
- Sanity checks: layer colors, Leader Map, app‑switcher, BSPC (tap/hold/triple‑hold), word‑delete (macOS/Linux/Windows), mouse ramp + ACL momentary.

## Commit & PR Guidelines
- Commit style: `cheapinov2/toby: <scope>: <short description>`
- PRs: purpose, behavior, risks, short test notes. Avoid mixed feature/refactor commits.

## Agent‑Specific Instructions
- Maintain Leader only in `leader_map.h` (Linux/macOS/Windows entries).
- BSPC timing: `BSPC_TRIPLE_TERM_MS` (window) and `BSPC_TRIPLE_HOLD_MS` (hold gate).
- Kinetic tuning in `config.h` (e.g., `MOUSEKEY_TIME_TO_MAX=4`).
- ACL0/1/2 are momentary (QMK default) for distinct speed stages.
