# Developer Architecture & AI Agent Guidelines: MCHOSE G75 OpenRGB

This document details the reverse-engineered USB HID lighting control architecture, memory organization, protocol mechanics, and code structure for the **MCHOSE G75 / G75 Pro** mechanical keyboard OpenRGB driver module.

---

## 1. Technical Protocol Mechanics

The MCHOSE G75 keyboard utilizes a **Planar RGB** framebuffer architecture across all 126 LED locations. Unlike standard RGB keyboards that pack color data in interleaved order (`R G B R G B...`), the G75 MCU arranges framebuffer memory in three contiguous plane arrays of 126 bytes each:

- **Red Plane (`0x00`..`0x7D`):** 126 bytes (1 byte per LED).
- **Green Plane (`0x7E`..`0xFB`):** 126 bytes (1 byte per LED).
- **Blue Plane (`0xFC`..`0x179`):** 126 bytes (1 byte per LED).
- **Total Color Frame Payload:** 378 bytes.

---

## 2. Device Connection Modes & Transfer Protocols

The driver implements dual-mode protocol resolution depending on the connection transport interface:

### A. Wired USB Type-C Mode
- **USB Vendor ID (VID):** `0x258A` (SinoWealth / BY Tech)
- **USB Product ID (PID):** `0x010C`
- **Target Interface:** Interface 1, Collection 6 (`Col06`, UsagePage `0xFF00`, Usage `0x0001`).
- **Transfer Method:** Single-shot HID Feature Report (`SET_REPORT`, Control Endpoint `0x00`).
- **Packet Length:** Exactly **520 bytes**.
- **Buffer Structure:**
  - `Header` (Bytes 0..8): `06 06 00 00 01 00 80 01 00`
  - `Red Plane` (Bytes 9..134): 126 bytes
  - `Green Plane` (Bytes 135..260): 126 bytes
  - `Blue Plane` (Bytes 261..386): 126 bytes
  - `Padding` (Bytes 387..519): 133 Zeros (`0x00`)
- **Latency / Performance:** 0ms inter-chunk delay. Sent as a single atomic USB transfer.

### B. Wireless 2.4G Receiver Mode
- **USB Vendor ID (VID):** `0x41E4`
- **USB Product ID (PID):** `0x2001`
- **Target Interface:** Interface 1, Collection 1 (`Col01`, UsagePage `0xFF02`, Usage `0x0002`).
- **Transfer Method:** Streamed HID Output Reports (`dev.write()`).
- **Packet Length:** **20 bytes** per chunk (27 chunks per frame).
- **Chunk Packet Structure:**
  - `Byte 0`: `0x13` (Report ID 19)
  - `Byte 1`: `0x02` (Matrix Category)
  - `Byte 2`: `0x1B` (Color Stream Opcode)
  - `Byte 3`: `Chunk Index` (`0x00` .. `0x1A`)
  - `Byte 4`: `0x0E` (Payload Length: 14 bytes)
  - `Bytes 5..18`: 14 bytes of Planar RGB slice
  - `Byte 19`: 8-bit Checksum (`sum(bytes[0..18]) & 0xFF`)
- **Timing Requirement:** Must include a 5ms delay (`std::this_thread::sleep_for(5ms)`) between chunk writes to prevent USB FIFO overflow in the MCU.

---

## 3. Matrix Mapping Architecture

