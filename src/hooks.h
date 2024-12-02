#include <stdint.h>

typedef struct {
	uint64_t address;
	void *hk_func;
	void **og_func;
	char *name;
	char enabled;
	char created;
} Hook;


#define OFFSET_PAUSERESUME_FUNC 0x1B33C8
//char __fastcall sub_1401B33C8(__int64 a1, unsigned __int8 a2, __int64 a3)
extern int8_t(__fastcall *og_pauseresume_func)(int64_t a1, uint8_t a2, int64_t a3);
int8_t __fastcall hk_pauseresume_func(int64_t a1, uint8_t a2, int64_t a3);

#define OFFSET_PAUSE_FUNC 0x799FF8
//void __fastcall sub_140799FF8(__int64 a1, int a2, void **a3)
extern void(__fastcall *og_pause_func)(int64_t a1, int a2, void **a3);
void __fastcall hk_pause_func(int64_t a1, int a2, void **a3);

#define OFFSET_RESUME_FUNC 0x79BDCC
//void __fastcall sub_14079BDCC(__int64 a1, int a2, void **a3)
extern void(__fastcall *og_resume_func)(int64_t a1, int a2, void **a3);
void __fastcall hk_resume_func(int64_t a1, int a2, void **a3);

#define OFFSET_PLAY_FUNC 0x58B66C
//__int64 __fastcall sub_14058B66C(__int64 a1, __int64 a2, __int64 a3)
extern int64_t(__fastcall *og_play_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_play_func(int64_t a1, int64_t a2, int64_t a3);