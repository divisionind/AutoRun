/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#include "systemtray.h"
#include "resource.h"

#define TRAY_CALLBACK_MSG 101
#define TRAY_ID 100
#define ic_strcpy(dest, src) memcpy(dest, src, strlen(src) + 1)

const char szClassName[] = "GenericTrayCallback";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == TRAY_CALLBACK_MSG) {
        switch (lParam) {
            case WM_LBUTTONUP:
                MessageBoxA(hwnd, "You can exit AutoRun by simply right-clicking the tray icon\n"
                                  "or with the hotkey WIN+ALT+PAUSE.", "AutoRun", MB_OK | MB_ICONINFORMATION);
                break;
            case WM_RBUTTONUP:
                ExitProcess(0);
            default:
                break; // sanity check
        }

        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

tray_error_t tray_register(HINSTANCE hInst, tray_t* tray) {
    // create a fake window class, register it, and obtain a window hwnd (which is required for system tray icons)
    WNDCLASSEXA* winclass = &tray->fakeWindow;
    ZeroMemory(winclass, sizeof(WNDCLASSEXA));
    winclass->cbSize = sizeof(WNDCLASSEXA);
    winclass->hInstance = hInst;
    winclass->lpszClassName = szClassName;
    winclass->lpfnWndProc = WndProc;
    if (!RegisterClassExA(winclass)) {
        return TRAY_ERR_CLASS;
    }
    HWND winHwnd = CreateWindowExA(0, szClassName, szClassName, 0, 0, 0, 0, 0, 0, 0, hInst, 0);

    // create the icon using our fake window
    NOTIFYICONDATAA* iconData = &tray->icon;
    ZeroMemory(iconData, sizeof(NOTIFYICONDATAA));
    iconData->cbSize = sizeof(NOTIFYICONDATAA);
    iconData->hWnd = winHwnd;
    iconData->uID = TRAY_ID;
    iconData->uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE; // NIF_ICON means the hIcon is valid, NIF_TIP means the tip string is valid, NIF_MESSAGE means that we are receiving messages through uCallbackMessage
    iconData->hIcon = LoadIconA(hInst, MAKEINTRESOURCE(IDI_MYICON));
    iconData->uCallbackMessage = TRAY_CALLBACK_MSG;
    ic_strcpy(iconData->szTip, "AutoRun Service | right-click to exit");

    // actually add the icon to the system tray
    if (!Shell_NotifyIconA(NIM_ADD, iconData)) {
        return TRAY_ERR_NOTIFY;
    }

    return TRAY_OK;
}

tray_error_t tray_remove(tray_t* tray) {
    // removes the tray icon and notifies of errors
    if (!Shell_NotifyIconA(NIM_DELETE, &tray->icon)) {
        return TRAY_ERR_NOTIFY;
    }

    return TRAY_OK;
}
