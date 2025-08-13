#include "main.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <CommCtrl.h>
#include "logging.h"
#include <tchar.h>
#include <crtdbg.h>

#include <hooking.h>
#include "hook_ui.h"
#include "hookfuncs.h"

#include <sigscanner.h>

#include <CustomControls.h>

#include "globals.h"

SpotLinkCtx* g_pCtx;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    UNREFERENCED_PARAMETER(lpvReserved);
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, Main, hinstDLL, 0, 0);
        break;
    }

    return TRUE;
}

DWORD WINAPI Main(LPVOID lpParameter) {
    SpotLinkCtx ctx = { 0 };
    ctx.hDll = lpParameter;
    g_pCtx = &ctx;

#ifndef NDEBUG
    _CrtSetDebugFillThreshold(0); // if this isnt called, _s functions fill the entire buffer with 0xFE which causes a crash
#endif
    if (init_main() != 0) {
        cleanup();
        return 1;
    }

    log_sep();
    log_msg(LOG_INFO, "Initialization successful - Spotify at 0x%llX", g_pCtx->hSpotify);
    log_sep();

    int ret = main_loop();

    cleanup();

    return ret;
}

int init_main(void) {
    g_pCtx->hSpotify = GetModuleHandle(NULL);
    if (!g_pCtx->hSpotify) return 1;

    sscn_set_log_fn(vlog_msg_wrapper);
    hooking_set_log_fn(vlog_msg_wrapper);

#ifdef CONSOLE
    if (init_console() != 0) return 1;
#endif

#ifdef LOGFILE
    if (init_logfile() != 0) return 1;
#endif

    if (CustomControlsInit(g_pCtx->hDll, log_msg_wrapper) != 0) return 1;

    if (rh_init_dlg() != 0) return 1;

    if (init_window() != 0) return 1;

    if (init_hooks() != 0) return 1;

    return 0;
}

#ifdef CONSOLE
int init_console(void) {
    if (!AllocConsole()) return 1;
    freopen_s(&g_pCtx->pConsole, "CONOUT$", "w", stdout);
    if (!g_pCtx->pConsole) return 1;
    if (ferror(g_pCtx->pConsole)) return 1;

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
    wc.hInstance = g_pCtx->hDll;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = SPOTLINK_WNDCLASS_NAME;
    wc.hIconSm = NULL;

    RegisterClassEx(&wc);

    RECT wr;
    wr.left = 100;
    wr.top = 100;
    wr.right = SPOTLINK_WINDOW_WIDTH + wr.left;
    wr.bottom = SPOTLINK_WINDOW_HEIGHT + wr.top;

    if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) {
        log_msg(LOG_ERROR, "Failed to AdjustWindowRect");
        return 1;
    }

    g_pCtx->spotlinkHwnd = CreateWindow(
        SPOTLINK_WNDCLASS_NAME,
        SPOTLINK_TITLE,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL,
        NULL,
        g_pCtx->hDll,
        NULL);


    if (!g_pCtx->spotlinkHwnd) {
        log_msg(LOG_ERROR, "Failed to create window\n");
        return 1;
    }

    return 0;
}

int init_ui(HWND hwnd) {
    g_pCtx->logHwnd = CreateWindow(
        TEXT("Edit"),
        TEXT(""),
        WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, SPOTLINK_WINDOW_HEIGHT / 2,
        SPOTLINK_WINDOW_WIDTH, SPOTLINK_WINDOW_HEIGHT / 2,
        hwnd,
        NULL, NULL, NULL
    );
    if (!g_pCtx->logHwnd) {
        log_msg(LOG_ERROR, "Failed to init logging window: %ld", GetLastError());
        return 1;
    }
    SendMessage(g_pCtx->logHwnd, EM_LIMITTEXT, (WPARAM)MAX_EDIT_BUFFER_SIZE, 0);

    set_log_window(&g_pCtx->logHwnd);

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

    g_pCtx->logLabelHwnd = CreateWindow(
        TEXT("Static"),
        TEXT("DebugLog length: 0"),
        WS_VISIBLE | WS_CHILD,
        200, 50,
        200, 30,
        hwnd, NULL,
        NULL, NULL
    );

    set_debug_label(&g_pCtx->logLabelHwnd);

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

    g_pCtx->pAccordion = AccordionCreate(
        SPOTLINK_WINDOW_WIDTH / 2, 0,
        SPOTLINK_WINDOW_WIDTH / 2, SPOTLINK_WINDOW_HEIGHT / 2,
        30,
        hwnd, g_pCtx->hDll
    );

    if (!g_pCtx->pAccordion) {
        log_msg(LOG_ERROR, "Failed to create accordion");
        return 1;
    }

    return 0;
}

