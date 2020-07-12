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

#ifndef AUTORUN_VKARRAY_H
#define AUTORUN_VKARRAY_H

#include <cstdint>

/**
 * Prepares an array of keycodes shows status should be monitored by this program.
 * @param arraySize
 * @return
 */
uint8_t* vkarray_prepare(int* arraySize);

#endif //AUTORUN_VKARRAY_H
