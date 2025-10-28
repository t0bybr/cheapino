# Cheapino v2 - Toby's Keymap Features & Dokumentation

**Stand:** 28. Oktober 2025 - **Version 2.4**
**QMK Version:** 0.30.6
**Keyboard:** Cheapino v2 (RP2040)
**Layout:** Colemak mit Home Row Mods

---

## 🎉 Version 2.2 - Hardware Features!

### ✅ Vollständig funktional:

1. **Chordal Hold** - Home Row Mods mit intelligenter Same-Hand/Opposite-Hand Detection
2. **OS Detection** - Automatische macOS/Linux Erkennung für OS-aware Shortcuts
3. **Tri-Layer** - ENT_SYM + BSP_NUM = FKEY Layer
4. **Combos (3x)** - OS-aware Copy/Paste/Cut
5. **Key Overrides** - Shift+Backspace = Delete, Shift+Esc = Tilde
6. **Leader Key** - DEL (Timeout 800ms, LED weiß)
7. **Caps Word** - Beide Shifts aktivieren
8. **Mouse Keys** - Combined Mode mit Acceleration (3 Geschwindigkeiten)
9. **Encoder** - Layer-abhängig (BASE=Scroll, MEDIA=Volume, NAV=Arrows)
10. **Repeat Key** - QK_REP auf NUM/FKEY Layer
11. **Ctrl+Backspace** - OS-aware Wort rückwärts löschen (Ctrl auf Linux, Option auf macOS)
12. **Ctrl+Delete** - OS-aware Wort vorwärts löschen (Ctrl auf Linux, Option auf macOS)
13. **Shift+Backspace** - Delete mit Autorepeat
14. **Quick Tap** - Schnelles Backspace-Repeat ohne Layer-Aktivierung
15. **LED Layer Indicator** - Nicht-blockierende Layerfarben
16. **OS Boot LED Flash** - Kurzer OS-Farbblitz beim Start

### ❌ Bewusst deaktiviert (verursachten Probleme):

- **Caps Word Combos (S+T, N+E)** - Beide Shifts funktionieren besser

---

## 🆕 Neueste Änderungen - Version 2.4 (28.10.2025)

### ✨ Änderungen:

1. **Tri-Layer umgestellt**
   - Jetzt: SYM_R + NUM = FKEY (Daumen: ENT_SYM + BSP_NUM)
   - Reihenfolge-unabhängig nutzbar

2. **Leader Key vereinfacht**
   - DEL ist jetzt direkt `QK_LEAD`
   - LED zeigt während Leader-Timeout Weiß (V=50)

3. **Tasten-Feinschliff**
   - `)` wieder auf SYM_R (linke mittlere Daumentaste)
   - `0` auf NUM (linke mittlere Daumentaste)

4. **LED-OS-Flash robuster**
   - Wartet auf OS-Erkennung, zeigt dann 800ms OS-Farbe (Linux/Windows=Grün, macOS=Magenta)

## 🆕 Neueste Änderungen - Version 2.3 (28.10.2025)

### ✨ Neue Features:

1. **LEDs reaktiviert (nicht-blockierend)**
   - Layerfarben per `layer_state_set_user()` (ereignisgetrieben)
   - OS-Farbblitz beim Boot via `defer_exec()`
   - Keine LED-Updates in `matrix_scan_user()` → kein Lockup mehr

2. **Doku-/Aufräumarbeiten**
   - Alte LED-Backup-Datei entfernt
   - VIA-Keymap-Altlast entfernt

## 🆕 Neueste Änderungen - Version 2.1 (28.10.2025)

### ✨ Neue Features:

1. **Automatischer Modifier-Swap auf macOS**
   - Ctrl ↔ Gui werden auf macOS automatisch getauscht
   - Bedeutung: Home Row Ctrl wird zu Command auf macOS
   - Home Row Gui wird zu Control auf macOS
   - Implementierung via `keymap_config.swap_lctl_lgui` in `matrix_scan_user()`
   - **Resultat:** Drücke Ctrl+C auf deinem Keyboard → sendet Cmd+C auf macOS, Ctrl+C auf Linux!

2. **Ctrl+Delete OS-aware**
   - Linux: Ctrl+Delete (löscht Wort vorwärts)
   - macOS: Option+Delete (löscht Wort vorwärts)
   - Symmetrisch zu Ctrl+Backspace für Wort-Löschen in beide Richtungen

