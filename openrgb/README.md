# MCHOSE G75 OpenRGB Native Controller Module

This module adds full native built-in controller support for the **MCHOSE G75 / G75 Pro** mechanical keyboard directly into upstream **OpenRGB**.

---

## Features

- **Wired Mode (USB Type-C):** `VID: 0x258A`, `PID: 0x010C`
- **Wireless Mode (2.4G Receiver):** `VID: 0x41E4`, `PID: 0x2001`
- **Full Per-Key Addressability:** Direct per-LED color control across all 126 planar RGB channels.
- **75% Compact Matrix:** Standard 6-row by 16-column physical matrix mapping with individual key labels.
- **Optimized Latency:** 520-byte single-shot HID Feature Reports in wired mode; 20-byte packet stream in 2.4G wireless mode.

---

## Source Files

- `MchoseG75Controller.h` / `MchoseG75Controller.cpp` — Low-level USB HID hardware driver and packet transport.
- `RGBController_MchoseG75.h` / `RGBController_MchoseG75.cpp` — OpenRGB controller bindings (zones, matrix, modes, and brightness).
- `MchoseG75Matrix.h` — Matrix topology map and human-readable key labels.
- `MchoseG75Detect.cpp` — OpenRGB auto-detection registration for wired and wireless interfaces.

---

## Building Inside OpenRGB (Linux / Windows)

### 1. Clone OpenRGB
```bash
git clone https://gitlab.com/CalcProgrammer1/OpenRGB.git
cd OpenRGB
```

### 2. Copy Controller Files
Copy the `openrgb` folder into OpenRGB's `Controllers` directory:
```bash
cp -r /path/to/MchooseG75-OpenRGB/openrgb Controllers/MchoseG75Controller
```

### 3. Add to `OpenRGB.pro`
Open `OpenRGB.pro` in a text editor and append the files to their respective sections:

**HEADERS:**
```qmake
HEADERS += \
    Controllers/MchoseG75Controller/MchoseG75Controller.h \
    Controllers/MchoseG75Controller/MchoseG75Matrix.h \
    Controllers/MchoseG75Controller/RGBController_MchoseG75.h
```

**SOURCES:**
```qmake
SOURCES += \
    Controllers/MchoseG75Controller/MchoseG75Controller.cpp \
    Controllers/MchoseG75Controller/RGBController_MchoseG75.cpp \
    Controllers/MchoseG75Controller/MchoseG75Detect.cpp
```

### 4. Build OpenRGB
On Linux:
```bash
qmake OpenRGB.pro
make -j$(nproc)
sudo make install
```

When you launch `openrgb`, the MCHOSE G75 keyboard will be automatically recognized and available under the **Devices** tab with full per-key RGB control!
