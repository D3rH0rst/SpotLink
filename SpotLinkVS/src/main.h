#pragma once

#include <Windows.h>

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

#define SPOTLINK_WNDCLASS_NAME TEXT("SpotLinkWndClass")

#ifdef NDEBUG
#define SPOTLINK_TITLE TEXT("SpotLink (R)")
#else
#define SPOTLINK_TITLE TEXT("SpotLink (D)")
#endif

#define SPOTLINK_WINDOW_WIDTH 1280
#define SPOTLINK_WINDOW_HEIGHT 720

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
void vlog_msg_wrapper(const TCHAR* format, va_list args);
int init_ui(HWND hwnd);
int init_hooks(void);
int rh_init_dlg(void);

int main_loop(void);
void cleanup(void);
DWORD WINAPI EjectThread(LPVOID lpParameter);

LRESULT CALLBACK SpotLinkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RH_DialogWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);