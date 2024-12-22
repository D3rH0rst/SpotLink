#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "logging.h"

#include "MinHook.h"

#include "hooking.h"
#include "hookfuncs.h"

#include "sigscanner.h"

#include <Win32CustomControls/CustomControls.h>

#define BUTTON_PLAY  (1)
#define BUTTON_PAUSE (2)
#define BUTTON_CLEAR (3)

HMODULE dll_handle;
uint64_t spotify_base;
uint64_t libcef_base;
#ifdef CONSOLE
FILE* console;
#endif

#ifdef LOGFILE
FILE* logfile;
const char* logfile_path = "C:\\Programming\\Projects\\SpotLink\\C_DLL\\logfile.txt";
#endif

HWND spotlink_hwnd;
HWND spotlink_log_hwnd;
HWND debug_label_hwnd;

Accordion *accordion;

const char* wndclass_name = "SpotLinkWndClass";
const int window_width = 1280;
const int window_height = 720;

DWORD WINAPI Main(LPVOID lpParameter);

int init_main(void);
#ifdef CONSOLE
int init_console(void);
#endif
#ifdef LOGFILE
int init_logfile(void);
#endif
int init_window();
void log_msg_wrapper(const char *format, ...);
int init_ui(HWND hwnd);
int init_hooks(void);

int main_loop(void);
void cleanup(void);
DWORD WINAPI EjectThread(LPVOID lpParameter);

void SuspendAllThreadsExceptCurrent(void);
void ResumeAllThreads(void);

LRESULT CALLBACK SpotLinkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    UNREFERENCED_PARAMETER(lpvReserved);
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            dll_handle = hinstDLL;
            CreateThread(NULL, 0, Main, NULL, 0, 0);
            break;
    }
    return TRUE;
}

DWORD WINAPI Main(LPVOID lpParameter) {
    UNREFERENCED_PARAMETER(lpParameter);

    if (init_main() != 0) return 1;

    log_sep();
    log_msg(LOG_INFO, "Successfully initialized everything");
    log_msg(LOG_INFO, "spotify.exe Base Address: 0x%llx", spotify_base);
    log_msg(LOG_INFO, "libcef.dll  Base Address: 0x%llx", libcef_base);
    log_sep();

    int ret = main_loop();

    cleanup();

    return ret;
}

int init_main(void) {
    spotify_base = (uint64_t)GetModuleHandle(NULL);
    if (!spotify_base) return 1;
    
    libcef_base = (uint64_t)GetModuleHandle("libcef.dll");
    if (!libcef_base) return 1;

#ifdef CONSOLE
    if (init_console() != 0) return 1;
#endif

#ifdef LOGFILE
    if (init_logfile() != 0) return 1;
#endif

    if (CustomControlsInit((HINSTANCE)spotify_base, log_msg_wrapper) != 0) return 1;

    if (init_window() != 0) return 1;

    if (init_hooks() != 0) return 1;

    return 0;
}

#ifdef CONSOLE
int init_console(void) {
    if (!AllocConsole()) return 1;
    freopen_s(&console, "CONOUT$", "w", stdout);

    if (ferror(console)) return 1;

    return 0;
}
#endif

int init_window(void) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = SpotLinkWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = (HINSTANCE)spotify_base;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wndclass_name;
    wc.hIconSm = NULL;

    RegisterClassEx(&wc);

    RECT wr;
    wr.left = 100;
    wr.top = 100;
    wr.right = window_width + wr.left;
    wr.bottom = window_height + wr.top;

    if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) {
        log_msg(LOG_ERROR, "Failed to AdjustWindowRect");
        return 1;
    }

    spotlink_hwnd = CreateWindow(
        wndclass_name,
        "SpotLink",
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL,
        NULL,
        (HINSTANCE)spotify_base,
        NULL);


    if (!spotlink_hwnd) {
        log_msg(LOG_ERROR, "Failed to create window\n");
        return 1;
    }

    return 0;
}

int init_ui(HWND hwnd) {
    spotlink_log_hwnd = CreateWindow(
        "Edit",
        "", 
        WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, window_height / 2,
        window_width, window_height / 2,
        hwnd,
        NULL, NULL, NULL
    );
    if (!spotlink_log_hwnd) {
        log_msg(LOG_ERROR, "Failed to init logging window: %ld", GetLastError());
        return 1;
    }
    SendMessage(spotlink_log_hwnd, EM_LIMITTEXT, (WPARAM)MAX_EDIT_BUFFER_SIZE, 0);

    set_log_window(&spotlink_log_hwnd);

    CreateWindow(
        "Button",
        "Play",
        WS_VISIBLE | WS_CHILD,
        50, 50,
        60, 20,
        hwnd,
        (HMENU)BUTTON_PLAY, NULL, NULL
    );

    CreateWindow(
        "Button",
        "Pause",
        WS_VISIBLE | WS_CHILD,
        50, 90,
        60, 20,
        hwnd,
        (HMENU)BUTTON_PAUSE, NULL, NULL
    );

    debug_label_hwnd = CreateWindow(
        "Static",
        "DebugLog length: 0",
        WS_VISIBLE | WS_CHILD,
        200, 50,
        200, 30,
        hwnd, NULL,
        NULL, NULL
    );

    set_debug_label(&debug_label_hwnd);

    CreateWindow(
        "Button",
        "Clear Log",
        WS_VISIBLE | WS_CHILD,
        200, 90,
        100, 20,
        hwnd,
        (HMENU)BUTTON_CLEAR,
        NULL, NULL
    );

    accordion = AccordionCreate(
        window_width / 2, 0,
        window_width / 2, window_height / 2,
        30,
        hwnd, (HINSTANCE)spotify_base
    );

    if (!accordion) {
        log_msg(LOG_ERROR, "Failed to create accordion");
        return 1;
    }

    return 0;
}

