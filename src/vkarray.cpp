/*
 * AutoRun - maintains pressed keys on your keyboard when activated by a hotkey
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <vector>

#include "vkarray.h"

void range(std::vector<uint8_t>& vector, int start, int end) {
    for (; start <= end; start++) {
        vector.push_back(start);
    }
}

// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
uint8_t* prepare_vkarray(int* arraySize) {
    std::vector<uint8_t> buff;

    // these are first so they are always added first in the input buffer (and apply their modification to the other keys)
    buff.push_back(VK_LSHIFT);   // shift
    buff.push_back(VK_RSHIFT);   // shift
    buff.push_back(VK_LCONTROL); // ctrl
    buff.push_back(VK_RCONTROL); // ctrl
    buff.push_back(VK_LMENU);    // alt
    buff.push_back(VK_RMENU);    // alt
    buff.push_back(VK_TAB);      // tab
    buff.push_back(VK_SPACE);    // space

    range(buff, 0x30, 0x39); // 0-9
    range(buff, 0x41, 0x5A); // A-Z
    range(buff, 0x60, 0x6F); // numpad(0-9), multiply, subtract key, etc.
    range(buff, 0x25, 0x28); // arrow keys

    // copy vector data to global buffer
    *arraySize = buff.size();
    uint8_t* vkarray = new uint8_t[*arraySize];
    for (int i = 0; i < *arraySize; ++i) {
        vkarray[i] = buff[i];
    }
    return vkarray;
}