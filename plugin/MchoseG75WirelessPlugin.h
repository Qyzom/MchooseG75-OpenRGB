/*---------------------------------------------------------*\
| MchoseG75WirelessPlugin.h                                 |
|                                                           |
|   OpenRGB Device Plugin for MCHOSE G75 (2.4G Wireless)    |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <QObject>
#include <QtPlugin>
#include <vector>
#include <string>
#include "OpenRGBPluginInterface.h"
#include "ResourceManagerCallback.h"

struct MchoseG75DeviceContext;

class MchoseG75WirelessPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID FILE "MchoseG75WirelessPlugin.json")
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    MchoseG75WirelessPlugin();
    ~MchoseG75WirelessPlugin();

    OpenRGBPluginInfo   GetPluginInfo() override;
    unsigned int        GetPluginAPIVersion() override;

    void                Load(OpenRGBPluginAPIInterface* plugin_api_ptr) override;
    QWidget*            GetWidget() override;
    QMenu*              GetTrayMenu() override;
    void                Unload() override;

    void                OnProfileAboutToLoad() override {}
    void                OnProfileLoad(nlohmann::json) override {}
    nlohmann::json      OnProfileSave() override { return nlohmann::json(); }
    unsigned char*      OnSDKCommand(unsigned int, unsigned char*, unsigned int*) override { return nullptr; }

    void                ProfileManagerUpdated(unsigned int) override {}
    void                ResourceManagerUpdated(unsigned int reason) override;
    void                SettingsManagerUpdated(unsigned int) override {}

private:
    void                ScanDevices();
    void                RegisterKeyboard(class MchoseG75Controller* ctl, const std::string& dev_path);

    OpenRGBPluginAPIInterface*           api;
    QWidget*                             widget;
    std::vector<MchoseG75DeviceContext*> active_devices;
};
