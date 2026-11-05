/*---------------------------------------------------------*\
| MchoseG75PluginCommon.cpp                                 |
|                                                           |
|   Shared definitions and helpers for MCHOSE G75 plugins   |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "MchoseG75PluginCommon.h"
#include "MchoseG75Matrix.h"
#include "LogManager.h"
#include <QLabel>
#include <QVBoxLayout>
#include <set>

void MchoseG75_DeviceUpdateLEDs(void* obj)
{
    MchoseG75DeviceContext* ctx = static_cast<MchoseG75DeviceContext*>(obj);
    if(!ctx || !ctx->ctl || !ctx->v_ctl)
    {
        return;
    }

    RGBColor* colors_ptr = ctx->v_ctl->GetColorsPointer();
    unsigned int led_count = ctx->v_ctl->GetLEDCount();
    if(!colors_ptr || led_count == 0)
    {
        return;
    }

    int active_mode = ctx->v_ctl->GetActiveMode();
    unsigned int brightness = ctx->v_ctl->GetModeBrightness(active_mode);
    if(brightness > 100)
    {
        brightness = 100;
    }

    std::vector<RGBColor> colors(led_count);
    RGBColor static_color = 0;
    if(active_mode == 1) // Static mode
    {
        static_color = ctx->v_ctl->GetModeColor(1, 0);
        ctx->v_ctl->SetAllColors(static_color);
    }

    for(size_t i = 0; i < led_count; ++i)
    {
        RGBColor c = (active_mode == 1) ? static_color : colors_ptr[i];
        if(brightness < 100)
        {
            unsigned char r = (RGBGetRValue(c) * brightness) / 100;
            unsigned char g = (RGBGetGValue(c) * brightness) / 100;
            unsigned char b = (RGBGetBValue(c) * brightness) / 100;
            colors[i] = ToRGBColor(r, g, b);
        }
        else
        {
            colors[i] = c;
        }
    }

    ctx->ctl->SetLEDsDirect(colors);
}

void MchoseG75_DeviceUpdateZoneLEDs(void* obj, int /*zone*/)
{
    MchoseG75_DeviceUpdateLEDs(obj);
}

void MchoseG75_DeviceUpdateSingleLED(void* obj, int /*led*/)
{
    MchoseG75_DeviceUpdateLEDs(obj);
}

void MchoseG75_DeviceUpdateMode(void* obj)
{
    MchoseG75_DeviceUpdateLEDs(obj);
}

void MchoseG75_DeviceSaveMode(void* /*obj*/)
{
}

