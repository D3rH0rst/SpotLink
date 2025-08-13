#include "hooking.h"

#include <stdio.h>
#include <tchar.h>
#include <intrin.h>
#include <MinHook.h>

Hook hooks[20];
int hooks_size = 0;

void(*hook_vlog_fn)(const TCHAR*, va_list) = NULL;

void hook_log_fn(const TCHAR *format, ...) {
	if (hook_vlog_fn) {
		va_list args;
		va_list args_copy;
		va_start(args, format);
		va_copy(args_copy, args);
		hook_vlog_fn(format, args_copy);
		va_end(args_copy);
		va_end(args);
	}
}

Hook* get_hooks(int* out_hooks_size) {
	*out_hooks_size = hooks_size;
	return hooks;
}

int init_hooking(void) {
	MH_STATUS status;

	if ((status = MH_Initialize()) != MH_OK) {
		hook_log_fn(TEXT("[ERROR] Failed to initialize MinHook: %s"), MH_StatusToString(status));
		return 1;
	}
	if (rh_init() != 0) {
		hook_log_fn(TEXT("[ERROR] Failed to initialize RuntimeHooks"));
		return 1;
	}

	hook_log_fn(TEXT("Successfully initialized Hooking"));

	return 0;
}

void cleanup_hooking() {
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

void hooking_set_log_fn(void(*fn)(const TCHAR *, ...)) {
	hook_vlog_fn = fn;
}

int add_hook(uint64_t address, void* hk_func, void** og_func, const TCHAR* name, char start_enabled, Hook** cb_hook) {
	if (address == 0) {
		hook_log_fn(TEXT("[ERROR] Invalid hook address for hook '%s'"), name);
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
		hook_log_fn(TEXT("[ERROR] Failed to create hook '%s': %s"), name, MH_StatusToString(status));
		return 1;
	}

	if (start_enabled) {
		if ((status = MH_EnableHook((LPVOID)address)) != MH_OK) {
			hook_log_fn(TEXT("[ERROR] Failed to enable hook '%s': %s"), name, MH_StatusToString(status));
			return 1;
		}
	}

	hook_log_fn(TEXT("Successfully created hook %s at 0x%llX"), name, address);

	// TODO: check if hook_size is larger than capacity and grow if necessary

	return 0;
}

int enable_hook(Hook *h) {
	MH_STATUS status;
	if ((status = MH_EnableHook((LPVOID)(h->address))) != MH_OK) {
		hook_log_fn(TEXT("[ERROR] Error enabling hook '%s': %s"), h->name, MH_StatusToString(status));
		return 1;
	}
	hook_log_fn(TEXT("Enabled hook '%s'"), h->name);
	return 0;
}

int disable_hook(Hook * h) {
	MH_STATUS status;
	if ((status = MH_DisableHook((LPVOID)(h->address))) != MH_OK) {
		hook_log_fn(TEXT("[ERROR] Error disabling hook '%s': %s"), h->name, MH_StatusToString(status));
		return 1;
	}
	hook_log_fn(TEXT("Disabled hook '%s'"), h->name);
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
		if (hook_log_fn) hook_log_fn(TEXT("Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)"),
			_tcsrchr(module_name, TEXT('\\')) + 1,
			caller, caller - (uint64_t)m,
			caller_entry
		);
	}
	else {
		hook_log_fn(TEXT("[WARNING] CaptureStackBackTrace captured 0 frames: %ld", GetLastError()));
		void* retaddr = _ReturnAddress();
		hook_log_fn(TEXT("_ReturnAddress() returned 0x%llX"), (uint64_t)retaddr);
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
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPTSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name) / sizeof(*module_name));
		hook_log_fn(TEXT("(level 1) Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)"),
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
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPTSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name) / sizeof(*module_name));
		hook_log_fn(TEXT("(level 1) Function caller: %s!0x%llX (OFF: 0x%llX, Entry: 0x%llX)"),
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
		hook_log_fn(TEXT("[ERROR] Failed to allocate memory for RuntimeHook"));
		return NULL;
	}
	h->runtime_hook->arg_count = argcount;

	void* func_addr = make_rh_hk_func(h);

	if (!func_addr) {
		hook_log_fn(TEXT("[ERROR] Failed to generate runtime hook function"));
		free(h->runtime_hook);
		return NULL;
	}
	hook_log_fn(TEXT("Created Runtime function at 0x%llX"), (uint64_t)func_addr);

	h->hk_func = func_addr;
	h->og_func = &h->runtime_hook->og_func;

	MH_STATUS status;
	if ((status = MH_CreateHook((LPVOID)addr, func_addr, h->og_func)) != MH_OK) {
		hook_log_fn(TEXT("[ERROR] Failed to create runtime hook: %s"), MH_StatusToString(status));
		free(h->runtime_hook->called_str);
		free(h->runtime_hook);
		return NULL;
	}

	if (start_enabled) {
		if ((status = MH_EnableHook((LPVOID)addr)) != MH_OK) {
			hook_log_fn(TEXT("[ERROR] Failed to enable runtime hook: %s"), MH_StatusToString(status));
			free(h->runtime_hook->called_str);
			free(h->runtime_hook);
			return NULL;
		}
		h->enabled = 1;
	}

	hooks_size++;

	return h;
}