#include "hookfuncs.h"
#include "logging.h"

#include <intrin.h>

#define GUID_FORMAT TEXT("{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}")

BOOL show_spotify_log = FALSE;


Hook* hk_logging = NULL;
void(*og_logging_func)(int a1, int a2, int a3, const char* a4, ...) = NULL;
void hk_logging_func(int a1, int a2, int a3, const char* a4, ...) {
	hook_called_callback(hk_logging);

	if (show_spotify_log) {
		va_list va;
		va_start(va, a4);

		char formatted_message[256];
		vsnprintf(formatted_message, sizeof(formatted_message), a4, va);

		log_sep();
		log_msg(LOG_INFO, "Spotify log: a1: %d, a2: %d, a3: %d | %s |", a1, a2, a3, formatted_message);
		log_sep();
		va_end(va);
	}

	//va_start(va, a4);
	//og_logging_func(a1, a2, a3, a4, va_arg(va, void*));
	//va_end(va);
}

Hook *hk_wndproc;
LRESULT(__fastcall *og_wndproc_func)(DWORD_PTR dwRefData, HWND *pHwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __fastcall hk_wndproc_func(DWORD_PTR dwRefData, HWND *pHwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	hook_called_callback(hk_wndproc);
	
	
	if (msg == 0xC028) {
		log_sep();
		log_msg(LOG_INFO, "Custom Message called");
		log_msg(LOG_INFO, "Called wndproc: (0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)", dwRefData, pHwnd, msg, wParam, lParam);
		print_caller_addr(_ReturnAddress());
	}
	return og_wndproc_func(dwRefData, pHwnd, msg, wParam, lParam);
}

Hook *hk_wnd_caller;
LRESULT(__fastcall *og_wnd_caller_func)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT __fastcall hk_wnd_caller_func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	hook_called_callback(hk_wnd_caller);
	if (uMsg == WM_COMMAND) {
		log_msg(LOG_INFO, "Called wndcaller with WM_COMMAND: (0x%llX, %d, 0x%llX, 0x%llX, 0x%llX, 0x%llX)", hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
		print_caller_addr(_ReturnAddress());
	}
	return og_wnd_caller_func(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
}


Hook *hk_alr_paused;
char *(*og_alr_paused_func)(void);
char *hk_alr_paused_func(void) {
	log_sep();
	log_msg(LOG_INFO, "Called hk_alr_paused()");
	print_caller_addr(_ReturnAddress());
	
	char *ret = og_alr_paused_func();

	return ret;
}

Hook *hk_dodo;
char(__fastcall *og_dodo_func)(uint64_t a1, uint64_t a2, char a3, uint64_t a4, uint64_t a5, uint64_t a6);
char __fastcall hk_dodo_func(uint64_t a1, uint64_t a2, char a3, uint64_t a4, uint64_t a5, uint64_t a6) {
	
	log_sep();
	log_msg(LOG_INFO, "Called hk_dodo_func(0x%llX, 0x%llX, %d, 0x%llX, 0x%llX, 0x%llX)", a1, a2, a3, a4, a5, a6);
	print_caller_addr(_ReturnAddress());
	char res = og_dodo_func(a1, a2, a3, a4, a5, a6);
	log_msg(LOG_INFO, "res: %d", res);
	
	return res;
}

Hook* hk_event = NULL;
void* (__fastcall* og_event_func)(uint64_t* a1, void* a2, size_t* a3, int64_t a4) = NULL;
void* __fastcall hk_event_func(uint64_t* a1, void* a2, size_t* a3, int64_t a4) {
	hook_called_callback(hk_event);
	log_sep();
	log_msg(LOG_INFO, "Called `char hk_event_func(0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4);
	print_caller();
	void* ret = og_event_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "ret = 0x%llX", (uint64_t)ret);
	log_sep();
	return a2;
}

Hook* hk_new_pause = NULL;
int64_t(__fastcall* og_new_pause_func)(int64_t** a1, int a2) = NULL;
int64_t __fastcall hk_new_pause_func(int64_t** a1, int a2) {

	hook_called_callback(hk_new_pause);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_new_pause_func(0x%llX, %d)`", a1, a2);
	print_caller();
	int64_t ret = og_new_pause_func(a1, a2);
	log_msg(LOG_INFO, "ret (void) = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook* hk_task_event = NULL;
void(__fastcall* og_task_event_func)(int64_t*** a1, int a2) = NULL;
void __fastcall hk_task_event_func(int64_t*** a1, int a2) {
	hook_called_callback(hk_new_pause);
	log_sep();
	log_msg(LOG_INFO, "Called `void hk_task_event_func(0x%llX, %d)`", a1, a2);
	print_caller();
	og_task_event_func(a1, a2);
	//log_msg(LOG_INFO, "ret (void) = 0x%llX", ret);
	log_sep();
	//return ret;
}

Hook* hk_new_song = NULL;
int64_t(__fastcall* og_new_song_func)(int64_t a1, uint64_t* a2, int64_t a3, int64_t a4, int64_t a5) = NULL;
int64_t __fastcall hk_new_song_func(int64_t a1, uint64_t* a2, int64_t a3, int64_t a4, int64_t a5) {
	hook_called_callback(hk_new_song);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_new_son_func(0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4, a5);
	print_caller();
	const char* v10 = (const char*)a2;
	if ((uint64_t)a2[3] >= 0x10)
		v10 = *(const char**)a2;
	log_msg(LOG_INFO, "Play Song, ID: %s", v10);
	//show_spotify_log = TRUE;
	int64_t ret = og_new_song_func(a1, a2, a3, a4, a5);
	//show_spotify_log = FALSE;

	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();

	return ret;
}



Hook* hk_pause = NULL;
int64_t(__fastcall* og_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
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

Hook* hk_play = NULL;
int64_t(__fastcall* og_play_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
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

Hook* hk_next = NULL;
int64_t(__fastcall* og_next_func)(int64_t a1, char* a2, int64_t a3) = NULL;
int64_t __fastcall hk_next_func(int64_t a1, char* a2, int64_t a3) {
	hook_called_callback(hk_next);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_next_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_next_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook* hk_prev = NULL;
int64_t(__fastcall* og_prev_func)(int64_t a1, BYTE* a2, int64_t a3) = NULL;
int64_t __fastcall hk_prev_func(int64_t a1, BYTE* a2, int64_t a3) {
	hook_called_callback(hk_prev);
	log_sep();
	log_msg(LOG_INFO, "Called `int64_t hk_prev_func(0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3);
	print_caller();
	int64_t ret = og_prev_func(a1, a2, a3);
	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();
	return ret;
}

Hook* hk_seek = NULL;
int64_t(__fastcall* og_seek_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
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

Hook* hk_song = NULL;
const GUID* (__fastcall* og_song_func)(int64_t a1, const GUID* a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) = NULL;
const GUID* __fastcall hk_song_func(int64_t a1, const  GUID* a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6) {
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

Hook* hk_repeat = NULL;
int64_t(__fastcall* og_repeat_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
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

Hook* hk_shuffle1 = NULL;
int64_t(__fastcall* og_shuffle1_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
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

Hook* hk_shuffle2 = NULL;
int64_t(__fastcall* og_shuffle2_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4) = NULL;
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
