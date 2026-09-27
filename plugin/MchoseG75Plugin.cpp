/*---------------------------------------------------------*\
| MchoseG75Plugin.cpp                                       |
|                                                           |
|   Standalone OpenRGB Device Plugin for MCHOSE G75         |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "MchoseG75Plugin.h"
#include "MchoseG75Controller.h"
#include "LogManager.h"
#include <QLabel>
#include <QVBoxLayout>
#include <set>

#define MATRIX_ROWS 6
#define MATRIX_COLS 16

static const unsigned int g75_matrix_map[MATRIX_ROWS][MATRIX_COLS] =
{
    /* Esc   F1    F2    F3    F4    F5    F6    F7    F8    F9   F10   F11   F12  PrtSc  Del  Knob */
    {   0,    6,   12,   18,   24,   30,   36,   42,   48,   54,   60,   66,   72,   78,   90,  102 },
    /*  ~     1     2     3     4     5     6     7     8     9     0     -     =   Bksp  Home  PgUp */
    {   1,    7,   13,   19,   25,   31,   37,   43,   49,   55,   61,   67,   73,   79,   96,  108 },
    /* Tab    Q     W     E     R     T     Y     U     I     O     P     [     ]     \    End  PgDn */
    {   2,    8,   14,   20,   26,   32,   38,   44,   50,   56,   62,   68,   74,   80,   91,   97 },
    /* Caps   A     S     D     F     G     H     J     K     L     ;     '    Enter                  */
    {   3,    9,   15,   21,   27,   33,   39,   45,   51,   57,   63,   69,   81,   81,  103,  109 },
    /* LShift Z     X     C     V     B     N     M     ,     .     /   RShift        Up                */
    {   4,   10,   16,   22,   28,   34,   40,   46,   52,   58,   64,   82,   82,   88,  104,  110 },
    /* LCtrl LWin LAlt             Space             RAlt  Fn   RCtrl        Left  Down Right       */
    {   5,   11,   17,   35,   35,   35,   35,   53,   59,   65,   83,   89,   95,   95,  105,  111 }
};

static const char* g75_led_names[MCHOSE_G75_LEDS_COUNT] =
{
    /* Col 0 */ "Key: Escape", "Key: `~", "Key: Tab", "Key: Caps Lock", "Key: Left Shift", "Key: Left Control",
    /* Col 1 */ "Key: F1", "Key: 1", "Key: Q", "Key: A", "Key: Z", "Key: Left Windows",
    /* Col 2 */ "Key: F2", "Key: 2", "Key: W", "Key: S", "Key: X", "Key: Left Alt",
    /* Col 3 */ "Key: F3", "Key: 3", "Key: E", "Key: D", "Key: C", "Key: LED 23",
    /* Col 4 */ "Key: F4", "Key: 4", "Key: R", "Key: F", "Key: V", "Key: LED 29",
    /* Col 5 */ "Key: F5", "Key: 5", "Key: T", "Key: G", "Key: B", "Key: Space",
    /* Col 6 */ "Key: F6", "Key: 6", "Key: Y", "Key: H", "Key: N", "Key: LED 41",
    /* Col 7 */ "Key: F7", "Key: 7", "Key: U", "Key: J", "Key: M", "Key: LED 47",
    /* Col 8 */ "Key: F8", "Key: 8", "Key: I", "Key: K", "Key: ,<", "Key: Right Alt",
    /* Col 9 */ "Key: F9", "Key: 9", "Key: O", "Key: L", "Key: .>", "Key: Function",
    /* Col 10 */ "Key: F10", "Key: 0", "Key: P", "Key: ;:", "Key: /?", "Key: Right Control",
    /* Col 11 */ "Key: F11", "Key: -_", "Key: [{", "Key: '\"", "Key: LED 70", "Key: LED 71",
    /* Col 12 */ "Key: F12", "Key: =+", "Key: ]}", "Key: LED 75", "Key: LED 76", "Key: LED 77",
    /* Col 13 */ "Key: Print Screen", "Key: Backspace", "Key: \\|", "Key: Enter", "Key: Right Shift", "Key: LED 83",
    /* Col 14 */ "Key: LED 84", "Key: LED 85", "Key: LED 86", "Key: LED 87", "Key: Up Arrow", "Key: Left Arrow",
    /* Col 15 */ "Key: Delete", "Key: End", "Key: LED 92", "Key: LED 93", "Key: LED 94", "Key: Down Arrow",
    /* Col 16 */ "Key: LED 96", "Key: Home", "Key: LED 98", "Key: LED 99", "Key: LED 100", "Key: LED 101",
    /* Col 17 */ "Key: Knob Click", "Key: LED 103", "Key: Page Up", "Key: LED 105", "Key: LED 106", "Key: Right Arrow",
    /* Col 18 */ "Key: LED 108", "Key: LED 109", "Key: LED 110", "Key: Page Down", "Key: LED 112", "Key: LED 113",
    /* Col 19 */ "Key: LED 114", "Key: LED 115", "Key: LED 116", "Key: LED 117", "Key: LED 118", "Key: LED 119",
    /* Col 20 */ "Key: LED 120", "Key: LED 121", "Key: LED 122", "Key: LED 123", "Key: LED 124", "Key: LED 125"
};

