#include "hookfuncs.h"
#include "logging.h"

#define GUID_FORMAT "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"

Hook *hk_pause = NULL;
int64_t(__fastcall *og_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
int64_t __fastcall hk_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3) {
	hook_called_callback(hk_pause);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_pause_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_pause_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_play = NULL;
int64_t(__fastcall *og_play_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
int64_t __fastcall hk_play_func(int64_t a1, ULONG_PTR a2, int64_t a3) {
	hook_called_callback(hk_play);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_play_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_next = NULL;
int64_t(__fastcall *og_next_func)(int64_t a1, char *a2, int64_t a3) = NULL;
int64_t __fastcall hk_next_func(int64_t a1, char *a2, int64_t a3) {
	hook_called_callback(hk_next);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_next_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_next_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_prev = NULL;
int64_t(__fastcall *og_prev_func)(int64_t a1, BYTE *a2, int64_t a3) = NULL;
int64_t __fastcall hk_prev_func(int64_t a1, BYTE *a2, int64_t a3) {
	hook_called_callback(hk_prev);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_prev_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_prev_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_seek = NULL;
int64_t(__fastcall *og_seek_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_seek_func(int64_t a1, int64_t a2, int64_t a3) {
	hook_called_callback(hk_seek);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_seek_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_seek_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_song = NULL;
const GUID*(__fastcall *og_song_func)(int64_t a1, const GUID *a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) = NULL;
const GUID* __fastcall hk_song_func(int64_t a1, const  GUID *a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
	hook_called_callback(hk_song);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_song_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6);

	log_msg(LOG_INFO, "song func GUID: "GUID_FORMAT, a2->Data1, a2->Data2, a2->Data3,
		a2->Data4[0], a2->Data4[1], a2->Data4[2], a2->Data4[3],
		a2->Data4[4], a2->Data4[5], a2->Data4[6], a2->Data4[7]);
	print_caller();
	const GUID* ret = og_song_func(a1, a2, a3, a4, a5, a6);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_msg(LOG_INFO, "ret GUID: "GUID_FORMAT, ret->Data1, ret->Data2, ret->Data3,
		ret->Data4[0], ret->Data4[1], ret->Data4[2], ret->Data4[3],
		ret->Data4[4], ret->Data4[5], ret->Data4[6], ret->Data4[7]);
	log_sep();
	return ret;
}

Hook *hk_repeat = NULL;
int64_t(__fastcall *og_repeat_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_repeat_func(int64_t a1, int64_t a2, int64_t a3) {
	hook_called_callback(hk_repeat);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_repeat_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_repeat_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_shuffle1 = NULL;
int64_t(__fastcall *og_shuffle1_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_shuffle1_func(int64_t a1, int64_t a2, int64_t a3) {
	hook_called_callback(hk_shuffle1);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_shuffle1_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_shuffle1_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook *hk_shuffle2 = NULL;
int64_t(__fastcall *og_shuffle2_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4) = NULL;
int64_t __fastcall hk_shuffle2_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4) {
	hook_called_callback(hk_shuffle2);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_shuffle2_func(0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4);
	print_caller();
	int64_t ret = og_shuffle2_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

// ----------------------------- SAMPLE HOOK FUNCS FOR REFERENCE IN IDA ----------------------------- //


// 0 args
Hook *hk_0 = NULL;
int64_t(__fastcall *og_0_func)(void) = NULL;
int64_t __fastcall hk_0_func(void) {
	hook_called_callback(hk_0);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_0_func()`");
	print_caller();
	int64_t ret = og_0_func();
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 1 arg
Hook *hk_1 = NULL;
int64_t(__fastcall *og_1_func)(int64_t a1) = NULL;
int64_t __fastcall hk_1_func(int64_t a1) {
	hook_called_callback(hk_1);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_1_func(0x%llX)`", a1);
	print_caller();
	int64_t ret = og_1_func(a1);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 2 args
Hook *hk_2 = NULL;
int64_t(__fastcall *og_2_func)(int64_t a1, int64_t a2) = NULL;
int64_t __fastcall hk_2_func(int64_t a1, int64_t a2) {
	hook_called_callback(hk_2);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_2_func(0x%llX, 0x%llX)`", a1, a2);
	print_caller();
	int64_t ret = og_2_func(a1, a2);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 3 args
Hook *hk_3 = NULL;
int64_t(__fastcall *og_3_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_3_func(int64_t a1, int64_t a2, int64_t a3) {
	hook_called_callback(hk_3);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_3_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_3_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 4 args
Hook *hk_4 = NULL;
int64_t(__fastcall *og_4_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4) = NULL;
int64_t __fastcall hk_4_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4) {
	hook_called_callback(hk_4);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_4_func(0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4);
	print_caller();
	int64_t ret = og_4_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 5 args
Hook *hk_5 = NULL;
int64_t(__fastcall *og_5_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5) = NULL;
int64_t __fastcall hk_5_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5) {
	hook_called_callback(hk_5);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_5_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5);
	print_caller();
	int64_t ret = og_5_func(a1, a2, a3, a4, a5);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 6 args
Hook *hk_6 = NULL;
int64_t(__fastcall *og_6_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) = NULL;
int64_t __fastcall hk_6_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
	hook_called_callback(hk_6);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_6_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6);
	print_caller();
	int64_t ret = og_6_func(a1, a2, a3, a4, a5, a6);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 7 args
Hook *hk_7 = NULL;
int64_t(__fastcall *og_7_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7) = NULL;
int64_t __fastcall hk_7_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7) {
	hook_called_callback(hk_7);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_7_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6, a7);
	print_caller();
	int64_t ret = og_7_func(a1, a2, a3, a4, a5, a6, a7);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 8 args
Hook *hk_8 = NULL;
int64_t(__fastcall *og_8_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8) = NULL;
int64_t __fastcall hk_8_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8) {
	hook_called_callback(hk_8);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_8_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6, a7, a8);
	print_caller();
	int64_t ret = og_8_func(a1, a2, a3, a4, a5, a6, a7, a8);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}

// 9 args
Hook *hk_9 = NULL;
int64_t(__fastcall *og_9_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8, int64_t a9) = NULL;
int64_t __fastcall hk_9_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, int64_t a8, int64_t a9) {
	hook_called_callback(hk_9);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_9_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5, a6, a7, a8, a9);
	print_caller();
	int64_t ret = og_9_func(a1, a2, a3, a4, a5, a6, a7, a8, a9);
	log_msg(LOG_INFO, "ret = 0xllX", ret);
	log_sep();
	return ret;
}