#pragma once

#include <tchar.h>
#include <inttypes.h>

typedef struct {
	char arg_count;
	TCHAR* called_str;
	int  stackspace;
	void* og_func;
} RuntimeHook;

typedef struct Hook {
	uint64_t address;
	void* hk_func;
	void** og_func;
	TCHAR name[100];
	char enabled;
	char created;
	int called_count;
	void *hook_data;
	//HWND called_count_label;
	RuntimeHook* runtime_hook;
} Hook;