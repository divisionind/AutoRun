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

#include "systemtray.h"
#include "resource.h"

#define TRAY_CALLBACK_MSG 101
#define TRAY_ID 69
#define ic_strcpy(dest, src) memcpy(dest, src, strlen(src) + 1)

const char szClassName[] = "GenericTrayCallback";

extern void safe_exit(int code);

BOOL CALLBACK DialogProcInfo(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    EndDialog(hwnd, wParam);
                    return TRUE;
            }
            break;
        case WM_INITDIALOG:
            RECT desktop;
            GetWindowRect(GetDesktopWindow(), &desktop);
            int horiz = desktop.right;
            int vert = desktop.bottom;

            RECT dialog;
            GetWindowRect(hwnd, &dialog);
            SetWindowPos(hwnd, NULL, (horiz/2) - (dialog.right/2), (vert/2) - (dialog.bottom/2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        default: ;
    }

    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case TRAY_CALLBACK_MSG:
            switch (lParam) {
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                    HMENU hMenu, hMenuContainer;
                    hMenuContainer = LoadMenuA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAY_MENU));
                    hMenu = GetSubMenu(hMenuContainer, 0);
                    // I ended up deciding to use resources
//                hMenu = CreatePopupMenu();
//                AppendMenuA(hMenu, MF_STRING, MENU_ID_TEST1, "Test1");
//                AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
//                AppendMenuA(hMenu, MF_STRING, MENU_ID_EXIT, "Exit");

                    POINT cursorLoc;
                    GetCursorPos(&cursorLoc);

                    SetForegroundWindow(hwnd); // needed to make the menu automatically close when it looses focus
                    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_RIGHTALIGN, cursorLoc.x, cursorLoc.y, 0, hwnd, NULL);
                    SendMessage(hwnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);
                    DestroyMenu(hMenuContainer);
                    break;
                default:
                    break;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TRAY_MENU_ABOUT:
                    MessageBoxA(NULL, "Created by Andrew Howard, <divisionind.com>. View the github "
                                      "at https://github.com/divisionind/AutoRun for more info.", "AutoRun v" AUTORUN_VERSION_ST, MB_OK | MB_ICONINFORMATION);
//                    DialogBox(NULL, MAKEINTRESOURCE(ID_DIALOG_TEST), NULL, DialogProcInfo);
                    break;
                case ID_TRAY_MENU_EXIT:
                    safe_exit(0);
                default:
                    break;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
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
    ic_strcpy(iconData->szTip, "AutoRun Service");

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
