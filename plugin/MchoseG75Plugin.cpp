/*---------------------------------------------------------*\
| MchoseG75Plugin.cpp                                       |
|                                                           |
|   Standalone OpenRGB Device Plugin for MCHOSE G75         |
|   (Unified Wired + Wireless Support)                      |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "MchoseG75Plugin.h"
#include "LogManager.h"

MchoseG75Plugin::MchoseG75Plugin()
    : api(nullptr), widget(nullptr)
{
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
    info.Version         = "1.0.3";
    info.Commit          = "1.0.3";
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

    api->LogEntry(__FILE__, __LINE__, LL_INFO, "[MchoseG75Plugin] Loading MCHOSE G75 Plugin...");
    MchoseG75_ScanWiredDevices(api, active_devices);
    MchoseG75_ScanWirelessDevices(api, active_devices);
}

void MchoseG75Plugin::ResourceManagerUpdated(unsigned int reason)
{
    if(reason == RESOURCEMANAGER_UPDATE_REASON_DETECTION_COMPLETE)
    {
        MchoseG75_ScanWiredDevices(api, active_devices);
        MchoseG75_ScanWirelessDevices(api, active_devices);
    }
}

QWidget* MchoseG75Plugin::GetWidget()
{
    if(!widget)
    {
        widget = MchoseG75_CreatePluginWidget(
            "MCHOSE G75 Plugin",
            "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard.<br>"
            "Both Wired and 2.4G Wireless connections are automatically detected and registered in OpenRGB."
        );
    }
    return widget;
}

QMenu* MchoseG75Plugin::GetTrayMenu()
{
    return nullptr;
}

void MchoseG75Plugin::Unload()
{
    MchoseG75_UnloadDevices(api, active_devices);

    if(widget)
    {
        delete widget;
        widget = nullptr;
    }
}
