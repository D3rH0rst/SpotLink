#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <CommCtrl.h>
#include "logging.h"
#include <tchar.h>
#include <crtdbg.h>

#include "hooking.h"
#include "hookfuncs.h"

#include "sigscanner.h"

#include <CustomControls.h>

#define ADD_HOOK_EX(address, name, start_enabled, hk) \
add_hook(address, hk_##name##_func, (void**)(&og_##name##_func), TEXT(#name)TEXT("_func"), start_enabled, &hk)
#define ADD_HOOK(address, name, start_enabled) \
ADD_HOOK_EX(address, name, start_enabled, hk_##name)

#define BUTTON_PLAY  (1)
#define BUTTON_PAUSE (2)
#define BUTTON_CLEAR (3)
#define BUTTON_CREATE_RH (4)
#define BUTTON_TOGGLE_SP_LOG (5)

#define RH_DLGWIDTH 600
#define RH_DLGHEIGHT 300

#define RH_ID_ADDRESS  (1001)
#define RH_ID_ARGCOUNT (1002)
#define RH_ID_HOOKNAME (1003)
#define RH_ID_STARTENABLED (1004)

HMODULE dll_handle;
uint64_t spotify_base;
uint64_t libcef_base;
#ifdef CONSOLE
FILE* console;
#endif

#ifdef LOGFILE
FILE* logfile;
const char* logfile_path = "./spotlink_logfile.txt";
#endif

HWND spotlink_hwnd;
HWND spotlink_log_hwnd;
HWND debug_label_hwnd;

Accordion* accordion;

DLGTEMPLATE* rh_dialog_template;

const TCHAR* wndclass_name = TEXT("SpotLinkWndClass");
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
void log_msg_wrapper(const TCHAR* format, ...);
int init_ui(HWND hwnd);
int init_hooks(void);
int rh_init_dlg(void);

int main_loop(void);
void cleanup(void);
DWORD WINAPI EjectThread(LPVOID lpParameter);

LRESULT CALLBACK SpotLinkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RH_DialogWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

#ifndef NDEBUG
    _CrtSetDebugFillThreshold(0); // if this isnt called, _s functions fill the entire buffer with 0xFE which causes a crash
#endif
    if (init_main() != 0) {
        cleanup();
        return 1;
    }

    log_sep();
    log_msg(LOG_INFO, "Initialization successful - Spotify at 0x%llX", spotify_base);
    log_sep();

    int ret = main_loop();

    cleanup();

    return ret;
}

int init_main(void) {
    spotify_base = (uint64_t)GetModuleHandle(NULL);
    if (!spotify_base) return 1;

    libcef_base = (uint64_t)GetModuleHandle(TEXT("libcef.dll"));
    //if (!libcef_base) return 1;

#ifdef CONSOLE
    if (init_console() != 0) return 1;
#endif

#ifdef LOGFILE
    if (init_logfile() != 0) return 1;
#endif

    if (CustomControlsInit((HINSTANCE)spotify_base, log_msg_wrapper) != 0) return 1;

    if (rh_init_dlg() != 0) return 1;

    if (init_window() != 0) return 1;

    if (init_hooks() != 0) return 1;

    return 0;
}

#ifdef CONSOLE
int init_console(void) {
    if (!AllocConsole()) return 1;
    freopen_s(&console, "CONOUT$", "w", stdout);
    if (!console) return 1;
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
        TEXT("SpotLink"),
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
        TEXT("Edit"),
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
        TEXT("Button"),
        TEXT("Play"),
        WS_VISIBLE | WS_CHILD,
        50, 50,
        60, 20,
        hwnd,
        (HMENU)BUTTON_PLAY, NULL, NULL
    );

    CreateWindow(
        TEXT("Button"),
        TEXT("Pause"),
        WS_VISIBLE | WS_CHILD,
        50, 90,
        60, 20,
        hwnd,
        (HMENU)BUTTON_PAUSE, NULL, NULL
    );

    // DEBUG
    CreateWindow(
        TEXT("Button"),
        TEXT("Create RuntimeHook"),
        WS_VISIBLE | WS_CHILD,
        50, 120,
        200, 30,
        hwnd,
        (HMENU)BUTTON_CREATE_RH, NULL, NULL
    );
    //

    debug_label_hwnd = CreateWindow(
        TEXT("Static"),
        TEXT("DebugLog length: 0"),
        WS_VISIBLE | WS_CHILD,
        200, 50,
        200, 30,
        hwnd, NULL,
        NULL, NULL
    );

    set_debug_label(&debug_label_hwnd);

    CreateWindow(
        TEXT("Button"),
        TEXT("Clear Log"),
        WS_VISIBLE | WS_CHILD,
        200, 90,
        100, 20,
        hwnd,
        (HMENU)BUTTON_CLEAR,
        NULL, NULL
    );

    CreateWindow(
        TEXT("Button"),
        TEXT("SP Logging"),
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        310, 90,
        90, 20,
        hwnd,
        (HMENU)BUTTON_TOGGLE_SP_LOG,
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
    
    if (_tfopen_s(&logfile, logfile_path, TEXT("w"))) {
        log_msg(LOG_ERROR, "Failed to open logfile at path %s", logfile_path);
        return 1;
    }

    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    _ftprintf(logfile, TEXT("SPOTLINK %02d-%02d-%d %02d:%02d:%02d\n\n"), tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);

    set_log_file(logfile);

    return 0;
}
#endif

const TCHAR* hook_text_cb(void* hook) {
    return ((Hook*)hook)->name;
}

int init_hooks(void) {

    if (init_hooking((HINSTANCE)spotify_base) != 0) return 1;

    //uint64_t logging_addr = scan_pattern((HINSTANCE)spotify_base, SIG_LOGGING_FUNC);
    //ADD_HOOK(logging_addr, logging, TRUE);

    //uint64_t VPauseReqeust_addr = spotify_base + OFF_VPAUSEREQUEST;
    //ADD_HOOK(VPauseReqeust_addr, VPauseRequest, TRUE);


    int hooks_size;
    Hook* hooks = get_hooks(&hooks_size);
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
    if (accordion)
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

    if (rh_dialog_template)
        free(rh_dialog_template);

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
        {
            SendMessage(spotlink_log_hwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
            int log_length = GetWindowTextLength(spotlink_log_hwnd);
            TCHAR buf[50];
            _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("DebugLog length: %d"), log_length);
            SetWindowText(debug_label_hwnd, buf);
            break;
        }
        case BUTTON_TOGGLE_SP_LOG:
            show_spotify_log = !show_spotify_log;
            break;
        case BUTTON_CREATE_RH:
        {
            RH_Data rh_data = { 0 };
            INT_PTR result = DialogBoxIndirectParam(NULL, rh_dialog_template, hwnd, RH_DialogWndProc, (LPARAM)(&rh_data));

            if (result == TRUE) { // Create button pressed
                log_msg(LOG_DEBUG, "Dialog TRUE result, addr: 0x%llX, args: %d, name: %s, enbl: %d", rh_data.addr, rh_data.arg_count, rh_data.name, rh_data.start_enabled);
                Hook* h = make_runtime_hook(rh_data.addr, rh_data.name, rh_data.arg_count, rh_data.start_enabled);
                if (!h) {
                    log_msg(LOG_ERROR, "Failed to create runtime hook at 0x%llX", rh_data.addr);
                    break;
                }
                AccordionAddItem(accordion, h, HOOKING_WNDCLASS_NAME, NULL, rh_data.name, 0);
                // use the rh_data to create the runtime hook
            }
            else if (result != FALSE) { // FALSE would be just a cancel
                log_msg(LOG_ERROR, "Some error occured in DialogBoxIndirectParam: %d, %ld", result, GetLastError());
            }
            break;
        }
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

void log_msg_wrapper(const TCHAR* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg(LOG_INFO, format, args);
    va_end(args);
}

int rh_init_dlg(void) {
    LPVOID lpVoid;
    WCHAR* lpwsStr;
    int nLen;

    rh_dialog_template = (DLGTEMPLATE*)malloc(sizeof(DLGTEMPLATE) + 4);
    if (rh_dialog_template == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for dialog template");
        return 1;
    }
    memset(rh_dialog_template, 0, sizeof(DLGTEMPLATE) + 4);
    rh_dialog_template->x = 0;
    rh_dialog_template->y = 0;
    rh_dialog_template->cx = RH_DLGWIDTH / 2; // divide by two because dlgunits is 2x pixelunits
    rh_dialog_template->cy = RH_DLGHEIGHT / 2; // divide by two because dlgunits is 2x pixelunits
    rh_dialog_template->style = WS_CAPTION | WS_VISIBLE | WS_SYSMENU;

    nLen = MultiByteToWideChar(CP_ACP, 0, "Create Runtime Hook", -1, NULL, 0);
    lpVoid = malloc(sizeof(DLGTEMPLATE) + 4 + (nLen * 2));
    if (lpVoid == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for lpVoid");
        return 1;
    }
    memcpy(lpVoid, rh_dialog_template, sizeof(DLGTEMPLATE) + 4 + (nLen * 2));
    free(rh_dialog_template);
    rh_dialog_template = (DLGTEMPLATE*)lpVoid;

    lpwsStr = (WCHAR*)malloc(nLen * sizeof(WCHAR));
    if (lpwsStr == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for lpwsStr");
        return 1;
    }
    MultiByteToWideChar(CP_ACP, 0, "Create Runtime Hook", -1, lpwsStr, nLen);
    memcpy((char*)rh_dialog_template + sizeof(DLGTEMPLATE) + 4, lpwsStr, (nLen * 2));
    free(lpwsStr);

    return 0;
}

int init_rh_dlg_ui(HWND hDlg) {
    HWND ui_hwnd;
    ui_hwnd = CreateWindow(TEXT("Button"), TEXT("Create"), WS_CHILD | WS_VISIBLE, 20, RH_DLGHEIGHT - 30 - 20, 100, 30, hDlg, (HMENU)IDOK, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Button for dialog: %ld", GetLastError());
        return 1;
    }

    ui_hwnd = CreateWindow(TEXT("Button"), TEXT("Cancel"), WS_CHILD | WS_VISIBLE, 140, RH_DLGHEIGHT - 30 - 20, 100, 30, hDlg, (HMENU)IDCANCEL, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Button for dialog: %ld", GetLastError());
        return 1;
    }

    ui_hwnd = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE, 20, 20, 150, 20, hDlg, (HMENU)RH_ID_ADDRESS, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Edit for dialog: %ld", GetLastError());
        return 1;
    }
    SendMessage(ui_hwnd, EM_SETCUEBANNER, (WPARAM)TRUE, (LPARAM)L"Hook Address");

    ui_hwnd = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER, 180, 20, 150, 20, hDlg, (HMENU)RH_ID_ARGCOUNT, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Edit for dialog: %ld", GetLastError());
        return 1;
    }
    SendMessage(ui_hwnd, EM_SETCUEBANNER, (WPARAM)TRUE, (LPARAM)L"Argument count");

    ui_hwnd = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE, 20, 50, 150, 20, hDlg, (HMENU)RH_ID_HOOKNAME, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Edit for dialog: %ld", GetLastError());
        return 1;
    }
    SendMessage(ui_hwnd, EM_SETCUEBANNER, (WPARAM)TRUE, (LPARAM)L"Hook Name");

    ui_hwnd = CreateWindow(TEXT("Button"), TEXT("Start Enabled"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHTBUTTON, 180, 50, 150, 20, hDlg, (HMENU)RH_ID_STARTENABLED, NULL, NULL);
    if (!ui_hwnd) {
        log_msg(LOG_ERROR, "Failed to create Checkbox for dialog: %ld", GetLastError());
        return 1;
    }

    return 0;
}

