# MCHOSE G75 OpenRGB Native Controller

Этот модуль добавляет полную нативную поддержку клавиатуры **MCHOSE G75 / G75 Pro** в **OpenRGB**.

## Возможности
* Поддержка **проводного режима (USB Type-C)**: `VID: 0x258A`, `PID: 0x010C`
* Поддержка **беспроводного режима (2.4G радиодонгл)**: `VID: 0x41E4`, `PID: 0x2001`
* Прямое управление каждым отдельным светодиодом (Per-Key RGB / Direct Mode).
* Матрица клавиш 75% формата (6 строк x 16 столбцов).
* Максимальная скорость обновления (520 байт за один Feature Report по проводу, потоковые чанки по 2.4G).

---

## Файлы
* `MchoseG75Controller.h` / `MchoseG75Controller.cpp` — низкоуровневый HID драйвер устройства.
* `RGBController_MchoseG75.h` / `RGBController_MchoseG75.cpp` — контроллер OpenRGB (матрица, светодиоды, зоны).
* `MchoseG75Detect.cpp` — регистрация автоопределения устройства в OpenRGB.

---

## Сборка с OpenRGB (Linux / Windows)

### 1. Клонирование OpenRGB
```bash
git clone https://gitlab.com/CalcProgrammer1/OpenRGB.git
cd OpenRGB
```

### 2. Добавление файлов
Скопируйте папку `openrgb_mchose_g75` в дерево исходников OpenRGB:
```bash
cp -r /path/to/openrgb_mchose_g75 Controllers/MchoseG75Controller
```

### 3. Добавление в файл проекта `OpenRGB.pro`
Откройте `OpenRGB.pro` и добавьте в соответствующие секции:

**HEADERS:**
```qmake
HEADERS += \
    Controllers/MchoseG75Controller/MchoseG75Controller.h \
    Controllers/MchoseG75Controller/RGBController_MchoseG75.h
```

**SOURCES:**
```qmake
SOURCES += \
    Controllers/MchoseG75Controller/MchoseG75Controller.cpp \
    Controllers/MchoseG75Controller/RGBController_MchoseG75.cpp \
    Controllers/MchoseG75Controller/MchoseG75Detect.cpp
```

### 4. Компиляция на Linux
```bash
qmake OpenRGB.pro
make -j$(nproc)
sudo make install
```

После запуска `openrgb` клавиатура автоматически появится в списке устройств со всеми 75% клавишами и полным управлением цветом!
