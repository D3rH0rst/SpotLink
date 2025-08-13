#include "hookfuncs.h"
#include "logging.h"
#include "hook_ui.h"
#include <intrin.h>

#define GUID_FORMAT TEXT("{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}")

BOOL show_spotify_log = FALSE;

DEFINE_HOOK(void, logging, int a1, int a2, int a3, const char* a4, ...) {
	hook_ui_hook_called_callback(hk_logging);

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
}

DEFINE_HOOK(uint64_t*, VPauseRequest, int64_t a1, uint64_t* a2, int64_t a3, int64_t* a4) {
	hook_ui_hook_called_callback(hk_VPauseRequest);

	log_sep();
	log_msg(LOG_INFO, "Called `int64_t VPauseRequest(0x%llX, 0x%llX, 0x%llX, 0x%llX)`", a1, a2, a3, a4);
	print_caller_addr(_ReturnAddress());
	
	uint64_t* ret = og_VPauseRequest_func(a1, a2, a3, a4);

	log_msg(LOG_INFO, "ret = 0x%llX", ret);
	log_sep();

	return ret;
}

DEFINE_HOOK(BOOL, PostQueuedCompletionStatus, HANDLE CompletionPort, DWORD dwNumBytes, ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped) {
	hook_ui_hook_called_callback(hk_PostQueuedCompletionStatus);

	log_sep();
	log_msg(LOG_INFO, "Called `bool PostQueuedCompletionStatus(0x%llX, 0x%X, 0x%llX, 0x%llX)`", CompletionPort, dwNumBytes, dwCompletionKey, lpOverlapped);
	print_caller_addr(_ReturnAddress());

	BOOL ret = og_PostQueuedCompletionStatus_func(CompletionPort, dwNumBytes, dwCompletionKey, lpOverlapped);

	log_msg(LOG_INFO, "ret = %d", ret);
	log_sep();

	return ret;
}

DEFINE_HOOK(char, pause, int64_t a1, char a2, int64_t a3) {
	hook_ui_hook_called_callback(hk_pause);

	log_sep();
	log_msg(LOG_INFO, "Called `char pause(0x%llX, %d, 0x%llX)`", a1, a2, a3);
	print_caller_addr(_ReturnAddress());

	char ret = og_pause_func(a1, a2, a3);

	log_msg(LOG_INFO, "ret = %d", ret);
	log_sep();

	return ret;
}

DEFINE_HOOK(void, event, int64_t a1, struct _OVERLAPPED* a2) {
	hook_ui_hook_called_callback(hk_pause);

	log_sep();
	log_msg(LOG_INFO, "Called `void event(0x%llX, 0x%llX)`", a1, a2);
	print_caller_addr(_ReturnAddress());

	og_event_func(a1, a2);
	log_sep();
}