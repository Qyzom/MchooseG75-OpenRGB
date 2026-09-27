/*---------------------------------------------------------*\
| MchoseG75Detect.cpp                                       |
|                                                           |
|   Detector for MCHOSE G75 / G75 Pro Mechanical Keyboard   |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "Detector.h"
#include "MchoseG75Controller.h"
#include "RGBController_MchoseG75.h"


void DetectMchoseG75Wired(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        MchoseG75Controller*     controller     = new MchoseG75Controller(dev, info->path, MCHOSE_G75_MODE_WIRED);
        RGBController_MchoseG75* rgb_controller = new RGBController_MchoseG75(controller);
        rgb_controller->name                    = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

void DetectMchoseG75Wireless(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        MchoseG75Controller*     controller     = new MchoseG75Controller(dev, info->path, MCHOSE_G75_MODE_WIRELESS);
        RGBController_MchoseG75* rgb_controller = new RGBController_MchoseG75(controller);
        rgb_controller->name                    = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

// Register Wired detector (Interface 1, UsagePage 0xFF00)
REGISTER_HID_DETECTOR_IP("MCHOSE G75 (Wired)", DetectMchoseG75Wired, MCHOSE_G75_WIRED_VID, MCHOSE_G75_WIRED_PID, 1, 0xFF00);

// Register Wireless detector (Interface 1, UsagePage 0xFF02)
REGISTER_HID_DETECTOR_IP("MCHOSE G75 (Wireless 2.4G)", DetectMchoseG75Wireless, MCHOSE_G75_WIRELESS_VID, MCHOSE_G75_WIRELESS_PID, 1, 0xFF02);