#ifdef LOGFILE
int init_logfile(void) {
    
    if (_tfopen_s(&g_pCtx->pLogfile, TEXT("./spotlink_logfile.txt"), TEXT("w"))) {
        log_msg(LOG_ERROR, "Failed to open logfile at path %s", TEXT("./spotlink_logfile.txt"));
        return 1;
    }

    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    _ftprintf(g_pCtx->pLogfile, TEXT("SPOTLINK %02d-%02d-%d %02d:%02d:%02d\n\n"), tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);

    set_log_file(g_pCtx->pLogfile);

    return 0;
}
#endif

const TCHAR* hook_text_cb(void* hook) {
    return ((Hook*)hook)->name;
}

int init_hooks(void) {
    if (init_hooking() != 0) return 1;
    if (init_hookui() != 0) return 1;

    uint64_t spotify_base = (uint64_t)g_pCtx->hSpotify;

    uint64_t logging_addr = sscn_scan_pattern(g_pCtx->hSpotify, SIG_LOGGING_FUNC);
    ADD_HOOK(logging_addr, logging, TRUE);

    //uint64_t VPauseReqeust_addr = spotify_base + OFF_VPAUSEREQUEST;
    //ADD_HOOK(VPauseReqeust_addr, VPauseRequest, TRUE);

    //ADD_HOOK(PostQueuedCompletionStatus, PostQueuedCompletionStatus, TRUE);

    //uint64_t pause_addr = spotify_base + OFF_PAUSE;
    //ADD_HOOK(pause_addr, pause, TRUE);

    //uint64_t event_addr = spotify_base + OFF_EVENT;
    //ADD_HOOK(event_addr, event, TRUE);

    int hooks_size;
    Hook* hooks = get_hooks(&hooks_size);
    if (AccordionAddItemArray(g_pCtx->pAccordion, hooks, hooks_size, sizeof(*hooks), HOOKING_WNDCLASS_NAME, NULL, hook_text_cb, 0) != 0) {
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
    if (g_pCtx->pAccordion)
        AccordionDestroy(g_pCtx->pAccordion);
    if (g_pCtx->spotlinkHwnd)
        DestroyWindow(g_pCtx->spotlinkHwnd);
    UnregisterClass(SPOTLINK_WNDCLASS_NAME, g_pCtx->hDll);

    CustomControlsUnInit(g_pCtx->hDll);

#ifdef CONSOLE
    if (g_pCtx->pConsole) {
        fclose(g_pCtx->pConsole);
        FreeConsole();
    }
#endif

#ifdef LOGFILE
    if (g_pCtx->pLogfile)
        fclose(g_pCtx->pLogfile);
#endif

    cleanup_hooking();
    cleanup_hookui();

    if (g_pCtx->pRHDlgTemplate)
        free(g_pCtx->pRHDlgTemplate);

    CreateThread(NULL, 0, EjectThread, g_pCtx->hDll, 0, NULL);
}

DWORD WINAPI EjectThread(LPVOID lpParameter) {
    FreeLibraryAndExitThread(lpParameter, 0);
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
            break;
        case BUTTON_PAUSE:
            log_msg(LOG_INFO, "Pause button pressed");
            break;
        case BUTTON_CLEAR:
        {
            SendMessage(g_pCtx->logLabelHwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
            int log_length = GetWindowTextLength(g_pCtx->logLabelHwnd);
            TCHAR buf[50];
            _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("DebugLog length: %d"), log_length);
            SetWindowText(g_pCtx->logLabelHwnd, buf);
            break;
        }
        case BUTTON_TOGGLE_SP_LOG:
            show_spotify_log = !show_spotify_log;
            break;
        case BUTTON_CREATE_RH:
        {
            RH_Data rh_data = { 0 };
            INT_PTR result = DialogBoxIndirectParam(NULL, g_pCtx->pRHDlgTemplate, hwnd, RH_DialogWndProc, (LPARAM)(&rh_data));

            if (result == TRUE) { // Create button pressed
                log_msg(LOG_DEBUG, "Dialog TRUE result, addr: 0x%llX, args: %d, name: %s, enbl: %d", rh_data.addr, rh_data.arg_count, rh_data.name, rh_data.start_enabled);
                Hook* h = make_runtime_hook(rh_data.addr, rh_data.name, rh_data.arg_count, rh_data.start_enabled);
                if (!h) {
                    log_msg(LOG_ERROR, "Failed to create runtime hook at 0x%llX", rh_data.addr);
                    break;
                }
                AccordionAddItem(g_pCtx->pAccordion, h, HOOKING_WNDCLASS_NAME, NULL, rh_data.name, 0);
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
    _vlog_msg(LOG_NOLEVEL, format, args);
    va_end(args);
}

void vlog_msg_wrapper(const TCHAR *format, va_list args) {
    _vlog_msg(LOG_NOLEVEL, format, args);
}

int rh_init_dlg(void) {
    LPVOID lpVoid;
    WCHAR* lpwsStr;
    int nLen;

    g_pCtx->pRHDlgTemplate = (DLGTEMPLATE*)malloc(sizeof(DLGTEMPLATE) + 4);
    if (g_pCtx->pRHDlgTemplate == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for dialog template");
        return 1;
    }
    memset(g_pCtx->pRHDlgTemplate, 0, sizeof(DLGTEMPLATE) + 4);
    g_pCtx->pRHDlgTemplate->x = 0;
    g_pCtx->pRHDlgTemplate->y = 0;
    g_pCtx->pRHDlgTemplate->cx = RH_DLGWIDTH / 2; // divide by two because dlgunits is 2x pixelunits
    g_pCtx->pRHDlgTemplate->cy = RH_DLGHEIGHT / 2; // divide by two because dlgunits is 2x pixelunits
    g_pCtx->pRHDlgTemplate->style = WS_CAPTION | WS_VISIBLE | WS_SYSMENU;

    nLen = MultiByteToWideChar(CP_ACP, 0, "Create Runtime Hook", -1, NULL, 0);
    lpVoid = malloc(sizeof(DLGTEMPLATE) + 4 + (nLen * 2));
    if (lpVoid == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for lpVoid");
        return 1;
    }
    memcpy(lpVoid, g_pCtx->pRHDlgTemplate, sizeof(DLGTEMPLATE) + 4 + (nLen * 2));
    free(g_pCtx->pRHDlgTemplate);
    g_pCtx->pRHDlgTemplate = (DLGTEMPLATE*)lpVoid;

    lpwsStr = (WCHAR*)malloc(nLen * sizeof(WCHAR));
    if (lpwsStr == NULL) {
        log_msg(LOG_ERROR, "Failed to allocate memory for lpwsStr");
        return 1;
    }
    MultiByteToWideChar(CP_ACP, 0, "Create Runtime Hook", -1, lpwsStr, nLen);
    memcpy((char*)g_pCtx->pRHDlgTemplate + sizeof(DLGTEMPLATE) + 4, lpwsStr, (nLen * 2));
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


