/*---------------------------------------------------------*\
| RGBController_MchoseG75.cpp                               |
|                                                           |
|   OpenRGB Driver for MCHOSE G75 / G75 Pro Keyboard        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "RGBController_MchoseG75.h"

// 6 rows x 16 columns matrix layout matching MCHOSE G75 75% matrix
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
    Static.colors[0]      = ToRGBColor(255, 0, 0);
    Static.brightness_min = 0;
    Static.brightness_max = 100;
    Static.brightness     = 100;
    modes.push_back(Static);

    SetupZones();
}

RGBController_MchoseG75::~RGBController_MchoseG75()
{
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

    zones.push_back(keyboard_zone);

    for(unsigned int i = 0; i < MCHOSE_G75_LEDS_COUNT; ++i)
    {
        led new_led;
        new_led.name  = "Key " + std::to_string(i);
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
    if(modes[active_mode].name == "Static" && modes[active_mode].colors.size() > 0)
    {
        RGBColor static_color = modes[active_mode].colors[0];
        for(size_t i = 0; i < colors.size(); ++i)
        {
            colors[i] = static_color;
        }
    }
    DeviceUpdateLEDs();
}
