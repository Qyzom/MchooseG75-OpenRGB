/*---------------------------------------------------------*\
| LogManagerStub.cpp                                        |
|                                                           |
|   Stub implementation of LogManager for standalone plugin |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "LogManager.h"

LogManager* LogManager::instance = nullptr;

LogManager::LogManager()
{
}

LogManager::~LogManager()
{
}

LogManager* LogManager::get()
{
    if(!instance)
    {
        instance = new LogManager();
    }
    return instance;
}

void LogManager::LogEntry(const char* /*filename*/, int /*line*/, unsigned int /*level*/, const char* /*fmt*/, ...)
{
}
