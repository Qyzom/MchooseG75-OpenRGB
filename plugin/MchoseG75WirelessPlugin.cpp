/*---------------------------------------------------------*\
| MchoseG75WirelessPlugin.cpp                               |
|                                                           |
|   OpenRGB Device Plugin for MCHOSE G75 (2.4G Wireless)    |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "MchoseG75WirelessPlugin.h"
#include "LogManager.h"

MchoseG75WirelessPlugin::MchoseG75WirelessPlugin()
    : api(nullptr), widget(nullptr)
{
}

MchoseG75WirelessPlugin::~MchoseG75WirelessPlugin()
{
    Unload();
}

OpenRGBPluginInfo MchoseG75WirelessPlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name            = "MCHOSE G75 Wireless Plugin";
    info.Description     = "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard (2.4G Wireless Dongle)";
    info.Version         = "1.0.3";
    info.Commit          = "1.0.3";
    info.URL             = "https://github.com/Qyzom/MchooseG75-OpenRGB";
    info.Location        = OPENRGB_PLUGIN_LOCATION_SETTINGS;
    info.Label           = "MCHOSE G75 Wireless";
    info.ProtocolVersion = 0;

    return info;
}

unsigned int MchoseG75WirelessPlugin::GetPluginAPIVersion()
{
    return OPENRGB_PLUGIN_API_VERSION;
}

void MchoseG75WirelessPlugin::Load(OpenRGBPluginAPIInterface* plugin_api_ptr)
{
    api = plugin_api_ptr;
    if(!api)
    {
        return;
    }

    api->LogEntry(__FILE__, __LINE__, LL_INFO, "[MchoseG75WirelessPlugin] Loading MCHOSE G75 Wireless Plugin...");
    MchoseG75_ScanWirelessDevices(api, active_devices);
}

void MchoseG75WirelessPlugin::ResourceManagerUpdated(unsigned int reason)
{
    if(reason == RESOURCEMANAGER_UPDATE_REASON_DETECTION_COMPLETE)
    {
        MchoseG75_ScanWirelessDevices(api, active_devices);
    }
}

QWidget* MchoseG75WirelessPlugin::GetWidget()
{
    if(!widget)
    {
        widget = MchoseG75_CreatePluginWidget(
            "MCHOSE G75 Wireless Plugin",
            "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard (2.4G Wireless Dongle).<br>"
            "Automatically detected and registered in OpenRGB."
        );
    }
    return widget;
}

QMenu* MchoseG75WirelessPlugin::GetTrayMenu()
{
    return nullptr;
}

void MchoseG75WirelessPlugin::Unload()
{
    MchoseG75_UnloadDevices(api, active_devices);

    if(widget)
    {
        delete widget;
        widget = nullptr;
    }
}
