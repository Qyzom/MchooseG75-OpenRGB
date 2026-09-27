/*---------------------------------------------------------*\
| MchoseG75Controller.h                                     |
|                                                           |
|   Driver for MCHOSE G75 / G75 Pro Mechanical Keyboard     |
|   Supports both Wired (0x258A:0x010C) and 2.4G (0x41E4:0x2001)|
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "RGBController.h"

#if defined(__has_include)
  #if __has_include(<hidapi/hidapi.h>)
    #include <hidapi/hidapi.h>
  #elif __has_include(<hidapi.h>)
    #include <hidapi.h>
  #else
    #include "hidapi.h"
  #endif
#else
  #include <hidapi/hidapi.h>
#endif

enum
{
    MCHOSE_G75_MODE_WIRED    = 0,
    MCHOSE_G75_MODE_WIRELESS = 1
};

#define MCHOSE_G75_WIRED_VID        0x258A
#define MCHOSE_G75_WIRED_PID        0x010C

#define MCHOSE_G75_WIRELESS_VID     0x41E4
#define MCHOSE_G75_WIRELESS_PID     0x2001

#define MCHOSE_G75_LEDS_COUNT       126
#define MCHOSE_G75_WIRED_BUF_SIZE   520
#define MCHOSE_G75_WIRELESS_CHUNKS  27
#define MCHOSE_G75_CHUNK_SIZE       14

class MchoseG75Controller
{
public:
    MchoseG75Controller(hid_device* dev_handle, const char* path, int connection_mode);
    ~MchoseG75Controller();

    std::string GetLocation();
    std::string GetName();
    std::string GetSerialString();
    int         GetConnectionMode();

    void        SetLEDsDirect(const std::vector<RGBColor>& colors);
    void        Reconnect();

private:
    hid_device* dev;
    std::string location;
    int         mode;

    void        SetLEDsDirectWired(const std::vector<RGBColor>& colors);
    void        SetLEDsDirectWireless(const std::vector<RGBColor>& colors);
};
