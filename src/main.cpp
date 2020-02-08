/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#include <Windows.h>
#include <thread>
#include <atomic>
#include <vector>

#include "vkarray.h"
#include "autorun_error.h"
#include "systemtray.h"

/*
 * *NOT IT* The idea here is to use keybd_event() instead of SendInput() to avoid the problem of
 * some (games in particular) applications not responding well to SendInput() while
 * also playing nice with the previous code written for SendInput
 * https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_event
 *
 * HAS THE SAME ISSUE
 * The games might be monitoring the status of LLKHF_INJECTED and ignoring it.
 * Modify this flag in the llkeyboardproc if it is our custom release keys?
 *
 * As written in "Practical Video Game Bots: Automating Game Processes..." by Ilya Shpigor p.g. 90:
 *  "All keyboard events, which are produced by the SendInput and keybd_event WinAPI functions, have
 *  the LLKHF_INJECTED flag in the KBDLLHOOKSTRUCT structure. One the other hand, keyboard events,
 *  which are produced by a keyboard driver, do not have this flag. This flag is set on the Windows
 *  kernel level, and it is impossible to disable the flag on the WinAPI level."
 *
 * We have some possibilities in that we could create a virtual keyboard driver for our application
 * or patch the windows kernel. However, these are pretty extreme for our purposes and would likely
 * not play well with some anticheats. If we really wanted to remove this flag we could attempt to
 * do so in llkeyboardproc as noted earlier. However, this may not fix the issue either as:
 *
 * *NOT IT* SendInput and keybd_event are susceptible to User Interface Privilege Isolation (UIPI). An
 * application running at a higher integrity level than an application calling SendInput will
 * not receive this input.
 *
 * Also I have seen reports that using scancode instead of the virtual keycode could fix the
 * keyup not registering problem. See the keybd_event microsoft docs for more.
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
 * for a list of scancodes (1.4 ordinary scancodes) section
 * *THIS WAS IT*
 */

/*
 * TODO Known Issues:
 * - games like Minecraft that rely more on the actual key up and key down messages rather than key repeats
 *   still hold the keys even when repeating is done (must manually press the held key and release to stop holding)
 *   - note: I now think that the game is just not listening to our key events and the only reason
 *     it is holding the keys is because we consumed the key up event
 */
#ifdef AUTORUN_DEBUG
// can use these as well: __FILE__, __LINE__ and __func__
#define log(x, ...) printf(x, __VA_ARGS__)
#else
#define log(x, ...)
#endif

#define KEY_STATE_BUFFER_SIZE 256

tray_t tray;
HHOOK keyboardHook;
uint8_t KEY_STATES[KEY_STATE_BUFFER_SIZE];
uint8_t* vkarray;
int vkarray_size;

// whether or not the program is holding down the keys
std::atomic_bool enabled;

// task responsible for holding down the pressed keys
std::thread* holdTask;
INPUT* inputsBuffer;
UINT inputsBufferSize;

void enable_hold_task() {
    // determine held keys using our own low-level tracking of key states which ignores keys injected by us
    std::vector<uint8_t> inputCodes;
    for (int i = 0; i < vkarray_size; i++) {
        uint8_t keyCode = vkarray[i];

        // possible race condition here, however, the effect would be negligible because of how we are quantifying "enabled"/"disabled" so we ignore it for now
        // if 1, key down otherwise, up
        if (KEY_STATES[keyCode] == 1) {
            // key down, send input
            inputCodes.push_back(keyCode);
        }
    }

    // prepare a windows input array using the determined pressed keys
    inputsBufferSize = inputCodes.size();

    // ensure there are keys pressed, if not, disable
    if (inputsBufferSize == 0) {
        enabled = false;
        return;
    }
    inputsBuffer = new INPUT[inputsBufferSize];
    memset(inputsBuffer, 0, inputsBufferSize * sizeof(INPUT));
    log("Inputs(%i): ", inputsBufferSize);
    for (int i = 0; i < inputsBufferSize; i++) {
        inputsBuffer[i].type = INPUT_KEYBOARD;
        inputsBuffer[i].ki.wVk = inputCodes[i];
        // resolves a scancode for the input, necessary for most games
        inputsBuffer[i].ki.wScan = MapVirtualKeyA(inputCodes[i], MAPVK_VK_TO_VSC);

        log("0x%02X ", inputCodes[i]);
    }
    log("\n");

    // allocate a new thread and run hold task
    holdTask = new std::thread([=] {
        clock_t time;
        do {
            // repeatedly send inputs
            SendInput(inputsBufferSize, inputsBuffer, sizeof(INPUT));

            // note: ensure def CLOCKS_PER_SEC == 1000
            //Sleep(33); // replaced with the following loop to reduce maximum delay from joining task in disable_hold_task() to ~1-2ms
            time = clock();
            do {
                // or std::this_thread::yield(), i like sleep because it allows the kernel more time for context switching
                Sleep(1);
            } while ((clock() - time) < 32 && enabled);
        } while (enabled);
    });
}

