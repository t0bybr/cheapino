# Cheapino Keyboard Firmware

Dieses Repository enthält meine angepasste QMK-Firmware für das Cheapino v2 Keyboard.

## Was ist neu?

### 🎯 Moderne QMK Features statt Vial
- **Achordion**: Intelligente Home Row Mods, die ungewollte Aktivierungen verhindern
- **Repeat Key**: Endlich Backspace/Delete mit Auto-Repeat!
- **Optimierte Tapping-Konfiguration**: Per-Key Tapping Terms für perfektes Feeling
- **Colemak Layout**: Mit Home Row Mods auf A, R, S, T (links) und N, E, I, O (rechts)

### 📂 Repository-Struktur

```
cheapino/
├── cheapino                    # Vial-Config (Backup der alten Config)
├── firmware/
│   └── keymaps/
│       └── custom/            # Custom QMK Keymap
│           ├── keymap.c       # Hauptkeymap mit allen Layern
│           ├── config.h       # Feature-Konfiguration
│           ├── rules.mk       # Build-Regeln
│           ├── features/
│           │   ├── achordion.c
│           │   └── achordion.h
│           └── README.md      # Detaillierte Dokumentation
└── README.md                  # Diese Datei
```

## 🚀 Schnellstart - Vorkompilierte Firmware

**Keine Lust auf lokales Setup?** Die Firmware wird automatisch über GitHub Actions kompiliert!

### Download der fertigen Firmware:

1. Gehe zu **[Actions](../../actions)** in diesem Repository
2. Klicke auf den neuesten erfolgreichen Workflow-Run (grüner Haken ✅)
3. Scrolle nach unten zu **"Artifacts"**
4. Lade **`cheapino-firmware`** herunter (enthält die `.uf2` Datei)
5. Entpacke das ZIP und flashe die `.uf2` Datei auf dein Cheapino:
   - BOOT-Taste am RP2040 drücken und halten
   - USB einstecken
   - `.uf2` Datei auf das erschienene USB-Laufwerk ziehen
   - Fertig! 🎉

## 🛠️ Lokales Setup (Fortgeschritten)

Falls du die Firmware selbst anpassen und kompilieren möchtest:

### 1. QMK Setup (Windows)
```bash
# QMK MSYS installieren von https://msys.qmk.fm/
# Dann:
qmk setup

# Cheapino Fork hinzufügen
cd ~/qmk_firmware
git remote add tompi https://github.com/tompi/qmk_firmware
git fetch tompi cheapinov2
git checkout tompi/cheapinov2
```

### 2. Keymap kopieren
```bash
# In QMK MSYS:
cd ~/qmk_firmware/keyboards/cheapino/keymaps/

# Dieses Repo klonen oder die Files manuell kopieren:
# firmware/keymaps/custom/* → keyboards/cheapino/keymaps/custom/
```

### 3. Kompilieren & Flashen
```bash
# Kompilieren
qmk compile -kb cheapino -km custom

# Cheapino in Bootloader-Modus versetzen:
# - BOOT-Taste am RP2040-Zero drücken und halten
# - USB einstecken
# - BOOT-Taste loslassen

# Flashen
qmk flash -kb cheapino -km custom
```

## Features im Detail

### Achordion - Bessere Home Row Mods
Verhindert unbeabsichtigte Mod-Aktivierungen:
- ✅ Nur bei gegenüberliegenden Händen
- ✅ Eager Mods für Shortcuts (Ctrl+C, Ctrl+V, etc.)
- ✅ Anpassbares Timeout (Standard: 1000ms)

### Repeat Key für Backspace/Delete
**So funktioniert's:**
1. Backspace halten → NUM Layer aktiv
2. Kurz nochmal tippen → QK_REP aktiviert
3. Backspace wiederholt sich automatisch!

Gleiches gilt für Delete auf dem FKEY Layer.

### 7 Layer
1. **BASE**: Colemak mit Home Row Mods
2. **MEDIA**: Mediensteuerung
3. **NAV**: Navigation (Pfeile, Home, End, etc.)
4. **MOUSE**: Maussteuerung
5. **SYM**: Symbole
6. **NUM**: Zahlen
7. **FKEY**: Funktionstasten

## Anpassungen

Alle Anpassungen sind in `firmware/keymaps/custom/` dokumentiert.

Siehe **[firmware/keymaps/custom/README.md](firmware/keymaps/custom/README.md)** für:
- Detaillierte Feature-Beschreibungen
- Anpassungsoptionen (Tapping Term, Achordion, etc.)
- Troubleshooting
- Layout-Diagramme

## Von Vial zu QMK gewechselt

Dieses Setup basiert auf meiner alten Vial-Konfiguration (siehe `cheapino` Datei), aber nutzt jetzt native QMK-Features für:
- Bessere Home Row Mods (Achordion)
- Neueste QMK-Features (Repeat Key, etc.)
- Mehr Kontrolle über das Verhalten
- Bessere Performance

## Credits

- **Cheapino Keyboard**: [tompi/cheapino](https://github.com/tompi/cheapino)
- **Achordion**: [getreuer.info](https://getreuer.info/posts/keyboards/achordion)
- **QMK Firmware**: [qmk.fm](https://qmk.fm/)

## Lizenz

- Achordion: Copyright 2022-2023 Google LLC, Apache-2.0
- Rest: QMK Firmware Lizenz (GPL)
