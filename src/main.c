#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "logging.h"

#include "MinHook.h"
#include "hooks.h"

#define CREATE_AND_ENABLE_HOOK(hk)                                                                                                  \
if ((status = MH_CreateHook((LPVOID)((hk).address), (hk).hk_func, (LPVOID*)((hk).og_func))) != MH_OK) {                             \
    log_msg(LOG_ERROR, "Error creating %s hook at 0x%llx: %s", (hk).name, (hk).address, MH_StatusToString(status));                 \
    return 1;                                                                                                                       \
}                                                                                                                                   \
(hk).created = 1;                                                                                                                   \
log_msg(LOG_INFO, "Successfully created %s hook at 0x%llx (og_%s at 0x%llx)", (hk).name, (hk).address, (hk).name, *((hk).og_func)); \
if ((status = MH_EnableHook((LPVOID)((hk).address))) != MH_OK) {                                                                    \
    log_msg(LOG_ERROR, "Error enabling %s hook at 0x%llx: %s", (hk).name, (hk).address, MH_StatusToString(status));                 \
    return 1;                                                                                                                       \
}                                                                                                                                   \
(hk).enabled = 1;                                                                                                                   \
log_msg(LOG_INFO, "Successfully enabled %s hook at 0x%llx", (hk).name, (hk).address);

#define ADD_HOOK(addr, hf, of, n) hooks[hooks_size++] = (Hook){.address = (addr), .hk_func = (hf), .og_func = (void**)&(of), .name = (n)};

#define MAX_HOOK_COUNT 10

#define BUTTON_PLAY  (1)
#define BUTTON_PAUSE (2)

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

Hook hooks[MAX_HOOK_COUNT];
size_t hooks_size;

const char* wndclass_name = "SpotLinkWndClass";
const int window_width = 800;
const int window_height = 600;

DWORD WINAPI Main(LPVOID lpParameter);

int init_main(void);
#ifdef CONSOLE
int init_console(void);
#endif
#ifdef LOGFILE
int init_logfile(void);
#endif
int init_window();
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

    if (init_window() != 0) return 1;

#ifdef LOGFILE
    if (init_logfile() != 0) return 1;
#endif

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
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
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

    ShowWindow(spotlink_hwnd, SW_SHOWDEFAULT);

    return 0;
}

