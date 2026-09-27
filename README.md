# MCHOSE G75 OpenRGB Plugin & Controller Module

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![OpenRGB: Supported](https://img.shields.io/badge/OpenRGB-Supported-success.svg)](https://openrgb.org)
[![Platform: Linux & Windows](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-brightgreen.svg)]()
[![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-orange.svg)]()

Native **OpenRGB** dynamic plugin and C++ controller module for the **MCHOSE G75 / G75 Pro** mechanical keyboard. Implements full per-key RGB control, 75% matrix mapping, and direct lighting updates over both **Wired (USB Type-C)** and **Wireless (2.4G USB Dongle)** connection modes.

---

### Overview

The official vendor utility (MCHOSE HUB) is Electron-based, heavy on resources, Windows-only, and lacks native Linux support. This repository provides a single-file dynamic plugin (`.dll` / `.so`) for OpenRGB, enabling seamless hardware lighting control across Linux and Windows without background CPU overhead or risk of EEPROM key-remap corruption.

---

### Key Features

1. **Dual-Mode Connection Support (Wired & 2.4G Wireless):**  
   Automatically detects and resolves transport protocols depending on connection mode:
   - **Wired USB Mode (`VID: 0x258A, PID: 0x010C`):** Transmits single-shot 520-byte HID Feature Reports via `Col06` for 0ms interaction latency.
   - **2.4G Wireless Mode (`VID: 0x41E4, PID: 0x2001`):** Transmits streamed 20-byte Output Reports (27 chunks) with 8-bit checksum calculation over `Col01`.

2. **Per-Key Planar RGB Engine:**  
   Full addressability over all 126 LED channels arranged in Planar RGB format (126 Red + 126 Green + 126 Blue = 378 bytes).

3. **100% Safe Operation (Zero Remap Risk):**  
   Interacts strictly with the volatile RAM framebuffer. Never sends key-remapping or EEPROM write opcodes.

4. **Native OpenRGB Integration:**  
   Integrates directly into OpenRGB's `ResourceManager`, supporting Per-Key RGB, Direct Mode, custom profiles, and synchronization with audio visualizers and effects plugins.

---

### Installation Guide

#### Option A: Single-File Plugin Installation (No Compilation Required)

To add **MCHOSE G75** to your existing OpenRGB installation:

1. Go to the [**Releases**](https://github.com/Qyzom/MchooseG75-OpenRGB/releases/latest) page.
2. Download the pre-compiled single-file plugin for your OS:
   - **Windows 10/11 (x64):** Download **`OpenRGBMchoseG75Plugin.dll`**
   - **Linux (x86_64 Qt 6 / OpenRGB 1.0):** Download **`OpenRGBMchoseG75Plugin.so`** (or `OpenRGBMchoseG75Plugin-Qt6.so`)
   - **Linux (x86_64 Qt 5):** Download **`OpenRGBMchoseG75Plugin-Qt5.so`**
3. Open **OpenRGB** (requires OpenRGB 1.0 or Pipeline build).
4. Go to **Settings → Plugins → Install Plugin** (or drag & drop the file into the plugins list).
   > **Linux Tip:** If the file chooser does not allow selecting the `.so` file, simply copy it directly to your OpenRGB plugins folder:
   > `mkdir -p ~/.config/OpenRGB/plugins && cp OpenRGBMchoseG75Plugin.so ~/.config/OpenRGB/plugins/`
   > (or `~/.var/app/org.openrgb.OpenRGB/config/OpenRGB/plugins/` for Flatpak), then restart OpenRGB.
5. The **MCHOSE G75 / G75 Pro** keyboard will automatically appear in the **Devices** tab!

---

### Hardware Protocol Specification (USB HID)

#### 1. Device Identifiers
- **Wired Connection (USB Type-C):** `VID: 0x258A` (9610), `PID: 0x010C` (268) — SinoWealth / BY Tech Controller.
- **Wireless Connection (2.4G Receiver):** `VID: 0x41E4` (16868), `PID: 0x2001` (8193) — 2.4G Receiver Controller.

#### 2. Framebuffer Structure (Planar RGB - 378 Bytes)
Color data is stored in contiguous planar arrays:
- `Red Plane`: 126 bytes (LEDs 0..125)
- `Green Plane`: 126 bytes (LEDs 0..125)
- `Blue Plane`: 126 bytes (LEDs 0..125)

#### 3. Wired Protocol Packet Layout (520 Bytes - Feature Report `0x06`)
- `Byte 0`: `0x06` (Report ID)
- `Byte 1`: `0x06` (Lighting Opcode)
- `Bytes 2..3`: `0x00, 0x00`
- `Byte 4`: `0x01` (Direct Matrix Mode)
- `Byte 5`: `0x00`
- `Bytes 6..7`: `0x80, 0x01` (Data Size: `0x0180` = 384 bytes)
- `Byte 8`: `0x00`
- `Bytes 9..134`: Red Plane (126 bytes)
- `Bytes 135..260`: Green Plane (126 bytes)
- `Bytes 261..386`: Blue Plane (126 bytes)
- `Bytes 387..519`: Zero Padding (`0x00`)

#### 4. Wireless Protocol Packet Layout (20 Bytes - Output Report `0x13`)
- `Byte 0`: `0x13` (Report ID)
- `Byte 1`: `0x02` (Matrix Category)
- `Byte 2`: `0x1B` (Color Stream Subcommand)
- `Byte 3`: `Chunk Index` (`0x00 .. 0x1A`, 27 chunks total)
- `Byte 4`: `0x0E` (Chunk Length: 14 bytes)
- `Bytes 5..18`: 14 bytes of Planar RGB slice
- `Byte 19`: Checksum (`sum(bytes[0..18]) & 0xFF`)

---

### Matrix Layout Map (75% Compact - 6 Rows x 16 Cols)

| Row \ Col | Col 0 | Col 1 | Col 2 | Col 3 | Col 4 | Col 5 | Col 6 | Col 7 | Col 8 | Col 9 | Col 10 | Col 11 | Col 12 | Col 13 | Col 14 | Col 15 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Row 0** | Esc (`0`) | F1 (`6`) | F2 (`12`) | F3 (`18`) | F4 (`24`) | F5 (`30`) | F6 (`36`) | F7 (`42`) | F8 (`48`) | F9 (`54`) | F10 (`60`) | F11 (`66`) | F12 (`72`) | Del (`78`) | — | — |
| **Row 1** | `~` (`1`) | 1 (`7`) | 2 (`13`) | 3 (`19`) | 4 (`25`) | 5 (`31`) | 6 (`37`) | 7 (`43`) | 8 (`49`) | 9 (`55`) | 0 (`61`) | - (`67`) | = (`73`) | Bksp (`79`) | Home (`96`) | PgUp (`108`) |
| **Row 2** | Tab (`2`) | Q (`8`) | W (`14`) | E (`20`) | R (`26`) | T (`32`) | Y (`38`) | U (`44`) | I (`50`) | O (`56`) | P (`62`) | [ (`68`) | ] (`74`) | `\` (`80`) | End (`91`) | PgDn (`97`) |
| **Row 3** | Caps (`3`) | A (`9`) | S (`15`) | D (`21`) | F (`27`) | G (`33`) | H (`39`) | J (`45`) | K (`51`) | L (`57`) | ; (`63`) | ' (`69`) | Enter (`81`) | — | — | — |
| **Row 4** | LShift (`4`) | Z (`10`) | X (`16`) | C (`22`) | V (`28`) | B (`34`) | N (`40`) | M (`46`) | , (`52`) | . (`58`) | / (`64`) | RShift (`82`) | Up (`88`) | — | — | — |
| **Row 5** | LCtrl (`5`) | LWin (`11`) | LAlt (`17`) | Space (`35`) | — | — | — | RAlt (`53`) | Fn (`59`) | RCtrl (`65`) | Left (`83`) | Down (`89`) | Right (`95`) | — | — | — |

---

### License

This project is licensed under the **[MIT License](LICENSE)**.  
OpenRGB controller source files adhere to the **GPL-2.0-or-later** license matching OpenRGB upstream requirements.