struct MchoseG75DeviceContext
{
    MchoseG75Controller*    ctl            = nullptr;
    RGBControllerInterface* v_ctl          = nullptr;
    std::string             path;
    int                     mode           = 0;
};

static void MchoseG75_DeviceUpdateLEDs(void* obj)
{
    MchoseG75DeviceContext* ctx = static_cast<MchoseG75DeviceContext*>(obj);
    if(!ctx || !ctx->ctl || !ctx->v_ctl)
    {
        return;
    }

    RGBColor* colors_ptr = ctx->v_ctl->GetColorsPointer();
    unsigned int led_count = ctx->v_ctl->GetLEDCount();
    if(colors_ptr && led_count > 0)
    {
        std::vector<RGBColor> colors(colors_ptr, colors_ptr + led_count);
        ctx->ctl->SetLEDsDirect(colors);
    }
}

static void MchoseG75_DeviceUpdateZoneLEDs(void* obj, int /*zone*/)
{
    MchoseG75_DeviceUpdateLEDs(obj);
}

static void MchoseG75_DeviceUpdateSingleLED(void* obj, int /*led*/)
{
    MchoseG75_DeviceUpdateLEDs(obj);
}

static void MchoseG75_DeviceUpdateMode(void* obj)
{
    MchoseG75DeviceContext* ctx = static_cast<MchoseG75DeviceContext*>(obj);
    if(!ctx || !ctx->ctl || !ctx->v_ctl)
    {
        return;
    }

    int active_mode = ctx->v_ctl->GetActiveMode();
    if(active_mode == 1) // Static mode
    {
        RGBColor static_color = ctx->v_ctl->GetModeColor(active_mode, 0);
        ctx->v_ctl->SetAllColors(static_color);
    }
    MchoseG75_DeviceUpdateLEDs(obj);
}

static void MchoseG75_DeviceSaveMode(void* /*obj*/)
{
}

MchoseG75Plugin::MchoseG75Plugin()
{
    api    = nullptr;
    widget = nullptr;
}

MchoseG75Plugin::~MchoseG75Plugin()
{
    Unload();
}

OpenRGBPluginInfo MchoseG75Plugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name            = "MCHOSE G75 Plugin";
    info.Description     = "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard (Wired & 2.4G Wireless)";
    info.Version         = "1.0.0";
    info.Commit          = "1.0.0";
    info.URL             = "https://github.com/Qyzom/MchooseG75-OpenRGB";
    info.Location        = OPENRGB_PLUGIN_LOCATION_SETTINGS;
    info.Label           = "MCHOSE G75";
    info.ProtocolVersion = 0;

    return info;
}

unsigned int MchoseG75Plugin::GetPluginAPIVersion()
{
    return OPENRGB_PLUGIN_API_VERSION;
}

