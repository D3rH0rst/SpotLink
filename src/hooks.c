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

int64_t(__fastcall *og_toggle_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
int64_t __fastcall hk_toggle_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3) {
	int64_t ret = og_toggle_pause_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_toggle_pause_func(0x%llx, 0x%lx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_toggle_play_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
int64_t __fastcall hk_toggle_play_func(int64_t a1, ULONG_PTR a2, int64_t a3) {
	int64_t ret = og_toggle_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_toggle_play_func(0x%llx, 0x%lx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_set_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3) = NULL;
int64_t __fastcall hk_set_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3) {
	int64_t ret = og_set_pause_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_set_pause_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;

	// this function calls an important getter function, argument a2 is gotten like this:
	/*
	`char v9[224];`
	`__int64 v7 = sub_1411B13CC(v9);`
	`v7` is then the second argument to this function call 
	*/
}

int64_t(__fastcall *og_set_play_func)(int64_t a1, uint64_t *a2, int64_t a3) = NULL;
int64_t __fastcall hk_set_play_func(int64_t a1, uint64_t *a2, int64_t a3) {
	int64_t ret = og_set_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_set_play_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_next_pause_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_next_pause_func(int64_t a1, int64_t a2, int64_t a3) {
	int64_t ret = og_next_pause_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_next_pause_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_next_play_func)(int64_t a1, int64_t a2, int64_t a3) = NULL;
int64_t __fastcall hk_next_play_func(int64_t a1, int64_t a2, int64_t a3) {
	int64_t ret = og_next_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_next_play_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_1_pause_func)(int64_t a1, uint64_t *a2, int64_t a3) = NULL;
int64_t __fastcall hk_1_pause_func(int64_t a1, uint64_t *a2, int64_t a3) {
	int64_t ret = og_1_pause_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_1_pause_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t(__fastcall *og_1_play_func)(int64_t a1, uint64_t *a2, int64_t a3) = NULL;
int64_t __fastcall hk_1_play_func(int64_t a1, uint64_t *a2, int64_t a3) {
	int64_t ret = og_1_play_func(a1, a2, a3);
	log_msg(LOG_INFO, "Called hk_1_play_func(0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, ret);
	return ret;
}

int64_t*(__fastcall *og_2_pause_func)(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4) = NULL;
int64_t *__fastcall hk_2_pause_func(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4) {
	int64_t *ret = og_2_pause_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "Called hk_2_pause_func(0x%llx, 0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

int64_t*(__fastcall *og_2_play_func)(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4) = NULL;
int64_t *__fastcall hk_2_play_func(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4) {
	int64_t *ret = og_2_play_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "Called hk_2_play_func(0x%llx, 0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

void(__fastcall *og_mediakey_ctrl_func)(int64_t **a1, int a2) = NULL;
void __fastcall hk_mediakey_ctrl_func(int64_t **a1, int a2) {
	log_msg(LOG_INFO, "Called hk_mediakey_ctrl_func(0x%llx, %d)", a1, a2);
	return og_mediakey_ctrl_func(a1, a2);
}

void*(__fastcall *og_3_pause_play_func)(uint64_t *a1, void *a2, size_t *a3, int64_t a4) = NULL;
void* __fastcall hk_3_pause_play_func(uint64_t *a1, void *a2, size_t *a3, int64_t a4) {
	void *ret = og_3_pause_play_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "Called hk_3_pause_play_func(0x%llx, 0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

void*(__fastcall *og_4_pause_play_func)(int64_t a1, void *a2, uint64_t *a3, int64_t a4) = NULL;
void* __fastcall hk_4_pause_play_func(int64_t a1, void *a2, uint64_t *a3, int64_t a4) {
	void *ret = og_4_pause_play_func(a1, a2, a3, a4);
	log_msg(LOG_INFO, "Called hk_4_pause_play_func(0x%llx, 0x%llx, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

char(__fastcall *og_5_pause_play_func)(int64_t a1, int64_t a2, int64_t a3, char a4, int64_t a5) = NULL;
char __fastcall hk_5_pause_play_func(int64_t a1, int64_t a2, int64_t a3, char a4, int64_t a5) {
	char ret = og_5_pause_play_func(a1, a2, a3, a4, a5);
	log_msg(LOG_INFO, "Called hk_5_pause_play_func(0x%llx, 0x%llx, 0x%llx, %d, 0x%llx), ret: %d", a1, a2, a3, a4, a5, ret);
	return ret;
}

int64_t(__fastcall *og_6_pause_play_func)(int64_t a1, int64_t a2, int64_t a3, char a4, uint64_t *a5) = NULL;
int64_t __fastcall hk_6_pause_play_func(int64_t a1, int64_t a2, int64_t a3, char a4, uint64_t *a5) {
	int64_t ret = og_6_pause_play_func(a1, a2, a3, a4, a5);
	log_msg(LOG_INFO, "Called hk_6_pause_play_func(0x%llx, 0x%llx, 0x%llx, %d, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, a5, ret);
	return ret;
}

int64_t(__fastcall *og_7_pause_play_func)(int64_t a1) = NULL;
int64_t __fastcall hk_7_pause_play_func(int64_t a1) {
	int64_t ret = og_7_pause_play_func(a1);
	log_msg(LOG_INFO, "Called hk_7_pause_play_func(0x%llx), ret: 0x%llx", a1, ret);
	return ret;
}

int64_t(__fastcall *og_8_pause_play_func)(int64_t a1, int64_t a2) = NULL;
int64_t __fastcall hk_8_pause_play_func(int64_t a1, int64_t a2) {
	int64_t ret = og_8_pause_play_func(a1, a2);
	log_msg(LOG_INFO, "Called hk_8_pause_play_func(0x%llx, 0x%llx), ret: 0x%llx", a1, a2, ret);
	return ret;
}

LONG is_critical = 0;
int64_t(__fastcall *og_9_pause_play_func)(int64_t a1, uint32_t a2, int64_t a3, int64_t a4) = NULL;
int64_t __fastcall hk_9_pause_play_func(int64_t a1, uint32_t a2, int64_t a3, int64_t a4) {
	InterlockedExchange(&is_critical, 1);
	int64_t ret = og_9_pause_play_func(a1, a2, a3, a4);
	InterlockedExchange(&is_critical, 0);
	log_msg(LOG_INFO, "Called hk_9_pause_play_func(0x%llx, %ld, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

