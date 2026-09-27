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
#include "RGBController_MchoseG75.h"

MchoseG75Plugin::MchoseG75Plugin()
{
    api = nullptr;
}

MchoseG75Plugin::~MchoseG75Plugin()
{

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

void MchoseG75Plugin::Load(OpenRGBPluginAPIInterface* plugin_api_ptr)
{
    api = plugin_api_ptr;

    if(!api)
    {
        return;
    }

    // Scan for Wired MCHOSE G75 (VID=0x258A, PID=0x010C)
    struct hid_device_info* devs = hid_enumerate(MCHOSE_G75_WIRED_VID, MCHOSE_G75_WIRED_PID);
    struct hid_device_info* cur_dev = devs;

    while(cur_dev)
    {
        if(cur_dev->interface_number == 1 && std::string(cur_dev->path).find("Col06") != std::string::npos)
        {
            hid_device* dev = hid_open_path(cur_dev->path);
            if(dev)
            {
                MchoseG75Controller*     ctl     = new MchoseG75Controller(dev, cur_dev->path, MCHOSE_G75_MODE_WIRED);
                RGBController_MchoseG75* rgb_ctl = new RGBController_MchoseG75(ctl);
                api->RegisterVirtualRGBController(rgb_ctl);
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
        if(cur_dev->interface_number == 1 && cur_dev->usage_page == 0xFF02)
        {
            hid_device* dev = hid_open_path(cur_dev->path);
            if(dev)
            {
                MchoseG75Controller*     ctl     = new MchoseG75Controller(dev, cur_dev->path, MCHOSE_G75_MODE_WIRELESS);
                RGBController_MchoseG75* rgb_ctl = new RGBController_MchoseG75(ctl);
                api->RegisterVirtualRGBController(rgb_ctl);
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}

QWidget* MchoseG75Plugin::GetWidget()
{
    return nullptr;
}

QMenu* MchoseG75Plugin::GetTrayMenu()
{
    return nullptr;
}

void MchoseG75Plugin::Unload()
{

}
