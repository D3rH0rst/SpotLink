#pragma once

#include "./Accordion/accordionctrl.h"

#include <Windows.h>

typedef void(*CCTRL_LOG)(const TCHAR*, ...);

int CustomControlsInit(HINSTANCE hInstance, CCTRL_LOG log_function);
int CustomControlsUnInit(HINSTANCE hInstance);
