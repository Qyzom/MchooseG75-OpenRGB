/*---------------------------------------------------------*\
| MchoseG75Controller.cpp                                   |
|                                                           |
|   Driver for MCHOSE G75 / G75 Pro Mechanical Keyboard     |
|   Supports both Wired (0x258A:0x010C) and 2.4G (0x41E4:0x2001)|
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <cstring>
#include "MchoseG75Controller.h"
#include "StringUtils.h"

MchoseG75Controller::MchoseG75Controller(hid_device* dev_handle, const char* path, int connection_mode)
{
    dev                = dev_handle;
    location           = path ? path : "";
    mode               = connection_mode;
    stop_thread        = false;
    has_pending_update = false;

    worker_thread      = std::thread(&MchoseG75Controller::WorkerThreadLoop, this);
}

MchoseG75Controller::~MchoseG75Controller()
{
    stop_thread = true;
    update_cv.notify_all();

    if(worker_thread.joinable())
    {
        worker_thread.join();
    }

    std::lock_guard<std::mutex> lock(dev_mutex);
    if(dev)
    {
        hid_close(dev);
        dev = nullptr;
    }
}

void MchoseG75Controller::Reconnect()
{
    std::lock_guard<std::mutex> lock(dev_mutex);
    if(dev)
    {
        hid_close(dev);
        dev = nullptr;
    }

    uint16_t vid = (mode == MCHOSE_G75_MODE_WIRED) ? MCHOSE_G75_WIRED_VID : MCHOSE_G75_WIRELESS_VID;
    uint16_t pid = (mode == MCHOSE_G75_MODE_WIRED) ? MCHOSE_G75_WIRED_PID : MCHOSE_G75_WIRELESS_PID;

    struct hid_device_info* devs = hid_enumerate(vid, pid);
    struct hid_device_info* cur_dev = devs;

    while(cur_dev)
    {
        if(cur_dev->interface_number == 1)
        {
            hid_device* new_dev = hid_open_path(cur_dev->path);
            if(new_dev)
            {
                dev      = new_dev;
                location = cur_dev->path ? cur_dev->path : "";
                break;
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}

std::string MchoseG75Controller::GetLocation()
{
    return ("HID: " + location);
}

std::string MchoseG75Controller::GetName()
{
    return (mode == MCHOSE_G75_MODE_WIRED) ? "MCHOSE G75 (Wired)" : "MCHOSE G75 (Wireless 2.4G)";
}

int MchoseG75Controller::GetConnectionMode()
{
    return mode;
}

std::string MchoseG75Controller::GetSerialString()
{
    std::lock_guard<std::mutex> lock(dev_mutex);
    if(!dev)
    {
        return "";
    }
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);
    if(ret != 0)
    {
        return "";
    }
    return StringUtils::wstring_to_string(serial_string);
}

void MchoseG75Controller::SetLEDsDirect(const std::vector<RGBColor>& colors)
{
    {
        std::lock_guard<std::mutex> lock(update_mutex);
        pending_colors     = colors;
        has_pending_update = true;
    }
    update_cv.notify_one();
}

void MchoseG75Controller::WorkerThreadLoop()
{
    // Minimum safe interval between hardware writes to protect the MCU USB FIFO from crashing
    // 35ms (~28 FPS) for wired, 150ms for multi-packet 2.4G wireless
    const auto min_interval = (mode == MCHOSE_G75_MODE_WIRED)
        ? std::chrono::milliseconds(35)
        : std::chrono::milliseconds(150);

    auto last_send_time = std::chrono::steady_clock::now() - min_interval;

    while(!stop_thread)
    {
        std::vector<RGBColor> colors_to_send;
        {
            std::unique_lock<std::mutex> lock(update_mutex);
            update_cv.wait(lock, [this]() {
                return stop_thread.load() || has_pending_update;
            });

            if(stop_thread && !has_pending_update)
            {
                break;
            }

            auto now     = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_send_time);
            if(elapsed < min_interval)
            {
                auto wait_time = min_interval - elapsed;
                update_cv.wait_for(lock, wait_time, [this]() {
                    return stop_thread.load();
                });
                if(stop_thread && !has_pending_update)
                {
                    break;
                }
            }

            colors_to_send     = pending_colors;
            has_pending_update = false;
        }

        if(!colors_to_send.empty())
        {
            SendFrame(colors_to_send);
            last_send_time = std::chrono::steady_clock::now();
        }
    }
}

void MchoseG75Controller::SendFrame(const std::vector<RGBColor>& colors)
{
    if(mode == MCHOSE_G75_MODE_WIRED)
    {
        SetLEDsDirectWired(colors);
    }
    else
    {
        SetLEDsDirectWireless(colors);
    }
}

void MchoseG75Controller::SetLEDsDirectWired(const std::vector<RGBColor>& colors)
{
    unsigned char buf[MCHOSE_G75_WIRED_BUF_SIZE];
    memset(buf, 0x00, sizeof(buf));

    // Header (9 bytes) for MCHOSE G75 Wired Feature Report
    buf[0x00] = 0x06; // Report ID
    buf[0x01] = 0x06; // Lighting command
    buf[0x04] = 0x01; // Direct Matrix Mode
    buf[0x06] = 0x80; // Size low (0x0180 = 384 bytes)
    buf[0x07] = 0x01; // Size high

    // Planar RGB: Red[126] -> Green[126] -> Blue[126] starting at offset 0x09
    size_t count = (colors.size() < MCHOSE_G75_LEDS_COUNT) ? colors.size() : MCHOSE_G75_LEDS_COUNT;
    for(size_t i = 0; i < count; ++i)
    {
        buf[0x09 + i]                            = RGBGetRValue(colors[i]);
        buf[0x09 + MCHOSE_G75_LEDS_COUNT + i]     = RGBGetGValue(colors[i]);
        buf[0x09 + 2 * MCHOSE_G75_LEDS_COUNT + i] = RGBGetBValue(colors[i]);
    }

    int ret = -1;
    {
        std::lock_guard<std::mutex> lock(dev_mutex);
        if(dev)
        {
            ret = hid_send_feature_report(dev, buf, MCHOSE_G75_WIRED_BUF_SIZE);
        }
    }

    if(ret < 0)
    {
        Reconnect();
        std::lock_guard<std::mutex> lock(dev_mutex);
        if(dev)
        {
            hid_send_feature_report(dev, buf, MCHOSE_G75_WIRED_BUF_SIZE);
        }
    }
}

void MchoseG75Controller::SetLEDsDirectWireless(const std::vector<RGBColor>& colors)
{
    // Build 378-byte planar buffer
    unsigned char raw_buffer[MCHOSE_G75_LEDS_COUNT * 3];
    memset(raw_buffer, 0x00, sizeof(raw_buffer));

    size_t count = (colors.size() < MCHOSE_G75_LEDS_COUNT) ? colors.size() : MCHOSE_G75_LEDS_COUNT;
    for(size_t i = 0; i < count; ++i)
    {
        raw_buffer[i]                            = RGBGetRValue(colors[i]);
        raw_buffer[MCHOSE_G75_LEDS_COUNT + i]     = RGBGetGValue(colors[i]);
        raw_buffer[2 * MCHOSE_G75_LEDS_COUNT + i] = RGBGetBValue(colors[i]);
    }

    std::lock_guard<std::mutex> lock(dev_mutex);
    if(!dev)
    {
        Reconnect();
        if(!dev)
        {
            return;
        }
    }

    // Send 27 chunks of 14 bytes via 20-byte Output Reports (Report ID 0x13)
    unsigned char packet[20];
    for(int chunk_idx = 0; chunk_idx < MCHOSE_G75_WIRELESS_CHUNKS; ++chunk_idx)
    {
        if(stop_thread)
        {
            break;
        }

        packet[0] = 0x13;
        packet[1] = 0x02;
        packet[2] = 0x1B;
        packet[3] = (unsigned char)chunk_idx;
        packet[4] = MCHOSE_G75_CHUNK_SIZE;

        memcpy(&packet[5], &raw_buffer[chunk_idx * MCHOSE_G75_CHUNK_SIZE], MCHOSE_G75_CHUNK_SIZE);

        unsigned int checksum = 0;
        for(int b = 0; b < 19; ++b)
        {
            checksum += packet[b];
        }
        packet[19] = (unsigned char)(checksum & 0xFF);

        int written = hid_write(dev, packet, 20);
        if(written < 0)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