int init_ui(HWND hwnd) {
    spotlink_log_hwnd = CreateWindow(
        "Edit",
        "", 
        WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
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

    return 0;
}

#ifdef LOGFILE
int init_logfile(void) {
    logfile = fopen(logfile_path, "w");
    if (!logfile) {
        log_msg(LOG_ERROR, "Failed to open logfile at path %s", logfile_path);
        return 1;
    }
    
    //setvbuf(logfile, NULL, _IONBF, 0);  // Disable buffering
    
    set_log_file(logfile);
    
    return 0;
}
#endif

int init_hooks(void) {
    MH_STATUS status;
    
    if ((status = MH_Initialize()) != MH_OK) {
        log_msg(LOG_ERROR, "Error initializing MinHook: %s", MH_StatusToString(status));
        return 1;
    }
    log_msg(LOG_INFO, "Successfully initialized MinHook");


    //ADD_HOOK(spotify_base + OFFSET_PAUSERESUME_FUNC, hk_pauseresume_func, og_pauseresume_func, "pauseresume_func");
    //ADD_HOOK(spotify_base + OFFSET_PAUSE_FUNC,       hk_pause_func,       og_pause_func,       "pause_func");
    //ADD_HOOK(spotify_base + OFFSET_RESUME_FUNC,      hk_resume_func,      og_resume_func,      "resume_func");
    //ADD_HOOK(spotify_base + OFFSET_PLAY_FUNC,        hk_play_func,        og_play_func,        "play_func");
    //ADD_HOOK(spotify_base + OFFSET_TOGGLE_PAUSE_FUNC,hk_toggle_pause_func,og_toggle_pause_func,"toggle_pause_func");
    //ADD_HOOK(spotify_base + OFFSET_TOGGLE_PLAY_FUNC, hk_toggle_play_func, og_toggle_play_func, "toggle_play_func");
    //ADD_HOOK(spotify_base + OFFSET_CHECK_PARAM_HOOK, hk_check_param_hook, og_check_param_hook, "check_param_func");
    //ADD_HOOK(spotify_base + OFFSET_SET_PAUSE_FUNC,   hk_set_pause_func, og_set_pause_func, "set_pause_func");
    //ADD_HOOK(spotify_base + OFFSET_SET_PLAY_FUNC,   hk_set_play_func, og_set_play_func, "set_play_func");
    //ADD_HOOK(spotify_base + OFFSET_NEXT_PAUSE_FUNC,   hk_next_pause_func, og_next_pause_func, "next_pause_func");
    //ADD_HOOK(spotify_base + OFFSET_NEXT_PLAY_FUNC,   hk_next_play_func, og_next_play_func, "next_play_func");
    //ADD_HOOK(spotify_base + OFFSET_1_PAUSE_FUNC,   hk_1_pause_func, og_1_pause_func, "1_pause_func");
    //ADD_HOOK(spotify_base + OFFSET_1_PLAY_FUNC,   hk_1_play_func, og_1_play_func, "1_play_func");
    //ADD_HOOK(spotify_base + OFFSET_2_PAUSE_FUNC,   hk_2_pause_func, og_2_pause_func, "2_pause_func");
    //ADD_HOOK(spotify_base + OFFSET_2_PLAY_FUNC,   hk_2_play_func, og_2_play_func, "2_play_func");
    
    //ADD_HOOK(spotify_base + OFFSET_MEDIAKEY_CTRL_FUNC, hk_mediakey_ctrl_func, og_mediakey_ctrl_func, "mediakey_ctrl_func");
    
    //ADD_HOOK(spotify_base + OFFSET_3_PAUSE_PLAY_FUNC,   hk_3_pause_play_func, og_3_pause_play_func, "3_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_4_PAUSE_PLAY_FUNC,   hk_4_pause_play_func, og_4_pause_play_func, "4_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_5_PAUSE_PLAY_FUNC,   hk_5_pause_play_func, og_5_pause_play_func, "5_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_6_PAUSE_PLAY_FUNC,   hk_6_pause_play_func, og_6_pause_play_func, "6_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_7_PAUSE_PLAY_FUNC,   hk_7_pause_play_func, og_7_pause_play_func, "7_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_8_PAUSE_PLAY_FUNC,   hk_8_pause_play_func, og_8_pause_play_func, "8_pause_play_func");
    //ADD_HOOK(spotify_base + OFFSET_9_PAUSE_PLAY_FUNC,   hk_9_pause_play_func, og_9_pause_play_func, "9_pause_play_func"); crashes on unload
    
    //ADD_HOOK(spotify_base + OFFSET_2_MEDIA_FUNC,   hk_2_media_func, og_2_media_func, "2_media_func");
    ADD_HOOK(spotify_base + OFFSET_3_MEDIA_FUNC,   hk_3_media_func, og_3_media_func, "3_media_func");
    ADD_HOOK(spotify_base + OFFSET_4_MEDIA_FUNC,   hk_4_media_func, og_4_media_func, "4_media_func");
    
    

    for (size_t i = 0; i < hooks_size; i++) {
        CREATE_AND_ENABLE_HOOK(hooks[i]);
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
    if (spotlink_hwnd)
        DestroyWindow(spotlink_hwnd);
    UnregisterClass(wndclass_name, (HINSTANCE)spotify_base);


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

    //log_msg(LOG_INFO, "");

    //while (InterlockedCompareExchange(&is_critical, 0, 0) != 0); // wait until critical section is done
    
    //SuspendAllThreadsExceptCurrent();

    for (size_t i = 0; i < hooks_size; i++) {
        
        //EnterCriticalSection((LPCRITICAL_SECTION)hooks[i].address);
        //is_critical = 1;
        if (hooks[i].enabled)
            MH_DisableHook((LPVOID)(hooks[i].address));
        
        if (hooks[i].created)
            MH_RemoveHook((LPVOID)(hooks[i].address));
        //is_critical = 0;
        //LeaveCriticalSection((LPCRITICAL_SECTION)hooks[i].address);
    }

    //ResumeAllThreads();

    MH_Uninitialize();


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
            PostQuitMessage(0);
            return 1;
        }
        break;
    case WM_COMMAND:
        switch (wParam) {
        case BUTTON_PLAY:
            log_msg(LOG_INFO, "Play button pressed");
            //hk_resume_func(0x1bfc08abf40, 0, 0x9888bfd5c0);
            break;
        case BUTTON_PAUSE:
            log_msg(LOG_INFO, "Pause button pressed");
            //hk_resume_func(0x1bfc08abf40, 1, 0x9888bfdcc0);
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