void MchoseG75Plugin::RegisterKeyboard(MchoseG75Controller* ctl, const std::string& dev_path, int mode)
{
    MchoseG75DeviceContext* ctx = new MchoseG75DeviceContext();
    ctx->ctl  = ctl;
    ctx->path = dev_path;
    ctx->mode = mode;

    RGBController_Setup setup;
    setup.description   = "MCHOSE G75 Mechanical Keyboard";
    setup.location      = ctl->GetLocation();
    setup.name          = ctl->GetName();
    setup.serial        = ctl->GetSerialString();
    setup.vendor        = "MCHOSE";
    setup.version       = "1.0.0";
    setup.configuration = "";

    setup.active_mode   = 0;
    setup.flags         = 0;
    setup.type          = DEVICE_TYPE_KEYBOARD;

    class mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    setup.modes.push_back(Direct);

    class mode Static;
    Static.name       = "Static";
    Static.value      = 1;
    Static.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.colors_min = 1;
    Static.colors_max = 1;
    Static.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Static.colors.resize(1);
    setup.modes.push_back(Static);

    zone keyboard_zone;
    keyboard_zone.name                   = "Keyboard";
    keyboard_zone.type                   = ZONE_TYPE_MATRIX;
    keyboard_zone.leds_min               = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.leds_max               = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.leds_count             = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.matrix_map.height      = MATRIX_ROWS;
    keyboard_zone.matrix_map.width       = MATRIX_COLS;
    keyboard_zone.matrix_map.map.resize(MATRIX_ROWS * MATRIX_COLS);

    for(unsigned int r = 0; r < MATRIX_ROWS; ++r)
    {
        for(unsigned int c = 0; c < MATRIX_COLS; ++c)
        {
            keyboard_zone.matrix_map.map[r * MATRIX_COLS + c] = g75_matrix_map[r][c];
        }
    }
    setup.zones.push_back(keyboard_zone);

    for(unsigned int i = 0; i < MCHOSE_G75_LEDS_COUNT; ++i)
    {
        led new_led;
        if(i < sizeof(g75_led_names) / sizeof(g75_led_names[0]) && g75_led_names[i])
        {
            new_led.name = g75_led_names[i];
        }
        else
        {
            new_led.name = "Key " + std::to_string(i);
        }
        new_led.value = i;
        setup.leds.push_back(new_led);
    }

    setup.object_ptr                                      = (void*)ctx;
    setup.DeviceConfigureZone                             = nullptr;
    setup.DeviceUpdateLEDs                                = MchoseG75_DeviceUpdateLEDs;
    setup.DeviceUpdateZoneLEDs                            = MchoseG75_DeviceUpdateZoneLEDs;
    setup.DeviceUpdateSingleLED                           = MchoseG75_DeviceUpdateSingleLED;
    setup.DeviceUpdateMode                                = MchoseG75_DeviceUpdateMode;
    setup.DeviceSaveMode                                  = MchoseG75_DeviceSaveMode;
    setup.DeviceUpdateZoneMode                            = nullptr;
    setup.DeviceUpdateDeviceSpecificConfiguration         = nullptr;
    setup.DeviceUpdateDeviceSpecificZoneConfiguration     = nullptr;

    RGBControllerInterface* v_ctl = api->CreateVirtualRGBController(&setup);
    if(v_ctl)
    {
        ctx->v_ctl = v_ctl;
        api->RegisterVirtualRGBController(v_ctl);
        active_devices.push_back(ctx);

        if(api)
        {
            api->LogEntry(__FILE__, __LINE__, LL_INFO, "[MchoseG75Plugin] Registered %s (%s)", ctl->GetName().c_str(), dev_path.c_str());
        }
    }
    else
    {
        if(api)
        {
            api->LogEntry(__FILE__, __LINE__, LL_ERROR, "[MchoseG75Plugin] Failed to create Virtual RGBController for %s", dev_path.c_str());
        }
        delete ctx->ctl;
        delete ctx;
    }
}

