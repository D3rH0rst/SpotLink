#include "hooks.h"
#include "logging.h"

#include <DbgHelp.h>

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

char is_critical = 0;
int64_t(__fastcall *og_9_pause_play_func)(int64_t a1, uint32_t a2, int64_t a3, int64_t a4) = NULL;
int64_t __fastcall hk_9_pause_play_func(int64_t a1, uint32_t a2, int64_t a3, int64_t a4) {
	//InterlockedExchange(&is_critical, 1);
	while (is_critical);
	int64_t ret = og_9_pause_play_func(a1, a2, a3, a4);
	//InterlockedExchange(&is_critical, 0);
	log_msg(LOG_INFO, "Called hk_9_pause_play_func(0x%llx, %ld, 0x%llx, 0x%llx), ret: 0x%llx", a1, a2, a3, a4, ret);
	return ret;
}

void(__fastcall *og_2_media_func)(int64_t ***a1, int a2) = NULL;
void __fastcall hk_2_media_func(int64_t ***a1, int a2) {
	log_msg(LOG_INFO, "Called hk_2_media_func(0x%llx, %d)", a1, a2);
	return og_2_media_func(a1, a2);
}

char in_3_media_func = 0;
void(__fastcall *og_3_media_func)(int64_t a1) = NULL;
void __fastcall hk_3_media_func(int64_t a1) {
	in_3_media_func = 1;
	/*return*/ og_3_media_func(a1);
	log_msg(LOG_INFO, "Called hk_3_media_func(0x%llx) (v1 = %ld)", a1, *(uint32_t*)(a1 + 16));
}

void PrintCallStack(void);

int64_t(__fastcall *og_4_media_func)(int64_t a1) = NULL;
int64_t __fastcall hk_4_media_func(int64_t a1) {
	if ((uint64_t)(**(void***)(*(uint64_t*)(a1 - 16))) - spotify_base == 0xCFE0BC) { // interesting offset
		int64_t v1; // rbx
		void(__fastcall * **v2)(uint64_t); // rcx
		int64_t result; // rax
		int64_t result2;
		int64_t v4; // rcx
		int64_t retval;
		log_sep();

		void* callee[1];
		unsigned short frames = CaptureStackBackTrace(0, 1, callee, NULL);  // Capture the function caller

		log_msg(LOG_INFO, "In og_4_media_func call.");
		for (int i = 0; i < frames; i++) {
			DWORD64 addr = (DWORD64)(callee[i]);
			// Resolve the symbol for the address
			//SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME, 1);
			//symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			//symbol->MaxNameLen = MAX_SYM_NAME;

			//if (SymFromAddr(GetCurrentProcess(), addr, 0, symbol)) {
			//	log_msg(LOG_INFO, "    0x%p - %s\n", addr, symbol->Name);
			//}
			//else {
			//	log_msg(LOG_INFO, "    0x%p - Symbol not found\n", addr);
			//}
			//free(symbol);
			log_msg(LOG_INFO, "Called by `sub_18%.7llX` (0x%llX)", addr - libcef_base, addr);
		}


		log_msg(LOG_INFO, "a1: 0x%llx", a1);
		if (a1) {
			v1 = *(uint64_t*)(a1 - 16);
			log_msg(LOG_INFO, "v1: 0x%llx", v1);
			if (v1) {
				v2 = (void(__fastcall ***)(uint64_t))(v1 + *(int *)(*(uint64_t*)(v1 + 8) + 4) + 8);
				log_msg(LOG_INFO, "v2: 0x%llx", v2);
				log_msg(LOG_INFO, "Calling function `sub_14%.7llX(0x%llx)`", (uint64_t)(**v2) - spotify_base, v2);
				(**v2)((uint64_t)v2);
			}
			log_msg(LOG_INFO, "v1: 0x%llx", v1);
			log_msg(LOG_INFO, "argtest: *(uint64_t*)(v1 + 72) = 0x%llx", *(uint64_t*)(v1 + 72));
			log_msg(LOG_INFO, "Calling function `sub_14%.7llX(0x%llx)`...", (uint64_t)(**(void***)v1) - spotify_base, v1);
			//log_msg(LOG_INFO, "Calling function `(**(int64_t(__fastcall ***)(int64_t))v1)(v1)`...");
			result = (**(int64_t(__fastcall ***)(int64_t))v1)(v1);// calls the media key function
			retval = result;
			log_msg(LOG_INFO, "result: 0x%llx", result);
			log_msg(LOG_INFO, "v1: 0x%llx", v1);
			if (v1) {
				v4 = v1 + *(int*)(*(uint64_t*)(v1 + 8) + 4) + 8;
				log_msg(LOG_INFO, "v4: 0x%llx", v4);
				log_msg(LOG_INFO, "Calling function `sub_14%.7llX(0x%llx)`", (uint64_t)(*(void**)(*(uint64_t*)v4 + 8)) - spotify_base, v4);
				//log_msg(LOG_INFO, "Calling function `(*(int64_t(__fastcall **)(int64_t))(*(uint64_t*)v4 + 8))(v4)`...");
				result2 = (*(int64_t(__fastcall **)(int64_t))(*(uint64_t*)v4 + 8))(v4);
				retval = result2;
				log_msg(LOG_INFO, "result: 0x%llx", result2);
			}
		}
		log_msg(LOG_INFO, "return value: 0x%llx", retval);

		log_sep();

		return retval;
	}

	return og_4_media_func(a1);
}

void PrintCallStack(void) {
	void* stack[5];  // Array to hold the call stack frames
	unsigned short frames = CaptureStackBackTrace(0, 5, stack, NULL);  // Capture the stack

	// Print out each stack frame
	for (int i = 0; i < frames; i++) {
		DWORD64 addr = (DWORD64)(stack[i]);
		// Resolve the symbol for the address
		SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME, 1);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		if (SymFromAddr(GetCurrentProcess(), addr, 0, symbol)) {
			log_msg(LOG_INFO, "    0x%p - %s\n", addr, symbol->Name);
		}
		else {
			log_msg(LOG_INFO, "    0x%p - Symbol not found\n", addr);
		}
		free(symbol);
	}
}