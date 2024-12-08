#include "hooks.h"
#include "logging.h"

#include <DbgHelp.h>


void print_caller(void) {
	uint64_t caller;
	uint16_t frame = CaptureStackBackTrace(2, 1, (void**)&caller, NULL);

	if (frame) {
		HMODULE m;
		char module_name[MAX_PATH];
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)caller, &m);
		GetModuleFileName(m, module_name, sizeof(module_name));
		log_msg(LOG_INFO, "Function caller: %s!0x%llX (OFF: 0x%llX)", strrchr(module_name, '\\') + 1, caller, caller - (uint64_t)m);
	}
}


void***(*og_esperanto_pause)(void) = NULL;
void*** hk_esperanto_pause(void) {
	log_sep();
	log_msg(LOG_INFO, "Called void `***hk_esperanto_pause(void)`");
	void*** ret = og_esperanto_pause();
	log_msg(LOG_INFO, "ret = 0x%llX; *ret = 0x%llX, **ret = 0x%llX", ret, *ret, **ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_track_skipped)(int64_t a1, int64_t a2) = NULL;
int64_t __fastcall hk_track_skipped(int64_t a1, int64_t a2) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_track_skipped(0x%llX, 0x%llX)`", a1, a2);
	print_caller();
	int64_t ret = og_track_skipped(a1, a2);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_skip_caller)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, uint32_t a8) = NULL;
int64_t __fastcall hk_skip_caller(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, uint32_t a8) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_skip_caller(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, %d)`", a1, a2, a3, a4, a5, a6, a7, a8);
	print_caller();
	int64_t ret = og_skip_caller(a1, a2, a3, a4, a5, a6, a7, a8);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

void(__fastcall *og_did_pause)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) = NULL;
void __fastcall hk_did_pause(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_did_pause(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6);
	print_caller();
	og_did_pause(a1, a2, a3, a4, a5, a6);
	log_sep();	
}

int64_t(__fastcall *og_pause_caller)(int64_t a1, int64_t a2, uint8_t *a3) = NULL;
int64_t __fastcall hk_pause_caller(int64_t a1, int64_t a2, uint8_t *a3) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_pause_caller(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	log_msg(LOG_INFO, "*a3 = %d", *a3);
	print_caller();
	int64_t ret = og_pause_caller(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_2_pause)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_2_pause(int64_t a1, int64_t a2, int64_t a3) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_2_pause(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_2_pause(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_3_pause)(int64_t a1, int64_t a2, ULONG_PTR a3) = NULL;
int64_t __fastcall hk_3_pause(int64_t a1, int64_t a2, ULONG_PTR a3) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_3_pause(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_3_pause(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_4_pause)(int64_t a1, int64_t a2) = NULL;
int64_t __fastcall hk_4_pause(int64_t a1, int64_t a2) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_4_pause(0x%llX, 0x%llX)`", a1, a2);
	print_caller();
	int64_t ret = og_4_pause(a1, a2);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_5_pause)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_5_pause(int64_t a1, int64_t a2, int64_t a3) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_5_pause(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_5_pause(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}


// sample functions


int64_t(__fastcall *og_0_arg)(void) = NULL;
int64_t __fastcall hk_0_arg(void) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_0_arg(void)`");
	print_caller();
	int64_t ret = og_0_arg();
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_1_arg)(int64_t a1) = NULL;
int64_t __fastcall hk_1_arg(int64_t a1) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_1_arg(0x%llX)`", a1);
	print_caller();
	int64_t ret = og_1_arg(a1);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_2_arg)(int64_t a1, int64_t a2) = NULL;
int64_t __fastcall hk_2_arg(int64_t a1, int64_t a2) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_2_arg(0x%llX, 0x%llX)`", a1, a2);
	print_caller();
	int64_t ret = og_2_arg(a1, a2);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_3_arg)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_3_arg(int64_t a1, int64_t a2, int64_t a3) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_2_arg(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_3_arg(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_4_arg)(int64_t a1, int64_t a2, int64_t a3, int64_t a4) = NULL;
int64_t __fastcall hk_4_arg(int64_t a1, int64_t a2, int64_t a3, int64_t a4) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_4_arg(0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4);
	print_caller();
	int64_t ret = og_4_arg(a1, a2, a3, a4);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_5_arg)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5) = NULL;
int64_t __fastcall hk_5_arg(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_5_arg(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5);
	print_caller();
	int64_t ret = og_5_arg(a1, a2, a3, a4, a5);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_6_arg)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) = NULL;
int64_t __fastcall hk_6_arg(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_6_arg(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6);
	print_caller();
	int64_t ret = og_6_arg(a1, a2, a3, a4, a5, a6);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_7_arg)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7) = NULL;
int64_t __fastcall hk_7_arg(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_7_arg(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6, a7);
	print_caller();
	int64_t ret = og_7_arg(a1, a2, a3, a4, a5, a6, a7);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

int64_t(__fastcall *og_8_arg)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8) = NULL;
int64_t __fastcall hk_8_arg(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8) {
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_8_arg(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6, a7, a8);
	print_caller();
	int64_t ret = og_8_arg(a1, a2, a3, a4, a5, a6, a7, a8);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}