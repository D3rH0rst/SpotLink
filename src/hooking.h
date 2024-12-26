#ifndef INCLUDE_HOOKING_H
#define INCLUDE_HOOKING_H

#include <Windows.h>
#include <inttypes.h>

#define HOOKING_WNDCLASS_NAME "HookingWndClass"
#define HOOKUI_PADDING 10

typedef struct {
	char *name;
	char arg_count;
	uint64_t addr;
} RH_Data;

typedef struct {
	char arg_count;
	char *called_str;
	int  stackspace;
	void *og_func;
} RuntimeHook;

typedef struct {
	uint64_t address;
	void *hk_func;
	void **og_func;
	const char *name;
	char enabled;
	char created;
	int called_count;
	HWND called_count_label;
	RuntimeHook *runtime_hook;
} Hook;

Hook *get_hooks(int *out_hooks_size);

int rh_init(void);
void rh_clean(void);
int init_hooking(HINSTANCE hInstance);
void cleanup_hooking(HINSTANCE hInstance);

int add_hook(uint64_t address, void *hk_func, void **og_func, const char *name, char start_enabled, Hook **cb_hook);

void hook_called_callback(Hook *h);

void print_caller(void);
Hook *make_runtime_hook(uint64_t addr, const char *name, int argcount, char start_enabled);
void *make_rh_hk_func(Hook *h);

#endif // INCLUDE_HOOKING_H