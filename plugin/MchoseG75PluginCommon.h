/*---------------------------------------------------------*\
| MchoseG75PluginCommon.h                                   |
|                                                           |
|   Shared definitions and helpers for MCHOSE G75 plugins   |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <QWidget>
#include "OpenRGBPluginInterface.h"
#include "MchoseG75Controller.h"

struct MchoseG75DeviceContext
{
    MchoseG75Controller*    ctl   = nullptr;
    RGBControllerInterface* v_ctl = nullptr;
    std::string             path;
    int                     mode  = 0;
};

// Virtual controller callbacks
void MchoseG75_DeviceUpdateLEDs(void* obj);
void MchoseG75_DeviceUpdateZoneLEDs(void* obj, int zone);
void MchoseG75_DeviceUpdateSingleLED(void* obj, int led);
void MchoseG75_DeviceUpdateMode(void* obj);
void MchoseG75_DeviceSaveMode(void* obj);

// Registration & scanning helpers
void MchoseG75_RegisterKeyboard(OpenRGBPluginAPIInterface* api,
                               std::vector<MchoseG75DeviceContext*>& active_devices,
                               MchoseG75Controller* ctl,
                               const std::string& dev_path,
                               int mode);

void MchoseG75_ScanWiredDevices(OpenRGBPluginAPIInterface* api,
                               std::vector<MchoseG75DeviceContext*>& active_devices);

void MchoseG75_ScanWirelessDevices(OpenRGBPluginAPIInterface* api,
                                 std::vector<MchoseG75DeviceContext*>& active_devices);

void MchoseG75_UnloadDevices(OpenRGBPluginAPIInterface* api,
                            std::vector<MchoseG75DeviceContext*>& active_devices);

QWidget* MchoseG75_CreatePluginWidget(const QString& title, const QString& description);