### 📊 Statistik:

- **Combo Count:** 3 (Copy, Paste, Cut)
- **COMBO_TERM:** 100ms (zuverlässig)
- **Layer Count:** 8 (BASE, MEDIA, NAV, MOUSE, SYM_R, NUM, FKEY, EXTRA)
- **Features aktiv:** 16
- **Code Lines:** ~450

---

## 🔄 Changelog - Version 2.0 → 2.1

### Version 2.1 (28.10.2025 - macOS Update)
- ✅ **NEU:** Automatischer Ctrl/Gui Swap auf macOS
- ✅ **NEU:** Ctrl+Delete OS-aware (Wort vorwärts löschen)
- 🎯 macOS-Optimierung: Home Row Ctrl wirkt als Command

### Version 2.0 (28.10.2025)
1. **Tri-Layer funktioniert jetzt!**
   - Problem: Trigger-Keys fehlten auf NAV/SYM_R Layern
   - Lösung: ENT_SYM im NAV Layer, SPC_NAV im SYM_R Layer
   - Space + Enter = EXTRA Layer (z.B. für Bootloader-Access)

2. **OS Detection aktiv**
   - Problem: Debug-Modus verursachte EECONFIG_SIZE Error
   - Lösung: OS_DETECTION_DEBUG_ENABLE entfernt
   - Jetzt: Automatisches macOS/Linux Switching

3. **LED-Funktionen temporär entfernt** (historisch)
   - Problem (damals): `rgblight_*` in `matrix_scan_user()` blockierte Tastatur
   - Heutige Lösung (v2.3): Nur noch ereignisgetriebene Updates + `defer_exec()`

4. **Ctrl+Backspace OS-aware**
   - Linux: Ctrl+Backspace (löscht Wort rückwärts)
   - macOS: Option+Backspace (löscht Wort rückwärts)
   - Automatische Umschaltung via OS Detection

5. **Shift+Backspace mit Autorepeat**
   - Delete-Funktion mit register_code() statt tap_code()
   - Funktioniert beim Halten der Taste

6. **SPC_NAV Fix**
   - HOLD_ON_OTHER_KEY_PRESS für SPC_NAV deaktiviert
   - Kein versehentliches Layer-Aktivieren beim schnellen Tippen

---

## 📋 Feature-Übersicht

### ✅ Aktive Features

| Feature | Status | QMK Version | OS-Aware | Beschreibung |
|---------|--------|-------------|----------|--------------|
| **Chordal Hold** | ✅ Aktiv | 0.30+ | - | Same-hand/opposite-hand Detection für HRMs |
| **OS Detection** | ✅ Aktiv | Core | ✅ | Auto-Erkennung macOS/Linux |
| **Modifier Swap** | ✅ Aktiv | Core | ✅ | Ctrl↔Gui auf macOS automatisch |
| **Tri-Layer** | ✅ Aktiv | Core | - | ENT_SYM+BSP_NUM = FKEY Layer |
| **Combos** | ✅ Aktiv | Core | ✅ | Copy/Paste/Cut OS-aware |
| **Leader Key** | ✅ Aktiv | Core | ✅ | DEL startet Leader (LED weiß) |
| **Key Overrides** | ✅ Aktiv | 0.13+ | - | Shift+Esc=Tilde |
| **Caps Word** | ✅ Aktiv | 0.20+ | - | Beide Shifts |
| **Repeat Key** | ✅ Aktiv | 0.21+ | - | QK_REP für letzte Taste |
| **Mouse Keys** | ✅ Aktiv | Core | - | 3-Speed Maus |
| **Quick Tap** | ✅ Aktiv | Core | - | Schnelles Backspace-Repeat |
| **Shift+Backspace** | ✅ Aktiv | Custom | - | Delete mit Autorepeat |
| **Ctrl+Backspace** | ✅ Aktiv | Custom | ✅ | Wort rückwärts löschen (OS-aware) |
| **Ctrl+Delete** | ✅ Aktiv | Custom | ✅ | Wort vorwärts löschen (OS-aware) |
| **LED Layer Indicator** | ✅ Aktiv | Core | - | Nicht-blockierend, Layerfarben |
| **OS Boot LED Flash** | ✅ Aktiv | Core | ✅ | OS-Farbblitz beim Start |

### ❌ Deaktivierte Features