#ifdef LOGFILE
int init_logfile(void) {
    logfile = fopen(logfile_path, "w");
    if (!logfile) {
        log_msg(LOG_ERROR, "Failed to open logfile at path %s", logfile_path);
        return 1;
    }
    
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(logfile, "SPOTLINK %02d-%02d-%d %02d:%02d:%02d\n\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    set_log_file(logfile);
    
    return 0;
}
#endif

const char *hook_text_cb(void* hook) {
    return ((Hook*)hook)->name;
}

int init_hooks(void) {

    if (init_hooking((HINSTANCE)spotify_base) != 0) return 1;

    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_PAUSE_FUNC), hk_pause_func, (void**)(&og_pause_func), "pause_func", 1, &hk_pause);
    add_hook(scan_pattern_ex((HINSTANCE)spotify_base, SIG_PLAY_FUNC, 1), hk_play_func, (void**)(&og_play_func), "play_func", 1, &hk_play);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_NEXT_FUNC), hk_next_func, (void**)(&og_next_func), "next_func", 1, &hk_next);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_PREV_FUNC), hk_prev_func, (void**)(&og_prev_func), "prev_func", 1, &hk_prev);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_SEEK_FUNC), hk_seek_func, (void**)(&og_seek_func), "seek_func", 1, &hk_seek);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_SONG_FUNC), hk_song_func, (void**)(&og_song_func), "song_func", 1, &hk_song);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_REPEAT_FUNC), hk_repeat_func, (void**)(&og_repeat_func), "repeat_func", 1, &hk_repeat);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_SHUFFLE1_FUNC), hk_shuffle1_func, (void**)(&og_shuffle1_func), "shuffle1_func", 1, &hk_shuffle1);
    add_hook(scan_pattern((HINSTANCE)spotify_base, SIG_SHUFFLE2_FUNC), hk_shuffle2_func, (void**)(&og_shuffle2_func), "shuffle2_func", 1, &hk_shuffle2);
   
    int hooks_size;
    Hook *hooks = get_hooks(&hooks_size);
    if (AccordionAddItemArray(accordion, hooks, hooks_size, sizeof(*hooks), HOOKING_WNDCLASS_NAME, NULL, hook_text_cb, 0) != 0) {
        log_msg(LOG_ERROR, "Failed to add item array to accordion");
        return 1;
    }

    return 0;
}

int main_loop(void) {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT)
            return msg.wParam;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void cleanup(void) {
    AccordionDestroy(accordion);
    if (spotlink_hwnd)
        DestroyWindow(spotlink_hwnd);
    UnregisterClass(wndclass_name, (HINSTANCE)spotify_base);

    CustomControlsUnInit((HINSTANCE)spotify_base);

#ifdef CONSOLE
    if (console) {
        fclose(console);
        FreeConsole();
    }
#endif

#ifdef LOGFILE
    if (logfile)
        fclose(logfile);
#endif

    cleanup_hooking((HINSTANCE)spotify_base);

    CreateThread(NULL, 0, EjectThread, NULL, 0, NULL);
}

DWORD WINAPI EjectThread(LPVOID lpParameter) {
    UNREFERENCED_PARAMETER(lpParameter);
    FreeLibraryAndExitThread(dll_handle, 0);
    return 0;
}

LRESULT CALLBACK SpotLinkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        if (init_ui(hwnd) != 0) {
            PostQuitMessage(1);
            break;
        }
        break;
    case WM_COMMAND:
        switch (wParam) {
        case BUTTON_PLAY:
            log_msg(LOG_INFO, "Play button pressed");
            //hk_esperanto_pause();
            //hk_resume_func(0x1bfc08abf40, 0, 0x9888bfd5c0);
            break;
        case BUTTON_PAUSE:
            log_msg(LOG_INFO, "Pause button pressed");
            //hk_resume_func(0x1bfc08abf40, 1, 0x9888bfdcc0);
            break;
        case BUTTON_CLEAR:
            SendMessage(spotlink_log_hwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
            int log_length = GetWindowTextLength(spotlink_log_hwnd);
            char buf[50];
            sprintf(buf, "DebugLog length: %d", log_length);
            SetWindowText(debug_label_hwnd, buf);
            break;
        }
        break;
    default:
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void log_msg_wrapper(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_msg(LOG_INFO, format, args);
    va_end(args);
}

void SuspendAllThreadsExceptCurrent(void) {
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    DWORD our_pid = GetCurrentProcessId();
    DWORD our_tid = GetCurrentThreadId();

    if (Thread32First(hProcess, &te32)) {
        do {
            if (te32.th32OwnerProcessID == our_pid && te32.th32ThreadID != our_tid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    SuspendThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hProcess, &te32));
    }
    CloseHandle(hProcess);
}

void ResumeAllThreads(void) {
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    DWORD our_pid = GetCurrentProcessId();
    DWORD our_tid = GetCurrentThreadId();

    if (Thread32First(hProcess, &te32)) {
        do {
            if (te32.th32OwnerProcessID == our_pid && te32.th32ThreadID != our_tid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hProcess, &te32));
    }
    CloseHandle(hProcess);
}