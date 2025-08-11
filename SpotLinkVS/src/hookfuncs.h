#pragma once

#include <Windows.h>
#include <inttypes.h>
#include "hooking.h"

#define DECLARE_HOOK(ret_type, name, ...) \
extern Hook *hk_##name; \
extern ret_type(*og_##name##_func)(__VA_ARGS__); \
ret_type hk_##name##_func(__VA_ARGS__);

#define DEFINE_HOOK(ret_type, name, ...) \
Hook* hk_##name = NULL; \
ret_type(*og_##name##_func)(__VA_ARGS__) = NULL; \
ret_type hk_##name##_func(__VA_ARGS__)


extern BOOL show_spotify_log;
#define SIG_LOGGING_FUNC "4C 8B DC 4D 89 4B 20 53 56"
DECLARE_HOOK(void, logging, int, int, int, const char*, ...);

#define OFF_VPAUSEREQUEST 0x594BA4
// _QWORD *__fastcall sub_140594BA4(__int64 a1, _QWORD *a2, __int64 a3, __int64 *a4)
DECLARE_HOOK(uint64_t*, VPauseRequest, int64_t, uint64_t*, int64_t, int64_t*);