| Feature | Status | Grund | Alternative |
|---------|--------|-------|-------------|
| **Homerow Mod LED** | ✅ Aktiv | Core | - | Farb-Overlay bei Hold (V=50) |
| **Caps Word Combos** | ❌ Entfernt | Beide Shifts besser | BOTH_SHIFTS_TURNS_ON_CAPS_WORD |

---

## 🎯 Combos (OS-aware)

| Combo | Linux | macOS | Beschreibung |
|-------|-------|-------|--------------|
| `X + C` | Ctrl+C | Cmd+C | Copy |
| `C + V` | Ctrl+V | Cmd+V | Paste |
| `Z + X` | Ctrl+X | Cmd+X | Cut |
| (entfällt) |  |  | Leader per DEL |

**Config:**
- `COMBO_COUNT = 4`
- `COMBO_TERM = 100ms` (war 50ms, jetzt zuverlässiger)

---

## 🔑 Leader Key Sequences (OS-aware)

| Sequence | Linux | macOS | Beschreibung |
|----------|-------|-------|--------------|
| `DEL` → `S` → `P` | Super | Cmd+Space | App Launcher |
| `DEL` → `T` → `C` | Ctrl+C | Ctrl+C | Terminal Interrupt |

**Config:**
- `LEADER_TIMEOUT = 300ms`
- `LEADER_PER_KEY_TIMING` aktiviert

---

## 🗂️ Layer-Struktur

### Layer 0: BASE (Colemak)
```
┌─────┬─────┬─────┬─────┬─────┐   ┌─────┬─────┬─────┬─────┬─────┐
│  Q  │  W  │  F  │  P  │  G  │   │  J  │  L  │  U  │  Y  │  '  │
├─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┤
│ A/⊞ │ R/⎇ │ S/⌃ │ T/⇧ │  D  │   │  H  │ N/⇧ │ E/⌃ │ I/⎇ │ O/⊞ │
├─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┤
│  Z  │  X  │  C  │  V  │  B  │   │  K  │  M  │  ,  │  .  │  /  │
└─────┴─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┴─────┘
            │ Esc │ Spc │ Tab │   │ Ent │ Bsp │ Del │
            │ MED │ NAV │ MOU │   │ SYM │ NUM │ FKY │
            └─────┴─────┴─────┘   └─────┴─────┴─────┘
```

**Home Row Mods:**
- Links: A=GUI, R=AltGr, S=Ctrl, T=Shift
- Rechts: N=Shift, E=Ctrl, I=Alt, O=GUI

### Layer 2: NAV (Navigation)
```
Navigation auf rechter Hand, Modifier auf linker:
┌──────┬──────┬──────┬──────┬─────┐   ┌──────┬──────┬──────┬──────┬─────┐
│      │      │      │      │     │   │      │ PgUp │  ↑   │ PgDn │     │
├──────┼──────┼──────┼──────┼─────┤   ├──────┼──────┼──────┼──────┼─────┤
│  ⊞   │  ⎇   │  ⌃   │  ⇧   │     │   │ Home │  ←   │  ↓   │  →   │ End │
└──────┴──────┴──────┴──────┴─────┘   └──────┴──────┴──────┴──────┴─────┘
                                       Thumbs: ENT  │      │
```
**Wichtig:** ENT_SYM + BSP_NUM → FKEY (Tri-Layer)

### Layer 4: SYM_R (Symbols)
```
Symbole auf linker Hand:
┌──────┬──────┬──────┬──────┬──────┐
│  {   │  &   │  *   │  (   │  }   │
├──────┼──────┼──────┼──────┼──────┤
│  :   │  $   │  %   │  ^   │  +   │
├──────┼──────┼──────┼──────┼──────┤
│  ~   │  !   │  @   │  #   │  |   │
└──────┴──────┴──────┴──────┴──────┘
Thumbs:  (   │ SPC  │  _
```
**Wichtig:** ENT_SYM + BSP_NUM → FKEY (Tri-Layer)

### Layer 7: EXTRA (historisch)
Früher via NAV + SYM_R; aktuell ungenutzt. Boot via FKEY Layer (QK_BOOT).

---

## 🛠️ Konfiguration

### Tapping Term (Per-Key)

| Key Type | Tapping Term | Begründung |
|----------|--------------|------------|
| Home Row (Standard) | 230ms | Basis aus Keyboard Config |
| GUI Keys (A, O) | 260ms (+30ms) | Mehr Zeit für seltene Modifier |
| Thumb Keys | 280ms (+50ms) | Mehr Komfort für Layer-Wechsel |