void disable_hold_task() {
    // clean up and merge hold task thread
    if (holdTask != NULL) {
        // join task and wait for it to exit
        holdTask->join();

        // modify key input buffer with keyup flag and send it
        for (int i = 0; i < inputsBufferSize; i++) {
            inputsBuffer[i].ki.dwFlags |= KEYEVENTF_KEYUP;
        }
        SendInput(inputsBufferSize, inputsBuffer, sizeof(INPUT));

        // free stuff
        delete holdTask;
        delete[] inputsBuffer;
        holdTask = NULL;

        // must zero memory here because we do not track key events whilst this task is enabled TODO may not be necessary
        memset(KEY_STATES, 0, KEY_STATE_BUFFER_SIZE);
        log("toggled off\n");
    }
}

/*
 * Note: Everything processed here needs to be very quick.
 * Also note: THE STACK SIZE IS VERY SMALL
 * See this for more details: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v%3Dvs.85)
 *
 * It may be wise to queue events and process them on another thread
 * if it is chosen to do more here in the future. Use a thread-safe queue
 * like this: https://github.com/cameron314/concurrentqueue
 * or
 * You could use a blocking queue using STL like what is demonstrated in AtomicQueue
 */
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wparam, LPARAM lparam) {
    if (code == HC_ACTION) {
        KBDLLHOOKSTRUCT* hook = (KBDLLHOOKSTRUCT *) lparam;

        // if the key event was not injected by SendInput, ignore it
        if (!(hook->flags & LLKHF_INJECTED)) {
            // toggle if toggle key
            if (hook->vkCode == VK_PAUSE) {
                if (wparam == WM_KEYDOWN) {
                    enabled = !enabled;
                    if (enabled) enable_hold_task(); else disable_hold_task();
                }
            } else {
                switch (wparam) {
                    case WM_KEYUP:
                    case WM_SYSKEYUP:
                        // ignore key ups
                        if (enabled) {
                            // if key repeating is enabled and a key is released, return 1 or -1 and do not call CallNextHookEx() e.g. consume the event
                            return -1;
                        } else KEY_STATES[hook->vkCode] = 0;
                        break;
                    case WM_KEYDOWN:
                    case WM_SYSKEYDOWN:
                    default:
                        // otherwise if is key down event (fresh key press), disable status
                        if (enabled) {
                            enabled = false;
                            disable_hold_task();
                        } else {
                            KEY_STATES[hook->vkCode] = 1;
                        }
                        break;
                }
            }
        }
    }

    return CallNextHookEx(NULL, code, wparam, lparam);
}

// provides both a main and winmain entry so it can be combined as or not as WIN32
int main() {
    return WinMain(GetModuleHandle(NULL), NULL, NULL, NULL);
}

/**
 * Exits the application immediately disposing resources
 */
extern "C" void safe_exit(int code) {
    UnhookWindowsHookEx(keyboardHook);
    if (enabled) {
        enabled = false;
        disable_hold_task();
    }
    delete[] vkarray;
    tray_remove(&tray);
    ExitProcess(code);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // the handle should equal NULL here if creating the mutex failed. however, it doesnt so I just check for the error code
    CreateMutexA(0, FALSE, "Global\\AutoRunService"); // or could be Local if I wanted multiple users to be running this at a time
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        P_FATAL_ERROR("The AutoRun application is already running. See the system "
                      "tray entry for more options.");
    }

    // register hotkey first (as this will fail if another instance of the program is already running)
    if (RegisterHotKey(NULL, 1, MOD_NOREPEAT | MOD_ALT | MOD_WIN, VK_PAUSE) == 0) {
        P_FATAL_ERROR("Error registering exit hotkey to WIN+ALT+VK_PAUSE.");
    }

    // initialize the tray icon
    switch (tray_register(hInstance, &tray)) {
        case TRAY_OK:
            break;
        case TRAY_ERR_CLASS:
            P_FATAL_ERROR("Error registering system tray fake class.");
        case TRAY_ERR_NOTIFY:
            P_FATAL_ERROR("Error registering system tray icon.");
    }

    // initialize autorun info
    memset(KEY_STATES, 0, KEY_STATE_BUFFER_SIZE);
    enabled = false;
    vkarray = prepare_vkarray(&vkarray_size);

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (keyboardHook == NULL) {
        P_FATAL_ERROR("Failed to set windows low level keyboard hook.");
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            // if hotkey, must have been exit hotkey so exit
            break;
        } else {
            // otherwise, pass on the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    safe_exit(0);
    return 0;
}