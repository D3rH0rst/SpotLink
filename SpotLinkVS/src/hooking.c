#include "hooking.h"

#include <stdio.h>
#include <tchar.h>
#include <intrin.h>
#include <MinHook.h>
#include <CustomControls.h>
#include "logging.h"

#define HOOK_ENABLED_CHECKBOX 1

Hook hooks[20];
int hooks_size = 0;

int hook_window_height;

Hook* get_hooks(int* out_hooks_size) {
	*out_hooks_size = hooks_size;
	return hooks;
}

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
	MH_STATUS status;
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
				if ((status = MH_EnableHook((LPVOID)(hook->address))) != MH_OK) {
					log_msg(LOG_ERROR, "Error enabling hook %s: %s", hook->name, MH_StatusToString(status));
					PostQuitMessage(1);
					return 0;
				}
				log_msg(LOG_INFO, "Enabled hook %s", hook->name);
			}
			else if (checked == BST_UNCHECKED) {
				if ((status = MH_DisableHook((LPVOID)(hook->address))) != MH_OK) {
					log_msg(LOG_ERROR, "Error disabling hook %s: %s", hook->name, MH_StatusToString(status));
					PostQuitMessage(1);
					return 0;
				}
				log_msg(LOG_INFO, "Disabled hook %s", hook->name);
			}
		}
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int init_hooking(HINSTANCE hInstance) {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = HookWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
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

	MH_STATUS status;

	if ((status = MH_Initialize()) != MH_OK) {
		log_msg(LOG_ERROR, "Error initializing MinHook: %s", MH_StatusToString(status));
		return 1;
	}
	if (rh_init() != 0) {
		log_msg(LOG_ERROR, "Failed to initialize RuntimeHooks");
		return 1;
	}

	log_msg(LOG_INFO, "Successfully initialized Hooking");

	return 0;
}

void cleanup_hooking(HINSTANCE hInstance) {
	UnregisterClass(HOOKING_WNDCLASS_NAME, hInstance);

	for (int i = 0; i < hooks_size; i++) {
		if (hooks[i].enabled)
			MH_DisableHook((LPVOID)(hooks[i].address));

		MH_RemoveHook((LPVOID)(hooks[i].address));
		if (hooks[i].runtime_hook) {
			free(hooks[i].runtime_hook->called_str);
			free(hooks[i].runtime_hook);
		}
	}

	MH_Uninitialize();
	rh_clean();
}

int add_hook(uint64_t address, void* hk_func, void** og_func, const TCHAR* name, char start_enabled, Hook** cb_hook) {
	if (address == 0) {
		log_msg(LOG_ERROR, "Invalid hook address for hook %s", name);
		return 1;
	}

	Hook* h = &hooks[hooks_size++];
	if (cb_hook)
		*cb_hook = h;

	h->address = address;
	h->hk_func = hk_func;
	h->og_func = og_func;
	//h->name = name;
	_sntprintf_s(h->name, sizeof(h->name) / sizeof(*h->name), _TRUNCATE, TEXT("%s"), name);
	h->enabled = start_enabled;

	MH_STATUS status;

	if ((status = MH_CreateHook((LPVOID)address, hk_func, og_func)) != MH_OK) {
		log_msg(LOG_ERROR, "Error creating hook %s: %s", name, MH_StatusToString(status));
		return 1;
	}

	if (start_enabled) {
		if ((status = MH_EnableHook((LPVOID)address)) != MH_OK) {
			log_msg(LOG_ERROR, "Error enabling hook %s: %s", name, MH_StatusToString(status));
			return 1;
		}
	}

	log_msg(LOG_INFO, "Successfully created hook %s at 0x%llX", name, address);

	// TODO: check if hook_size is larger than capacity and grow if necessary

	return 0;
}

void hook_called_callback(Hook* h) {
	if (!h) return;

	TCHAR buf[50];
	_stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("Called %d times"), ++h->called_count);
	SetWindowText(h->called_count_label, buf);
}

