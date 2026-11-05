/*---------------------------------------------------------*\
| MchoseG75WiredPlugin.cpp                                  |
|                                                           |
|   OpenRGB Device Plugin for MCHOSE G75 (USB Wired)        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "MchoseG75WiredPlugin.h"
#include "LogManager.h"

MchoseG75WiredPlugin::MchoseG75WiredPlugin()
    : api(nullptr), widget(nullptr)
{
}

MchoseG75WiredPlugin::~MchoseG75WiredPlugin()
{
    Unload();
}

OpenRGBPluginInfo MchoseG75WiredPlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name            = "MCHOSE G75 Wired Plugin";
    info.Description     = "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard (USB Wired)";
    info.Version         = "1.0.3";
    info.Commit          = "1.0.3";
    info.URL             = "https://github.com/Qyzom/MchooseG75-OpenRGB";
    info.Location        = OPENRGB_PLUGIN_LOCATION_SETTINGS;
    info.Label           = "MCHOSE G75 Wired";
    info.ProtocolVersion = 0;

    return info;
}

unsigned int MchoseG75WiredPlugin::GetPluginAPIVersion()
{
    return OPENRGB_PLUGIN_API_VERSION;
}

void MchoseG75WiredPlugin::Load(OpenRGBPluginAPIInterface* plugin_api_ptr)
{
    api = plugin_api_ptr;
    if(!api)
    {
        return;
    }

    api->LogEntry(__FILE__, __LINE__, LL_INFO, "[MchoseG75WiredPlugin] Loading MCHOSE G75 Wired Plugin...");
    MchoseG75_ScanWiredDevices(api, active_devices);
}

void MchoseG75WiredPlugin::ResourceManagerUpdated(unsigned int reason)
{
    if(reason == RESOURCEMANAGER_UPDATE_REASON_DETECTION_COMPLETE)
    {
        MchoseG75_ScanWiredDevices(api, active_devices);
    }
}

QWidget* MchoseG75WiredPlugin::GetWidget()
{
    if(!widget)
    {
        widget = MchoseG75_CreatePluginWidget(
            "MCHOSE G75 Wired Plugin",
            "Native driver plugin for MCHOSE G75 / G75 Pro Mechanical Keyboard (USB Wired).<br>"
            "Automatically detected and registered in OpenRGB."
        );
    }
    return widget;
}

QMenu* MchoseG75WiredPlugin::GetTrayMenu()
{
    return nullptr;
}

void MchoseG75WiredPlugin::Unload()
{
    MchoseG75_UnloadDevices(api, active_devices);

    if(widget)
    {
        delete widget;
        widget = nullptr;
    }
}
