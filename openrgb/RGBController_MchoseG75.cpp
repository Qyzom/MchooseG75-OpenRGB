/*---------------------------------------------------------*\
| RGBController_MchoseG75.cpp                               |
|                                                           |
|   OpenRGB Driver for MCHOSE G75 / G75 Pro Keyboard        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "RGBController_MchoseG75.h"
#include "MchoseG75Matrix.h"

RGBController_MchoseG75::RGBController_MchoseG75(MchoseG75Controller* controller_ptr)
{
    controller  = controller_ptr;

    name        = controller->GetName();
    vendor      = "MCHOSE";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "MCHOSE G75 Mechanical Keyboard";
    location    = controller->GetLocation();
    serial      = controller->GetSerialString();

    mode Direct;
    Direct.name           = "Direct";
    Direct.value          = 0;
    Direct.flags          = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Direct.color_mode     = MODE_COLORS_PER_LED;
    Direct.brightness_min = 0;
    Direct.brightness_max = 100;
    Direct.brightness     = 100;
    modes.push_back(Direct);

    mode Static;
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
    modes.push_back(Static);

    SetupZones();
}

RGBController_MchoseG75::~RGBController_MchoseG75()
{
    Shutdown();
    delete controller;
}

void RGBController_MchoseG75::SetupZones()
{
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

    zones.push_back(keyboard_zone);

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
        leds.push_back(new_led);
        colors.push_back(0);
    }

    SetupColors();
}

void RGBController_MchoseG75::ResizeZone(int /*zone*/, int /*new_size*/)
{
    // Fixed size 75% keyboard zone
}

void RGBController_MchoseG75::DeviceUpdateLEDs()
{
    if(active_mode < 0 || static_cast<size_t>(active_mode) >= modes.size())
    {
        active_mode = 0;
    }

    unsigned int brightness = modes[active_mode].brightness;
    if((modes[active_mode].flags & MODE_FLAG_HAS_BRIGHTNESS) && brightness < 100)
    {
        std::vector<RGBColor> scaled_colors(colors.size());
        for(size_t i = 0; i < colors.size(); ++i)
        {
            unsigned char r = (RGBGetRValue(colors[i]) * brightness) / 100;
            unsigned char g = (RGBGetGValue(colors[i]) * brightness) / 100;
            unsigned char b = (RGBGetBValue(colors[i]) * brightness) / 100;
            scaled_colors[i] = ToRGBColor(r, g, b);
        }
        controller->SetLEDsDirect(scaled_colors);
    }
    else
    {
        controller->SetLEDsDirect(colors);
    }
}

void RGBController_MchoseG75::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_MchoseG75::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_MchoseG75::DeviceUpdateMode()
{
    if(active_mode < 0 || static_cast<size_t>(active_mode) >= modes.size())
    {
        active_mode = 0;
    }

    if(modes[active_mode].name == "Static" && !modes[active_mode].colors.empty())
    {
        RGBColor static_color = modes[active_mode].colors[0];
        for(size_t i = 0; i < colors.size(); ++i)
        {
            colors[i] = static_color;
        }
    }
    DeviceUpdateLEDs();
}