### Quick Tap Term

| Key | Quick Tap | Begründung |
|-----|-----------|------------|
| Backspace (BSP_NUM) | 100ms | Schnelles Repeat ohne Layer |
| Andere Layer-Tap Keys | Disabled | Normal Hold-Behavior |

### Hold on Other Key Press

| Key | HOOKP | Begründung |
|-----|-------|------------|
| ESC_MED, TAB_MOU, ENT_SYM, BSP_NUM, DEL_FKY | ✅ Enabled | Schneller Layer-Zugriff |
| SPC_NAV | ❌ Disabled | Verhindert versehentliches Aktivieren beim schnellen Tippen |

---

## 🔧 Bekannte Probleme & Lösungen

### ✅ Gelöst: Tastatur blockiert

**Problem:**
- LED-Funktionen in `matrix_scan_user()` blockierten Tastatur
- `rgblight_*` Funktionen liefen zu oft (1000x/s)
- Tastatur reagierte nicht mehr auf Input

**Lösung (v2.3):**
- LED-Updates aus `matrix_scan_user()` entfernt
- Layerfarben in `layer_state_set_user()` (nur bei Zustandswechsel)
- OS-Farbblitz per `defer_exec()` (einmalig beim Boot)

### ✅ Gelöst: Tri-Layer funktionierte nicht

**Problem:**
- `update_tri_layer_state()` funktionierte nicht mit Layer-Tap Keys
- Trigger-Keys fehlten auf NAV/SYM_R Layern
- Konnte zweiten Layer nicht aktivieren während erster gehalten

**Lösung:**
- ENT_SYM im NAV Layer verfügbar gemacht
- SPC_NAV im SYM_R Layer verfügbar gemacht
- Jetzt: Space halten + Enter drücken = EXTRA Layer ✅

### ✅ Gelöst: OS Detection funktionierte nicht

**Problem:**
- `OS_DETECTION_DEBUG_ENABLE` verursachte EECONFIG_SIZE Error
- Kompilierung schlug fehl auf RP2040

**Lösung:**
- Debug-Modus in config.h entfernt
- EEPROM wear-leveling Config hinzugefügt (war nicht das Problem)
- OS Detection funktioniert jetzt perfekt ✅

### ✨ NEU: Automatischer Modifier-Swap auf macOS (v2.1)

**Ziel:**
- Home Row Ctrl soll auf macOS als Command funktionieren
- Ctrl+C auf Keyboard → sendet Cmd+C auf macOS, Ctrl+C auf Linux

**Implementierung:**
```c
void matrix_scan_user(void) {
    static bool swap_configured = false;
    static os_variant_t last_os = OS_UNSURE;

    os_variant_t current_os = detected_host_os();

    if (!swap_configured || last_os != current_os) {
        bool is_macos = (current_os == OS_MACOS || current_os == OS_IOS);

        // Swap Ctrl ↔ Gui on macOS
        keymap_config.swap_lctl_lgui = is_macos;
        keymap_config.swap_rctl_rgui = is_macos;

        swap_configured = true;
        last_os = current_os;
    }
}
```

**Resultat auf macOS:**
- Home Row `S` (labeled Ctrl) → sendet Command
- Home Row `E` (labeled Ctrl) → sendet Command
- Home Row `A` (labeled Gui) → sendet Control
- Home Row `O` (labeled Gui) → sendet Control
- `Ctrl+C` → `Cmd+C`
- `Ctrl+V` → `Cmd+V`
- `Ctrl+Backspace` → `Option+Backspace` (via zusätzliche OS-aware Handler)

**Resultat auf Linux:**
- Alles bleibt wie beschriftet
- `Ctrl+C` → `Ctrl+C`
- `Gui+Space` → `Super` (Launcher)

### ✅ Gelöst: Ctrl+Backspace bleibt aktiv

**Problem:**
- Nach Ctrl+Backspace blieb Ctrl aktiv
- Modifier State nicht korrekt wiederhergestellt

**Lösung:**
```c
// Vorher: unregister_mods(), register_code()
// Jetzt: clear_mods(), tap_code16(), set_mods()

clear_mods();
send_keyboard_report();
tap_code16(is_macos ? LALT(KC_BSPC) : LCTL(KC_BSPC));
set_mods(mods);  // Restore exact state
send_keyboard_report();
```

