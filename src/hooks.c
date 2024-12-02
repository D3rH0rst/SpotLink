#include "hooks.h"
#include "logging.h"

int8_t(__fastcall *og_pauseresume_func)(int64_t a1, uint8_t a2, int64_t a3) = NULL;
int8_t __fastcall hk_pauseresume_func(int64_t a1, uint8_t a2, int64_t a3) {
	// this function gets called on pause and resume, hk_pauseresume calls it
	int8_t ret = og_pauseresume_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_pauseresume_func(0x%llx, %d, 0x%llx), ret: %d", a1, a2, a3, ret);
	return ret;
}

void(__fastcall *og_pause_func)(int64_t a1, int a2, void **a3) = NULL;
void __fastcall hk_pause_func(int64_t a1, int a2, void **a3) {
	// this function only gets called on PAUSE
	log_msg(LOG_INFO, "Called hk_pause_func(0x%llx, %d, 0x%llx)", a1, a2, a3);
	return og_pause_func(a1, a2, a3);
}

void(__fastcall *og_resume_func)(int64_t a1, int a2, void **a3) = NULL;
void __fastcall hk_resume_func(int64_t a1, int a2, void **a3) {
	// this function only gets called on RESUME
	log_msg(LOG_INFO, "Called hk_resume_func(0x%llx, %d, 0x%llx)", a1, a2, a3);
	return og_resume_func(a1, a2, a3);
}

int64_t(__fastcall *og_play_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_play_func(int64_t a1, int64_t a2, int64_t a3) {
	int64_t ret = og_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_play_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}