#pragma once

#include <Windows.h>
#include <inttypes.h>

#define HOOKING_WNDCLASS_NAME TEXT("HookingWndClass")
#define HOOKUI_PADDING 10

typedef struct Hook Hook;

typedef struct {
	TCHAR name[100];
	char arg_count;
	uint64_t addr;
	char start_enabled;
} RH_Data;

Hook* get_hooks(int* out_hooks_size);
size_t get_hook_item_size(void);

int rh_init(void);
void rh_clean(void);
int init_hooking(void);
void cleanup_hooking(void);

void hooking_set_vlog_fn(void(*fn)(const TCHAR*, va_list));
void rh_set_called_callback(void(*fn)(Hook *h));
void rh_set_print_seperator(void(*fn)(void));
void rh_set_log_msg(void(*fn)(void)); // debug

int add_hook(uint64_t address, void* hk_func, void** og_func, const TCHAR* name, char start_enabled, Hook** cb_hook);
int enable_hook(Hook *h);
int disable_hook(Hook *h);

const TCHAR *get_hook_name(Hook *h);
void set_hook_data(Hook *h, void *data);
void *get_hook_data(Hook *h);
uint64_t get_hook_address(Hook *h);
void *get_hook_func(Hook *h);
char is_hook_enabled(Hook *h);
int get_hook_called_count(Hook *h);
void increment_hook_called_count(Hook *h);
int increment_and_get_hook_called_count(Hook *h);

void print_caller(void);
void print_caller2(void);
void print_caller_addr(uint64_t caller);
Hook* make_runtime_hook(uint64_t addr, const TCHAR* name, int argcount, char start_enabled);
void* make_rh_hk_func(Hook* h);
