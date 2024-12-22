#ifndef INCLUDE_HOOKING_H
#define INCLUDE_HOOKING_H

#include <Windows.h>
#include <inttypes.h>

#define HOOKING_WNDCLASS_NAME "HookingWndClass"
#define HOOKUI_PADDING 10

typedef struct {
	uint64_t address;
	void *hk_func;
	void **og_func;
	const char *name;
	char enabled;
	char created;
	int called_count;
	HWND called_count_label;
} Hook;

Hook *get_hooks(int *out_hooks_size);

int init_hooking(HINSTANCE hInstance);
void cleanup_hooking(HINSTANCE hInstance);

int add_hook(uint64_t address, void *hk_func, void **og_func, const char *name, char start_enabled, Hook **cb_hook);

void hook_called_callback(Hook *h);

void print_caller(void);

#endif // INCLUDE_HOOKING_H