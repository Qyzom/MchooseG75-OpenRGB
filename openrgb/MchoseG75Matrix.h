/*---------------------------------------------------------*\
| MchoseG75Matrix.h                                         |
|                                                           |
|   Matrix layout and LED definitions for MCHOSE G75        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include "MchoseG75Controller.h"

#define MCHOSE_G75_MATRIX_ROWS 6
#define MCHOSE_G75_MATRIX_COLS 16

static const unsigned int g75_matrix_map[MCHOSE_G75_MATRIX_ROWS][MCHOSE_G75_MATRIX_COLS] =
{
    /* Esc   F1    F2    F3    F4    F5    F6    F7    F8    F9   F10   F11   F12  PrtSc  Del  Knob */
    {   0,    6,   12,   18,   24,   30,   36,   42,   48,   54,   60,   66,   72,   78,   90,  102 },
    /*  ~     1     2     3     4     5     6     7     8     9     0     -     =   Bksp  Home  PgUp */
    {   1,    7,   13,   19,   25,   31,   37,   43,   49,   55,   61,   67,   73,   79,   96,  108 },
    /* Tab    Q     W     E     R     T     Y     U     I     O     P     [     ]     \    End  PgDn */
    {   2,    8,   14,   20,   26,   32,   38,   44,   50,   56,   62,   68,   74,   80,   91,   97 },
    /* Caps   A     S     D     F     G     H     J     K     L     ;     '    Enter                  */
    {   3,    9,   15,   21,   27,   33,   39,   45,   51,   57,   63,   69,   81,   81,  103,  109 },
    /* LShift Z     X     C     V     B     N     M     ,     .     /   RShift        Up                */
    {   4,   10,   16,   22,   28,   34,   40,   46,   52,   58,   64,   82,   82,   88,  104,  110 },
    /* LCtrl LWin LAlt             Space             RAlt  Fn   RCtrl        Left  Down Right       */
    {   5,   11,   17,   35,   35,   35,   35,   53,   59,   65,   83,   89,   95,   95,  105,  111 }
};

static const char* const g75_led_names[MCHOSE_G75_LEDS_COUNT] =
{
    /* Col 0 */ "Key: Escape", "Key: `~", "Key: Tab", "Key: Caps Lock", "Key: Left Shift", "Key: Left Control",
    /* Col 1 */ "Key: F1", "Key: 1", "Key: Q", "Key: A", "Key: Z", "Key: Left Windows",
    /* Col 2 */ "Key: F2", "Key: 2", "Key: W", "Key: S", "Key: X", "Key: Left Alt",
    /* Col 3 */ "Key: F3", "Key: 3", "Key: E", "Key: D", "Key: C", "Key: LED 23",
    /* Col 4 */ "Key: F4", "Key: 4", "Key: R", "Key: F", "Key: V", "Key: LED 29",
    /* Col 5 */ "Key: F5", "Key: 5", "Key: T", "Key: G", "Key: B", "Key: Space",
    /* Col 6 */ "Key: F6", "Key: 6", "Key: Y", "Key: H", "Key: N", "Key: LED 41",
    /* Col 7 */ "Key: F7", "Key: 7", "Key: U", "Key: J", "Key: M", "Key: LED 47",
    /* Col 8 */ "Key: F8", "Key: 8", "Key: I", "Key: K", "Key: ,<", "Key: Right Alt",
    /* Col 9 */ "Key: F9", "Key: 9", "Key: O", "Key: L", "Key: .>", "Key: Function",
    /* Col 10 */ "Key: F10", "Key: 0", "Key: P", "Key: ;:", "Key: /?", "Key: Right Control",
    /* Col 11 */ "Key: F11", "Key: -_", "Key: [{", "Key: '\"", "Key: LED 70", "Key: LED 71",
    /* Col 12 */ "Key: F12", "Key: =+", "Key: ]}", "Key: LED 75", "Key: LED 76", "Key: LED 77",
    /* Col 13 */ "Key: Print Screen", "Key: Backspace", "Key: \\|", "Key: Enter", "Key: Right Shift", "Key: LED 83",
    /* Col 14 */ "Key: LED 84", "Key: LED 85", "Key: LED 86", "Key: LED 87", "Key: Up Arrow", "Key: Left Arrow",
    /* Col 15 */ "Key: Delete", "Key: End", "Key: LED 92", "Key: LED 93", "Key: LED 94", "Key: Down Arrow",
    /* Col 16 */ "Key: LED 96", "Key: Home", "Key: LED 98", "Key: LED 99", "Key: LED 100", "Key: LED 101",
    /* Col 17 */ "Key: Knob Click", "Key: LED 103", "Key: Page Up", "Key: LED 105", "Key: LED 106", "Key: Right Arrow",
    /* Col 18 */ "Key: LED 108", "Key: LED 109", "Key: LED 110", "Key: Page Down", "Key: LED 112", "Key: LED 113",
    /* Col 19 */ "Key: LED 114", "Key: LED 115", "Key: LED 116", "Key: LED 117", "Key: LED 118", "Key: LED 119",
    /* Col 20 */ "Key: LED 120", "Key: LED 121", "Key: LED 122", "Key: LED 123", "Key: LED 124", "Key: LED 125"
};