INT_PTR CALLBACK RH_DialogWndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RH_Data* rh_data = (RH_Data*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
    switch (uMsg) {
    case WM_INITDIALOG:
    {
        rh_data = (RH_Data*)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)(rh_data));

        init_rh_dlg_ui(hDlg);
        // Initialize dialog controls if needed
        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: // Handle "Create" button
        {
            TCHAR addrstr[100];
            GetDlgItemText(hDlg, RH_ID_ADDRESS, addrstr, sizeof(addrstr) / sizeof(*addrstr));


            rh_data->arg_count = GetDlgItemInt(hDlg, RH_ID_ARGCOUNT, NULL, FALSE);
            rh_data->addr = strtoull(addrstr, NULL, 0);
            GetDlgItemText(hDlg, RH_ID_HOOKNAME, rh_data->name, sizeof(rh_data->name) / sizeof(*rh_data->name));
            rh_data->start_enabled = IsDlgButtonChecked(hDlg, RH_ID_STARTENABLED);
            EndDialog(hDlg, TRUE); // Close dialog and return TRUE
            return TRUE;
        }

        case IDCANCEL: // Handle "Cancel" button
            EndDialog(hDlg, FALSE); // Close dialog and return FALSE
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, FALSE); // Handle close button
        return TRUE;
    }
    return FALSE;
}


