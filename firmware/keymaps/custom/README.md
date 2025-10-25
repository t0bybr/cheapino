# Custom Cheapino Keymap

Eine moderne QMK-Firmware für das Cheapino Keyboard mit erweiterten Features für Home Row Mods.

## Features

### 🎯 Achordion - Intelligente Home Row Mods
[Achordion](https://getreuer.info/posts/keyboards/achordion) verhindert unbeabsichtigte Mod-Aktivierungen beim schnellen Tippen:
- **Bilateral Combinations**: Mods werden nur aktiviert bei Tastenkombinationen mit beiden Händen
- **Eager Mods**: Sofortige Aktivierung für häufige Shortcuts (Ctrl+C, Ctrl+V, etc.)
- **Timeout-basiert**: Automatisches Zurücksetzen nach 1 Sekunde

### 🔁 Repeat Key für Backspace/Delete
Endlich kannst du Backspace und Delete wiederholen lassen!

**So funktioniert's:**
1. **Backspace normal**: Einfach tippen
2. **Backspace halten**: NUM Layer aktivieren
3. **Nochmal tippen** (während gehalten): QK_REP = Backspace wiederholt sich

Gleiches für Delete auf dem FKEY Layer!

### ⌨️ Home Row Mods - Colemak Layout
```
Linke Hand:  A=GUI | R=ALT | S=CTRL | T=SHIFT
Rechte Hand: N=SHIFT | E=CTRL | I=ALT | O=GUI
```

### 🎚️ 7 Layer
1. **BASE**: Colemak mit Home Row Mods
2. **MEDIA**: Mediensteuerung, Lautstärke
3. **NAV**: Navigation (Pfeile, Home, End, Page Up/Down)
4. **MOUSE**: Maussteuerung und Scrolling
5. **SYM_R**: Symbole (rechte Hand)
6. **NUM**: Zahlen
7. **FKEY**: Funktionstasten F1-F12

## Setup & Installation

### Voraussetzungen
1. **QMK Firmware** installiert ([QMK Setup Guide](https://docs.qmk.fm/#/newbs_getting_started))
2. **Cheapino QMK Fork** von tompi

### Installation

#### 1. QMK einrichten (Windows mit QMK MSYS)
```bash
# QMK MSYS öffnen
qmk setup

# Cheapino Fork hinzufügen
cd ~/qmk_firmware
git remote add tompi https://github.com/tompi/qmk_firmware
git fetch tompi cheapinov2
git checkout tompi/cheapinov2
```

#### 2. Custom Keymap kopieren
```bash
# Keymap-Verzeichnis erstellen
mkdir -p keyboards/cheapino/keymaps/custom

# Diese Dateien in keyboards/cheapino/keymaps/custom/ kopieren:
# - keymap.c
# - config.h
# - rules.mk
# - features/achordion.c
# - features/achordion.h
```

#### 3. Firmware kompilieren
```bash
qmk compile -kb cheapino -km custom
```

#### 4. Firmware flashen
```bash
# Cheapino in DFU-Modus versetzen:
# - BOOT-Taste am RP2040-Zero drücken und halten
# - USB einstecken
# - BOOT-Taste loslassen

# Flashen
qmk flash -kb cheapino -km custom

# Datei wird nach .build/cheapino_custom.uf2 kompiliert
# Drag & Drop auf das USB-Laufwerk
```

## Keymap Details

### Base Layer (Colemak)
```
┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
│ Q │ W │ F │ P │ G │Tab│       │Ent│ J │ L │ U │ Y │ ' │
├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
│ A │ R │ S │ T │ D │Bsp│       │Bsp│ H │ N │ E │ I │ O │
│Gui│Alt│Ctl│Sft│   │Num│       │Num│   │Sft│Ctl│Alt│Gui│
├───┼───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┼───┤
│ Z │ X │ C │ V │ B │Del│       │Del│ K │ M │ , │ . │ / │
└───┴───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┴───┘
            ┌───┐                       ┌───┐
            │Ply│                       │Ply│
      ┌─────┼───┼─────┐           ┌─────┼───┼─────┐
      │ Esc │   │ Tab │           │ Ent │   │ Spc │
      │Media│   │Mouse│           │ Sym │   │ Nav │
      └─────┴───┴─────┘           └─────┴───┴─────┘
```

### Daumentasten mit Layer-Tap
- **Esc/Media**: Tap = Escape, Hold = Media Layer
- **Space/Nav**: Tap = Space, Hold = Navigation Layer
- **Tab/Mouse**: Tap = Tab, Hold = Mouse Layer
- **Enter/Sym**: Tap = Enter, Hold = Symbols Layer
- **Backspace/Num**: Tap = Backspace, Hold = Numbers Layer (mit QK_REP!)
- **Delete/Fkey**: Tap = Delete, Hold = F-Keys Layer (mit QK_REP!)

## Konfiguration anpassen

### Tapping Term ändern
In `config.h`:
```c
#define TAPPING_TERM 200  // Standard: 200ms
```

Für einzelne Tasten in `keymap.c` → `get_tapping_term()`

### Achordion Timeout ändern
In `features/achordion.c`:
```c
#define ACHORDION_TIMEOUT 1000  // Standard: 1000ms
```

### Repeat Key Geschwindigkeit
Der Repeat Key (QK_REP) nutzt die System-Repeat-Rate. Für custom Repeat:
- Entkommentiere BSPC_REP/DEL_REP Code in keymap.c
- Passe REPEAT_INTERVAL an (aktuell 50ms)

## Tipps & Tricks

### Home Row Mods richtig nutzen
1. **Langsam am Anfang**: Achordion hilft, aber die Eingewöhnung braucht Zeit
2. **Bilateral Typing**: Versuche, Modifier mit der einen Hand, Key mit der anderen zu drücken
3. **Shortcuts**: Ctrl+C, Ctrl+V etc. werden durch Achordion sofort erkannt

### Backspace Repeat optimal nutzen
**Schnelles Löschen:**
1. Backspace halten → NUM Layer aktiv
2. Kurz nochmal antippen → Repeat startet
3. Jeden weiteren Tap = 1x Backspace (oder halten für continuous repeat via System)

**Alternative**: Nutze QK_AREP (Alternate Repeat) für zusätzliche Flexibilität

### Layer-Switching
- **Sticky Layers**: Nicht implementiert, aber einfach hinzuzufügen (OSL())
- **Tri-Layer**: Könnte für häufige Kombinationen nützlich sein

## Troubleshooting

### Firmware lässt sich nicht kompilieren
```bash
# Prüfe QMK Version
qmk --version  # Mindestens 0.20.0 für REPEAT_KEY

# Achordion-Dateien fehlen?
ls keyboards/cheapino/keymaps/custom/features/
```

### Home Row Mods triggern ungewollt
- Erhöhe TAPPING_TERM in config.h
- Passe achordion_chord() an für deine Tippgewohnheiten

### Repeat Key funktioniert nicht
- Prüfe QMK-Version (mindestens 0.20.0)
- Stelle sicher, dass REPEAT_KEY_ENABLE = yes in rules.mk

### Encoder funktioniert nicht
- Prüfe ENCODER_ENABLE in rules.mk
- Passe encoder_map in keymap.c an

## Weiterentwicklung

### Geplante Features
- [ ] Combos für häufige Zeichen (ä, ö, ü, ß)
- [ ] Caps Word für ALL CAPS Tippen
- [ ] Sticky Modifiers
- [ ] Macro-Unterstützung

### Weitere Infos
- [QMK Dokumentation](https://docs.qmk.fm/)
- [Achordion von getreuer](https://getreuer.info/posts/keyboards/achordion)
- [Cheapino Projekt](https://github.com/tompi/cheapino)
- [Precondition's Home Row Mods Guide](https://precondition.github.io/home-row-mods)

## Lizenz

- **Achordion**: Copyright 2022-2023 Google LLC, SPDX-License-Identifier: Apache-2.0
- **Rest**: QMK Firmware Lizenz (GPL)