void print_caller2(void) {
	uint64_t caller;
	uint64_t caller_entry;
	uint64_t caller_base;
	PRUNTIME_FUNCTION runtime_function;
	uint16_t frame = CaptureStackBackTrace(2, 1, (void**)&caller, NULL);

	if (frame) {
		runtime_function = RtlLookupFunctionEntry(caller, &caller_base, NULL);
		if (!runtime_function) {
			caller_entry = 0;
		}
		else {
			caller_entry = caller_base + runtime_function->BeginAddress;
		}
		HMODULE m;
		TCHAR module_name[MAX_PATH];
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPTSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name) / sizeof(*module_name));
		log_msg(LOG_INFO, "Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)",
			_tcsrchr(module_name, TEXT('\\')) + 1,
			caller, caller - (uint64_t)m,
			caller_entry
		);
	}
	else {
		log_msg(LOG_WARNING, "CaptureStackBackTrace captured 0 frames: %ld", GetLastError());
		void* retaddr = _ReturnAddress();
		log_msg(LOG_DEBUG, "_ReturnAddress() returned 0x%llX", (uint64_t)retaddr);
	}
}

void print_caller(void) {
	uint64_t caller;
	uint64_t caller_entry;
	uint64_t caller_base;
	PRUNTIME_FUNCTION runtime_function;
	TCHAR module_name[MAX_PATH];
	HMODULE m;

	caller = (uint64_t)_ReturnAddress();
	if (caller) {
		runtime_function = RtlLookupFunctionEntry(caller, &caller_base, NULL);
		if (!runtime_function) {
			caller_entry = 0;
		}
		else {
			caller_entry = caller_base + runtime_function->BeginAddress;
		}
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPTSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name) / sizeof(*module_name));
		log_msg(LOG_INFO, "(level 1) Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)",
			_tcsrchr(module_name, TEXT('\\')) + 1,
			caller, caller - (uint64_t)m,
			caller_entry
		);
	}
}

void print_caller_addr(uint64_t caller) {
	uint64_t caller_entry;
	uint64_t caller_base;
	PRUNTIME_FUNCTION runtime_function;
	TCHAR module_name[MAX_PATH];
	HMODULE m;

	if (caller) {
		runtime_function = RtlLookupFunctionEntry(caller, &caller_base, NULL);
		if (!runtime_function) {
			caller_entry = 0;
		}
		else {
			caller_entry = caller_base + runtime_function->BeginAddress;
		}
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPTSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name) / sizeof(*module_name));
		log_msg(LOG_INFO, "(level 1) Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)",
			_tcsrchr(module_name, TEXT('\\')) + 1,
			caller, caller - (uint64_t)m,
			caller_entry
		);
	}
}

Hook* make_runtime_hook(uint64_t addr, const TCHAR* name, int argcount, char start_enabled) {
	Hook* h = &hooks[hooks_size];
	h->address = addr;
	
	_sntprintf_s(h->name, sizeof(h->name) / sizeof(*h->name), _TRUNCATE, TEXT("%s"), name);
	h->runtime_hook = malloc(sizeof(RuntimeHook));
	if (!h->runtime_hook) {
		log_msg(LOG_ERROR, "Failed to allocate memory for RuntimeHook");
		return NULL;
	}
	h->runtime_hook->arg_count = argcount;

	void* func_addr = make_rh_hk_func(h);

	if (!func_addr) {
		log_msg(LOG_ERROR, "Failed to generate runtime hook function");
		free(h->runtime_hook);
		return NULL;
	}
	log_msg(LOG_SUCCESS, "Created Runtime function at 0x%llX", (uint64_t)func_addr);

	h->hk_func = func_addr;
	h->og_func = &h->runtime_hook->og_func;

	MH_STATUS status;
	if ((status = MH_CreateHook((LPVOID)addr, func_addr, h->og_func)) != MH_OK) {
		log_msg(LOG_ERROR, "Failed to create runtime hook: %s", MH_StatusToString(status));
		free(h->runtime_hook->called_str);
		free(h->runtime_hook);
		return NULL;
	}

	if (start_enabled) {
		if ((status = MH_EnableHook((LPVOID)addr)) != MH_OK) {
			log_msg(LOG_ERROR, "Failed to enable runtime hook: %s", MH_StatusToString(status));
			free(h->runtime_hook->called_str);
			free(h->runtime_hook);
			return NULL;
		}
		h->enabled = 1;
	}

	hooks_size++;

	return h;
}