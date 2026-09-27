/*---------------------------------------------------------*\
| RGBController_MchoseG75.h                                 |
|                                                           |
|   OpenRGB Driver for MCHOSE G75 / G75 Pro Keyboard        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "MchoseG75Controller.h"

class RGBController_MchoseG75 : public RGBController
{
public:
    RGBController_MchoseG75(MchoseG75Controller* controller_ptr);
    ~RGBController_MchoseG75();

    void SetupZones();
    void ResizeZone(int zone, int new_size);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);

    void DeviceUpdateMode();

private:
    MchoseG75Controller* controller;
};