### ✅ Gelöst: NAV Layer beim schnellen Tippen

**Problem:**
- Beim schnellen Tippen "word " aktivierte sich NAV Layer
- Space zwischen Wörtern triggerte Layer-Hold

**Lösung:**
- `HOLD_ON_OTHER_KEY_PRESS` für SPC_NAV deaktiviert
- Layer aktiviert sich nur bei langem Hold (>280ms)
- Schnelles Tippen funktioniert jetzt fehlerfrei ✅

---

## 📦 Build & Flash

### Firmware kompilieren

```bash
cd /var/home/t0bybr/containers/qmk/qmk_firmware
qmk compile -kb cheapinov2 -km toby
```

**Output:**
- `qmk_firmware/cheapinov2_toby.uf2`
- `qmk_userspace/cheapinov2_toby.uf2`

### Firmware flashen

**Methode 1: Bootloader via Tri-Layer**
1. Enter (ENT_SYM) halten + Backspace (BSP_NUM) drücken = FKEY Layer
2. Q drücken (QK_BOOT)
3. RPI-RP2 Drive erscheint
4. `cheapinov2_toby.uf2` auf Drive kopieren

**Methode 2: Reset-Button**
1. Reset-Button auf Keyboard drücken
2. RPI-RP2 Drive erscheint
3. `cheapinov2_toby.uf2` auf Drive kopieren

### QMK Updates

```bash
cd qmk_firmware
git pull origin master
git submodule update --init --recursive
qmk compile -kb cheapinov2 -km toby
```

**Vorteil:** Userspace bleibt unberührt, nur Firmware-Updates

---

## 🎓 Gelernte Lektionen

### 1. Tri-Layer + Layer-Tap
**Problem:** Trigger-Keys müssen auf beiden Layern verfügbar sein
**Lösung:** Beide Keys in beiden Layern definieren

### 2. RGBLIGHT in matrix_scan_user()
**Problem:** Blockiert Tastatur bei zu häufigen Aufrufen
**Lösung:** Nur in layer_state_set_user() oder mit deferred_exec()

### 3. OS Detection Debug-Modus
**Problem:** EECONFIG_SIZE Error auf RP2040
**Lösung:** Debug-Modus entfernen, Standard-Modus funktioniert

### 4. register_code() vs tap_code()
**Problem:** register_code() ohne unregister_code() lässt Key aktiv
**Lösung:** Für Modifier-State: clear_mods() → tap_code16() → set_mods()

---

## 📚 Ressourcen

### Dokumentation
- [QMK Firmware Docs](https://docs.qmk.fm/)
- [Chordal Hold Blog](https://blog.zsa.io/chordal-hold/)
- [Pascal Getreuer's Guide](https://getreuer.info/posts/keyboards/)

### Code
- **Firmware:** `qmk_firmware/` (Official QMK)
- **Userspace:** `qmk_userspace/` (Custom)
- **Keymap:** `qmk_userspace/keyboards/cheapinov2/keymaps/toby/`

---

## 📝 Changelog

### Version 2.0 (28. Oktober 2025) - FINAL

**Neue Features:**
- ✅ OS Detection aktiv und funktioniert
- ✅ Tri-Layer aktiv und funktioniert (Space + Enter = EXTRA)
- ✅ Ctrl+Backspace OS-aware (Ctrl auf Linux, Option auf macOS)
- ✅ Shift+Backspace mit Autorepeat

**Fixes:**
- ✅ Tastatur-Lockup behoben (LED nicht-blockierend implementiert)
- ✅ Tri-Layer funktioniert mit Layer-Tap
- ✅ NAV Layer beim schnellen Tippen behoben
- ✅ Ctrl+Backspace Modifier-State behoben
- ✅ OS Detection EECONFIG Error behoben

**Entfernt:**
- ❌ Caps Word Combos (beide Shifts besser)

**Config-Änderungen:**
- COMBO_COUNT: 6 → 4
- COMBO_TERM: 50ms → 100ms
- TRI_LAYER_ENABLE: no → yes
- OS_DETECTION_ENABLE: no → yes
- SPC_NAV: HOOKP disabled

---

**Letzte Aktualisierung:** 28. Oktober 2025
**Version:** 2.1 - macOS Optimized
**Autor:** Toby
**Keyboard:** Cheapino v2 (RP2040)
**Status:** 🎉 Production Ready - Cross-Platform!
