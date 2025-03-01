#pragma once

#include <Windows.h>
#include <inttypes.h>

#define HOOKING_WNDCLASS_NAME TEXT("HookingWndClass")
#define HOOKUI_PADDING 10

typedef struct {
	TCHAR name[100];
	char arg_count;
	uint64_t addr;
	char start_enabled;
} RH_Data;

typedef struct {
	char arg_count;
	TCHAR* called_str;
	int  stackspace;
	void* og_func;
} RuntimeHook;

typedef struct {
	uint64_t address;
	void* hk_func;
	void** og_func;
	TCHAR name[100];
	char enabled;
	char created;
	int called_count;
	HWND called_count_label;
	RuntimeHook* runtime_hook;
} Hook;

Hook* get_hooks(int* out_hooks_size);

int rh_init(void);
void rh_clean(void);
int init_hooking(HINSTANCE hInstance);
void cleanup_hooking(HINSTANCE hInstance);

int add_hook(uint64_t address, void* hk_func, void** og_func, const TCHAR* name, char start_enabled, Hook** cb_hook);

void hook_called_callback(Hook* h);

void print_caller(void);
void print_caller2(void);
void print_caller_addr(uint64_t caller);
Hook* make_runtime_hook(uint64_t addr, const TCHAR* name, int argcount, char start_enabled);
void* make_rh_hk_func(Hook* h);
