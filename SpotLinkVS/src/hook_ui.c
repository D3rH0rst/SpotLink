#include "hook_ui.h"
#include <Windows.h>
#include <tchar.h>
#include <CustomControls.h>
#include "globals.h"
#include "logging.h"

#include <hooking.h>

#define HOOK_ENABLED_CHECKBOX 1

int hook_window_height;

static inline int ui_create_config_label(HWND hwnd, Hook* h, int* pos_y, int* pos_x, HDC hdc, HFONT hFont) {
	TCHAR buf[100];
	int len;
	SIZE sz;
	HWND ui_hwnd;
	len = _sntprintf_s(buf, sizeof(buf) / sizeof(*buf), _TRUNCATE, TEXT("Hook Config `%s`"), h->name);
	GetTextExtentPoint32(hdc, buf, len, &sz);

	ui_hwnd = CreateWindow(
		TEXT("Static"),
		buf,
		WS_CHILD | WS_VISIBLE,
		*pos_x, *pos_y,
		sz.cx, sz.cy,
		hwnd, NULL, NULL, NULL
	);

	if (!ui_hwnd) {
		_tprintf(TEXT("Failed to create 'config_label' Window: %ld\n"), GetLastError());
		return 1;
	}

	*pos_y += sz.cy + HOOKUI_PADDING;
	SendMessage(ui_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	return 0;
}
static inline int ui_create_called_label(HWND hwnd, Hook* h, int* pos_y, int* pos_x, HDC hdc, HFONT hFont) {
	TCHAR buf[100];
	int len;
	SIZE sz;
	HWND ui_hwnd;

	len = _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("Called 0000000000 times"));
	GetTextExtentPoint32(hdc, buf, len, &sz);

	ui_hwnd = CreateWindow(
		TEXT("Static"),
		TEXT("Called 0 times"),
		WS_CHILD | WS_VISIBLE,
		*pos_x, *pos_y,
		sz.cx, sz.cy,
		hwnd, NULL, NULL, NULL
	);

	*pos_y += sz.cy + HOOKUI_PADDING;

	if (!ui_hwnd) {
		_tprintf(TEXT("Failed to create 'called_label' Window: %ld\n"), GetLastError());
		return 1;
	}

	SendMessage(ui_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	h->called_count_label = ui_hwnd;

	return 0;
}
static inline int ui_create_address_labels(HWND hwnd, Hook* h, int* pos_y, int* pos_x, HDC hdc, HFONT hFont) {
	TCHAR buf[100];
	int len;
	SIZE sz;
	HWND ui_hwnd;

	len = _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("Hook address: 0x%llX"), h->address);
	GetTextExtentPoint32(hdc, buf, len, &sz);

	ui_hwnd = CreateWindow(
		TEXT("Static"),
		buf,
		WS_CHILD | WS_VISIBLE,
		*pos_x, *pos_y,
		sz.cx, sz.cy,
		hwnd, NULL, NULL, NULL
	);

	//*pos_y += sz.cy + HOOKUI_PADDING;
	*pos_x += sz.cx + HOOKUI_PADDING;

	if (!ui_hwnd) {
		_tprintf(TEXT("Failed to create 'address_label' Window: %ld\n"), GetLastError());
		return 1;
	}

	SendMessage(ui_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	len = _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("hk_func address: 0x%llX"), (int64_t)h->hk_func);
	GetTextExtentPoint32(hdc, buf, len, &sz);

	ui_hwnd = CreateWindow(
		TEXT("Static"),
		buf,
		WS_CHILD | WS_VISIBLE,
		*pos_x, *pos_y,
		sz.cx, sz.cy,
		hwnd, NULL, NULL, NULL
	);

	*pos_y += sz.cy + HOOKUI_PADDING;
	*pos_x = HOOKUI_PADDING;

	if (!ui_hwnd) {
		_tprintf(TEXT("Failed to create 'address_label' Window: %ld\n"), GetLastError());
		return 1;
	}

	SendMessage(ui_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	return 0;
}
static inline int ui_create_enable_button(HWND hwnd, Hook* h, int* pos_y, int* pos_x, HDC hdc, HFONT hFont) {
	TCHAR buf[100];
	int len;
	SIZE sz;
	HWND ui_hwnd;
	len = _stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("Enable Hook"));
	GetTextExtentPoint32(hdc, buf, len, &sz);

	ui_hwnd = CreateWindow(
		TEXT("Button"),
		buf,
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
		*pos_x, *pos_y,
		sz.cx + 50, sz.cy, // +50 for the actual checkbox
		hwnd, (HMENU)HOOK_ENABLED_CHECKBOX, NULL, NULL
	);

	*pos_y += sz.cy + HOOKUI_PADDING;

	if (!ui_hwnd) {
		_tprintf(TEXT("Failed to create Label Window: %ld\n"), GetLastError());
		return 1;
	}

	SendMessage(ui_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(ui_hwnd, BM_SETCHECK, h->enabled, 0);

	return 0;
}

int init_hooking_ui(HWND hwnd, Hook* h) {
	int pos_y = HOOKUI_PADDING;
	int pos_x = HOOKUI_PADDING;

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	HDC hdc = GetDC(hwnd);
	HFONT oldFont = SelectObject(hdc, hFont);

	if (ui_create_config_label(hwnd, h, &pos_y, &pos_x, hdc, hFont) != 0) return 1;
	if (ui_create_called_label(hwnd, h, &pos_y, &pos_x, hdc, hFont) != 0) return 1;
	if (ui_create_address_labels(hwnd, h, &pos_y, &pos_x, hdc, hFont) != 0) return 1;
	if (ui_create_enable_button(hwnd, h, &pos_y, &pos_x, hdc, hFont) != 0) return 1;

	hook_window_height = pos_y;

	SelectObject(hdc, oldFont);
	ReleaseDC(hwnd, hdc);

	return 0;
}


LRESULT CALLBACK HookWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HBRUSH hBrush = NULL;
	Hook* hook = (Hook*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (uMsg) {
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		hook = (Hook*)cs->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)hook);
		if (init_hooking_ui(hwnd, hook) != 0) {
			log_msg(LOG_ERROR, "Failed to init hooking UI");
			PostQuitMessage(1);
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;

		// Set the background mode to transparent
		SetBkMode(hdcStatic, TRANSPARENT);

		// Return the parent window's background brush
		if (!hBrush) {
			hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // Matches default background
		}
		return (LRESULT)hBrush;
	}
	case WM_GET_CHILD_HEIGHT:
		return hook_window_height;
	case WM_COMMAND:
	{
		if (wParam == HOOK_ENABLED_CHECKBOX) {
			char checked = IsDlgButtonChecked(hwnd, HOOK_ENABLED_CHECKBOX);
			if (checked == BST_CHECKED) {
				if (enable_hook(hook) != 0) {
					PostQuitMessage(1);
					return 0;
				}
			}
			else if (checked == BST_UNCHECKED) {
				if (disable_hook(hook) != 0) {
					PostQuitMessage(1);
					return 0;
				}
			}
		}
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int init_hookui(void) {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = HookWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_pCtx->hDll;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = HOOKING_WNDCLASS_NAME;
	wc.hIconSm = NULL;

	if (!RegisterClassEx(&wc)) {
		log_msg(LOG_ERROR, "Failed to register hook window class: %ld", GetLastError());
		return 1;
	}
    return 0;
}

int cleanup_hookui(void) {
	UnregisterClass(HOOKING_WNDCLASS_NAME, g_pCtx->hDll);
	return 0;
}