void MchoseG75_RegisterKeyboard(OpenRGBPluginAPIInterface* api,
                               std::vector<MchoseG75DeviceContext*>& active_devices,
                               MchoseG75Controller* ctl,
                               const std::string& dev_path,
                               int mode)
{
    if(!api || !ctl)
    {
        return;
    }

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
    setup.version       = "1.0.3";
    setup.configuration = "";

    setup.active_mode   = 0;
    setup.flags         = 0;
    setup.type          = DEVICE_TYPE_KEYBOARD;

    class mode Direct;
    Direct.name           = "Direct";
    Direct.value          = 0;
    Direct.flags          = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Direct.color_mode     = MODE_COLORS_PER_LED;
    Direct.brightness_min = 0;
    Direct.brightness_max = 100;
    Direct.brightness     = 100;
    setup.modes.push_back(Direct);

    class mode Static;
    Static.name           = "Static";
    Static.value          = 1;
    Static.flags          = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.colors_min     = 1;
    Static.colors_max     = 1;
    Static.color_mode     = MODE_COLORS_MODE_SPECIFIC;
    Static.colors.resize(1);
    Static.colors[0]      = ToRGBColor(0, 255, 255);
    Static.brightness_min = 0;
    Static.brightness_max = 100;
    Static.brightness     = 100;
    setup.modes.push_back(Static);

    zone keyboard_zone;
    keyboard_zone.name                   = "Keyboard";
    keyboard_zone.type                   = ZONE_TYPE_MATRIX;
    keyboard_zone.leds_min               = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.leds_max               = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.leds_count             = MCHOSE_G75_LEDS_COUNT;
    keyboard_zone.matrix_map.height      = MCHOSE_G75_MATRIX_ROWS;
    keyboard_zone.matrix_map.width       = MCHOSE_G75_MATRIX_COLS;
    keyboard_zone.matrix_map.map.resize(MCHOSE_G75_MATRIX_ROWS * MCHOSE_G75_MATRIX_COLS);

    for(unsigned int r = 0; r < MCHOSE_G75_MATRIX_ROWS; ++r)
    {
        for(unsigned int c = 0; c < MCHOSE_G75_MATRIX_COLS; ++c)
        {
            keyboard_zone.matrix_map.map[r * MCHOSE_G75_MATRIX_COLS + c] = g75_matrix_map[r][c];
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

    setup.object_ptr                                      = static_cast<void*>(ctx);
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

        api->LogEntry(__FILE__, __LINE__, LL_INFO,
                      "[MCHOSE G75] Registered %s (%s)",
                      ctl->GetName().c_str(), dev_path.c_str());
    }
    else
    {
        api->LogEntry(__FILE__, __LINE__, LL_ERROR,
                      "[MCHOSE G75] Failed to create Virtual RGBController for %s",
                      dev_path.c_str());
        delete ctx->ctl;
        delete ctx;
    }
}

static void ScanDeviceType(OpenRGBPluginAPIInterface* api,
                           std::vector<MchoseG75DeviceContext*>& active_devices,
                           uint16_t vid,
                           uint16_t pid,
                           int mode)
{
    if(!api)
    {
        return;
    }

    std::set<std::string> scanned_paths_this_run;
    struct hid_device_info* devs = hid_enumerate(vid, pid);
    struct hid_device_info* cur_dev = devs;

    while(cur_dev)
    {
        std::string dev_path = cur_dev->path ? cur_dev->path : "";
        bool is_match = false;

#ifdef _WIN32
        if(cur_dev->interface_number == 1)
        {
            if(mode == MCHOSE_G75_MODE_WIRED &&
               (cur_dev->usage_page == 0xFF00 ||
                dev_path.find("col06") != std::string::npos ||
                dev_path.find("Col06") != std::string::npos))
            {
                is_match = true;
            }
            else if(mode == MCHOSE_G75_MODE_WIRELESS &&
                    (cur_dev->usage_page == 0xFF02 ||
                     dev_path.find("col01") != std::string::npos ||
                     dev_path.find("Col01") != std::string::npos))
            {
                is_match = true;
            }
        }
#else
        if(cur_dev->interface_number == 1)
        {
            if(mode == MCHOSE_G75_MODE_WIRED &&
               (cur_dev->usage_page == 0 || cur_dev->usage_page == 0xFF00))
            {
                is_match = true;
            }
            else if(mode == MCHOSE_G75_MODE_WIRELESS &&
                    (cur_dev->usage_page == 0 || cur_dev->usage_page == 0xFF02))
            {
                is_match = true;
            }
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
                    MchoseG75Controller* ctl = new MchoseG75Controller(dev, cur_dev->path, mode);
                    MchoseG75_RegisterKeyboard(api, active_devices, ctl, dev_path, mode);
                }
                else
                {
                    api->LogEntry(__FILE__, __LINE__, LL_WARNING,
                                  "[MCHOSE G75] Failed to open %s device: %s",
                                  (mode == MCHOSE_G75_MODE_WIRED ? "wired" : "wireless"),
                                  dev_path.c_str());
                }
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}

void MchoseG75_ScanWiredDevices(OpenRGBPluginAPIInterface* api,
                               std::vector<MchoseG75DeviceContext*>& active_devices)
{
    ScanDeviceType(api, active_devices, MCHOSE_G75_WIRED_VID, MCHOSE_G75_WIRED_PID, MCHOSE_G75_MODE_WIRED);
}

void MchoseG75_ScanWirelessDevices(OpenRGBPluginAPIInterface* api,
                                 std::vector<MchoseG75DeviceContext*>& active_devices)
{
    ScanDeviceType(api, active_devices, MCHOSE_G75_WIRELESS_VID, MCHOSE_G75_WIRELESS_PID, MCHOSE_G75_MODE_WIRELESS);
}

void MchoseG75_UnloadDevices(OpenRGBPluginAPIInterface* api,
                            std::vector<MchoseG75DeviceContext*>& active_devices)
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
}

QWidget* MchoseG75_CreatePluginWidget(const QString& title, const QString& description)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    QLabel* title_label = new QLabel(QString("<h2>%1</h2>").arg(title), widget);
    QLabel* desc_label  = new QLabel(description, widget);
    title_label->setAlignment(Qt::AlignCenter);
    desc_label->setAlignment(Qt::AlignCenter);
    layout->addWidget(title_label);
    layout->addWidget(desc_label);
    layout->addStretch();
    return widget;
}