void MchoseG75Plugin::ScanDevices()
{
    if(!api)
    {
        return;
    }

    std::set<std::string> scanned_paths_this_run;

    // Scan for Wired MCHOSE G75 (VID=0x258A, PID=0x010C)
    struct hid_device_info* devs = hid_enumerate(MCHOSE_G75_WIRED_VID, MCHOSE_G75_WIRED_PID);
    struct hid_device_info* cur_dev = devs;

    while(cur_dev)
    {
        std::string dev_path = cur_dev->path ? cur_dev->path : "";

        bool is_match = false;
#ifdef _WIN32
        if(cur_dev->interface_number == 1 &&
           (cur_dev->usage_page == 0xFF00 ||
            dev_path.find("col06") != std::string::npos ||
            dev_path.find("Col06") != std::string::npos))
        {
            is_match = true;
        }
#else
        if(cur_dev->interface_number == 1)
        {
            is_match = true;
        }
#endif

        if(is_match && !dev_path.empty() && scanned_paths_this_run.find(dev_path) == scanned_paths_this_run.end())
        {
            scanned_paths_this_run.insert(dev_path);

            bool already_active = false;
            for(MchoseG75DeviceContext* ctx : active_devices)
            {
                if(ctx && ctx->path == dev_path)
                {
                    already_active = true;
                    break;
                }
            }

            if(!already_active)
            {
                hid_device* dev = hid_open_path(cur_dev->path);
                if(dev)
                {
                    MchoseG75Controller* ctl = new MchoseG75Controller(dev, cur_dev->path, MCHOSE_G75_MODE_WIRED);
                    RegisterKeyboard(ctl, dev_path, MCHOSE_G75_MODE_WIRED);
                }
                else
                {
                    api->LogEntry(__FILE__, __LINE__, LL_WARNING, "[MchoseG75Plugin] Failed to open wired HID device: %s", dev_path.c_str());
                }
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    // Scan for 2.4G Wireless MCHOSE G75 (VID=0x41E4, PID=0x2001)
    devs = hid_enumerate(MCHOSE_G75_WIRELESS_VID, MCHOSE_G75_WIRELESS_PID);
    cur_dev = devs;

    while(cur_dev)
    {
        std::string dev_path = cur_dev->path ? cur_dev->path : "";

        bool is_match = false;
#ifdef _WIN32
        if(cur_dev->interface_number == 1 &&
           (cur_dev->usage_page == 0xFF02 ||
            dev_path.find("col") != std::string::npos ||
            dev_path.find("Col") != std::string::npos))
        {
            is_match = true;
        }
#else
        if(cur_dev->interface_number == 1)
        {
            is_match = true;
        }
#endif

        if(is_match && !dev_path.empty() && scanned_paths_this_run.find(dev_path) == scanned_paths_this_run.end())
        {
            scanned_paths_this_run.insert(dev_path);

            bool already_active = false;
            for(MchoseG75DeviceContext* ctx : active_devices)
            {
                if(ctx && ctx->path == dev_path)
                {
                    already_active = true;
                    break;
                }
            }

            if(!already_active)
            {
                hid_device* dev = hid_open_path(cur_dev->path);
                if(dev)
                {
                    MchoseG75Controller* ctl = new MchoseG75Controller(dev, cur_dev->path, MCHOSE_G75_MODE_WIRELESS);
                    RegisterKeyboard(ctl, dev_path, MCHOSE_G75_MODE_WIRELESS);
                }
                else
                {
                    api->LogEntry(__FILE__, __LINE__, LL_WARNING, "[MchoseG75Plugin] Failed to open wireless HID device: %s", dev_path.c_str());
                }
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}

void MchoseG75Plugin::Load(OpenRGBPluginAPIInterface* plugin_api_ptr)
{
    api = plugin_api_ptr;

    if(!api)
    {
        return;
    }

    api->LogEntry(__FILE__, __LINE__, LL_INFO, "[MchoseG75Plugin] Loading MCHOSE G75 Plugin...");
    ScanDevices();
}

void MchoseG75Plugin::ResourceManagerUpdated(unsigned int reason)
{
    if(reason == RESOURCEMANAGER_UPDATE_REASON_DETECTION_COMPLETE)
    {
        ScanDevices();
    }
}

QWidget* MchoseG75Plugin::GetWidget()
{
    if(!widget)
    {
        widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(widget);
        QLabel* title = new QLabel("<h2>MCHOSE G75 Plugin</h2>", widget);
        QLabel* desc  = new QLabel("Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard.<br>Keyboards are automatically detected and registered in OpenRGB.", widget);
        title->setAlignment(Qt::AlignCenter);
        desc->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);
        layout->addWidget(desc);
        layout->addStretch();
    }
    return widget;
}

QMenu* MchoseG75Plugin::GetTrayMenu()
{
    return nullptr;
}

void MchoseG75Plugin::Unload()
{
    for(MchoseG75DeviceContext* ctx : active_devices)
    {
        if(ctx)
        {
            if(api && ctx->v_ctl)
            {
                api->UnregisterVirtualRGBController(ctx->v_ctl);
                api->DeleteVirtualRGBController(ctx->v_ctl);
            }
            if(ctx->ctl)
            {
                delete ctx->ctl;
            }
            delete ctx;
        }
    }
    active_devices.clear();

    if(widget)
    {
        delete widget;
        widget = nullptr;
    }
}
