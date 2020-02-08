/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#ifndef AUTORUN_AUTORUN_ERROR_H
#define AUTORUN_AUTORUN_ERROR_H

#define P_FATAL_ERROR(x) MessageBoxA(NULL, x, "Error", MB_OK | MB_ICONERROR); \
                         ExitProcess(-1)

#endif //AUTORUN_AUTORUN_ERROR_H
