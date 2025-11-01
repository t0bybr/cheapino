# Repository Guidelines

## Project Structure & Module Organization
- `keyboards/cheapinov2/` – Board-specific code and config.
  - `config.h`, `rules.mk`, `halconf.h`, `mcuconf.h` – hardware/features.
  - `keymaps/toby/` – main keymap in C (`keymap.c`, `config.h`, `rules.mk`).
- `README.md` – features, flashing, and usage.
- `qmk.json` – build target metadata.
- Build output (`*.uf2`) may appear here and in `../qmk_firmware/`.
- In the sibling repo `../qmk_firmware/` a symlink points back to this userspace.

## Build, Test, and Development Commands
- Configure overlay (once): `qmk config user.overlay_dir=/qmk_userspace`
- Compile: `cd ../qmk_firmware && qmk compile -kb cheapinov2 -km toby`
- Flash (RP2040 UF2): enter bootloader (`QK_BOOT` or reset) and copy `cheapinov2_toby.uf2` to `RPI-RP2`.
- Container scripts (optional): `../build.sh`, `../flash.sh` (podman-based).

## Coding Style & Naming Conventions
- Language: C. Follow QMK style; keep diffs minimal and focused.
- Use descriptive names; prefer `static` for internal linkage; snake_case filenames.
- RGB rules on RP2040:
  - Never call `rgblight_*` from `matrix_scan_user()`.
  - Use event-driven hooks (`layer_state_set_user`, `leader_*_user`) and `defer_exec()`.
  - Use `_noeeprom` variants; keep brightness reasonable (`RGBLIGHT_LIMIT_VAL 50`).
  - Byte order is `WS2812_BYTE_ORDER_GRB`; DI pin `GP16`.

## Testing Guidelines
- No unit tests; validate via compile + hardware.
- Ensure clean build with no new warnings.
- Sanity-check: layer toggles, Tri-Layer (ENT_SYM + BSP_NUM → FKEY), Leader (DEL), LED overlays (Leader=weiß, HRM=random).

## Commit & Pull Request Guidelines
- Commit messages: `cheapinov2: <scope>: <concise summary>` (component prefix + action).
- PRs: describe intent, behavioral changes, and risks; link related issues; add short test notes.
- Do not mix unrelated refactors with feature/bugfix changes.

## Security & Configuration Tips (QMK/RP2040)
- Avoid `OS_DETECTION_DEBUG_ENABLE` on RP2040 (EEPROM size issues).
- Keep `DEFERRED_EXEC_ENABLE = yes`, `RGBLIGHT_ENABLE = yes`, `WS2812_DRIVER = vendor`.
- For matrix/encoder changes, prefer small, reviewable steps; the custom matrix is performance-sensitive.

