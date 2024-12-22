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