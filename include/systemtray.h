/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#ifndef AUTORUN_SYSTEMTRAY_H
#define AUTORUN_SYSTEMTRAY_H

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NOTIFYICONDATAA icon;
    WNDCLASSEXA fakeWindow;
} tray_t;

typedef enum {
    TRAY_OK = 0,
    TRAY_ERR_CLASS,
    TRAY_ERR_NOTIFY
} tray_error_t;

tray_error_t tray_register(HINSTANCE, tray_t*);
tray_error_t tray_remove(tray_t*);

#ifdef __cplusplus
}
#endif

#endif //AUTORUN_SYSTEMTRAY_H
