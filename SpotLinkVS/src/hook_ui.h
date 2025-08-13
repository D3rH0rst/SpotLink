#pragma once

#include <hooking.h>

#define HOOKING_WNDCLASS_NAME TEXT("HookingWndClass")
#define HOOKUI_PADDING 10

int init_hookui(void);
int cleanup_hookui(void);

void hook_ui_hook_called_callback(Hook* h);