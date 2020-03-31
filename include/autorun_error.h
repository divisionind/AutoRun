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

#ifndef AUTORUN_AUTORUN_ERROR_H
#define AUTORUN_AUTORUN_ERROR_H

#define P_FATAL_ERROR(x) do { MessageBoxA(NULL, x, "Error", MB_OK | MB_ICONERROR); ExitProcess(1); } while(0)

#endif //AUTORUN_AUTORUN_ERROR_H