The keyboard uses a 6-row by 16-column matrix topology:
- **Key Matrix Map (6x16):**
  - Row 0: `Esc (0)`, `F1 (6)`, `F2 (12)`, `F3 (18)`, `F4 (24)`, `F5 (30)`, `F6 (36)`, `F7 (42)`, `F8 (48)`, `F9 (54)`, `F10 (60)`, `F11 (66)`, `F12 (72)`, `Del (78)`
  - Row 1: `~ (1)`, `1 (7)`, `2 (13)`, `3 (19)`, `4 (25)`, `5 (31)`, `6 (37)`, `7 (43)`, `8 (49)`, `9 (55)`, `0 (61)`, `- (67)`, `= (73)`, `Bksp (79)`, `Home (96)`, `PgUp (108)`
  - Row 2: `Tab (2)`, `Q (8)`, `W (14)`, `E (20)`, `R (26)`, `T (32)`, `Y (38)`, `U (44)`, `I (50)`, `O (56)`, `P (62)`, `[ (68)`, `] (74)`, `\ (80)`, `End (91)`, `PgDn (97)`
  - Row 3: `Caps (3)`, `A (9)`, `S (15)`, `D (21)`, `F (27)`, `G (33)`, `H (39)`, `J (45)`, `K (51)`, `L (57)`, `; (63)`, `' (69)`, `Enter (81)`
  - Row 4: `LShift (4)`, `Z (10)`, `X (16)`, `C (22)`, `V (28)`, `B (34)`, `N (40)`, `M (46)`, `, (52)`, `. (58)`, `/ (64)`, `RShift (82)`, `Up (88)`
  - Row 5: `LCtrl (5)`, `LWin (11)`, `LAlt (17)`, `Space (35)`, `RAlt (53)`, `Fn (59)`, `RCtrl (65)`, `Left (83)`, `Down (89)`, `Right (95)`

---

## 4. Codebase Structure

```
MchooseG75-OpenRGB/
├── .github/
│   └── workflows/
│       └── build.yml               # Automated CI validation pipeline
├── openrgb/                        # Native OpenRGB C++ controller module
│   ├── MchoseG75Controller.h       # HID transport driver header
│   ├── MchoseG75Controller.cpp     # Wired & wireless protocol implementation
│   ├── RGBController_MchoseG75.h   # OpenRGB controller header
│   ├── RGBController_MchoseG75.cpp # OpenRGB matrix, zone, and LED bindings
│   ├── MchoseG75Detect.cpp         # OpenRGB HID detector registration
│   └── README.md                   # Integration guide for OpenRGB
├── mchose_g75_protocol.md          # Full protocol reverse engineering specification
├── AGENTS.md                       # Developer architecture documentation
├── README.md                       # Main repository documentation
├── LICENSE                         # MIT License
└── .gitignore                      # Git ignore file
```

---

## 5. GitHub Release Formatting Standard

All GitHub releases across the repository **must strictly adhere to the following unified English format**. Never include filler phrases, extraneous prose, or unlinked changelogs.

### Release Body Template

```markdown
### What's New
- Concise bulleted summary of new features, optimizations, and hardware behavior fixes.
- Second key change.
- Third key change.

[Full Changelog](https://github.com/Qyzom/MchooseG75-OpenRGB/commits/main)

### Release Assets
- **`OpenRGBMchoseG75Plugin.dll`** — Single-file OpenRGB plugin for Windows 10/11 x64 (Load via Settings -> Plugins -> Add Plugin).
- **`OpenRGBMchoseG75Plugin.so`** — Single-file OpenRGB plugin for Linux x86_64 (Load via Settings -> Plugins -> Add Plugin).
```

### Release Rules & Workflow:
1. **Language:** Always 100% in **English**.
2. **"What's New" Section:** Keep it concise and bulleted (3–7 bullets maximum). Focus on tangible user-facing value and low-level improvements.
3. **Changelog Link:** The link between the sections must strictly be `[Full Changelog](https://github.com/Qyzom/MchooseG75-OpenRGB/commits/main)`. Do not add trailing or leading filler sentences.
4. **Asset Descriptions:** Every non-source-code binary or archive asset attached to the release must be explicitly described with its target OS and deployment method.
5. **Publishing Process:**
   - Create Git tag: `git tag -a v1.0.0 -m "MchooseG75-OpenRGB 1.0.0"` and push: `git push origin main --tags`.
   - Create GitHub release via GitHub CLI:
     ```bash
     gh release create v1.0.0 \
       openrgb_mchose_g75.tar.gz \
       --title "MchooseG75-OpenRGB 1.0.0" \
       --notes-file release_notes.md
     ```
