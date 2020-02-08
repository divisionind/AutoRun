/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#ifndef AUTORUN2_AUTORUN_ERROR_H
#define AUTORUN2_AUTORUN_ERROR_H

#define P_FATAL_ERROR(x) MessageBoxA(NULL, x, "Error", MB_OK | MB_ICONERROR); \
                         exit(-1)

#endif //AUTORUN2_AUTORUN_ERROR_H
