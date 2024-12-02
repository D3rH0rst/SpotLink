#include <stdint.h>
#include <Windows.h>
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

#define OFFSET_TOGGLE_PAUSE_FUNC 0x58E5A8
//__int64 __fastcall sub_14058E5A8(__int64 a1, ULONG_PTR a2, __int64 a3)
extern int64_t(__fastcall *og_toggle_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3);
int64_t __fastcall hk_toggle_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3);

#define OFFSET_TOGGLE_PLAY_FUNC 0x590154
//__int64 __fastcall sub_140590154(__int64 a1, ULONG_PTR a2, __int64 a3)
extern int64_t(__fastcall *og_toggle_play_func)(int64_t a1, ULONG_PTR a2, int64_t a3);
int64_t __fastcall hk_toggle_play_func(int64_t a1, ULONG_PTR a2, int64_t a3);

#define OFFSET_SET_PAUSE_FUNC 0x5A098C
//__int64 __fastcall sub_1405A098C(__int64 a1, ULONG_PTR a2, __int64 a3)
extern int64_t(__fastcall *og_set_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3);
int64_t __fastcall hk_set_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3);

#define OFFSET_SET_PLAY_FUNC 0x5A0F68
//__int64 __fastcall sub_1405A0F68(__int64 a1, _QWORD *a2, __int64 a3)
extern int64_t(__fastcall *og_set_play_func)(int64_t a1, uint64_t *a2, int64_t a3);
int64_t __fastcall hk_set_play_func(int64_t a1, uint64_t *a2, int64_t a3);

#define OFFSET_NEXT_PAUSE_FUNC 0x2953F4
//__int64 __fastcall sub_1402953F4(__int64 a1, __int64 a2, __int64 a3)
extern int64_t(__fastcall *og_next_pause_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_next_pause_func(int64_t a1, int64_t a2, int64_t a3);

#define OFFSET_NEXT_PLAY_FUNC 0x2969E4
//__int64 __fastcall sub_1402969E4(__int64 a1, __int64 a2, __int64 a3)
extern int64_t(__fastcall *og_next_play_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_next_play_func(int64_t a1, int64_t a2, int64_t a3);

#define OFFSET_1_PAUSE_FUNC 0x5A0A14
//__int64 __fastcall sub_1405A0A14(__int64 a1, _QWORD *a2)
extern int64_t(__fastcall *og_1_pause_func)(int64_t a1, uint64_t *a2, int64_t a3);
int64_t __fastcall hk_1_pause_func(int64_t a1, uint64_t *a2, int64_t a3);

#define OFFSET_1_PLAY_FUNC 0x5A0FF0
//__int64 __fastcall sub_1405A0FF0(__int64 a1, _QWORD *a2)
extern int64_t(__fastcall *og_1_play_func)(int64_t a1, uint64_t *a2, int64_t a3);
int64_t __fastcall hk_1_play_func(int64_t a1, uint64_t *a2, int64_t a3);

#define OFFSET_2_PAUSE_FUNC 0x4EB704
//__int64 *__fastcall sub_1404EB704(__int64 a1, __int64 *a2, __int64 a3, __int64 *a4)
extern int64_t*(__fastcall *og_2_pause_func)(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4);
int64_t* __fastcall hk_2_pause_func(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4);

#define OFFSET_2_PLAY_FUNC 0x4EC168
//__int64 *__fastcall sub_1404EC168(__int64 a1, __int64 *a2, __int64 a3, __int64 *a4)
extern int64_t*(__fastcall *og_2_play_func)(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4);
int64_t* __fastcall hk_2_play_func(int64_t a1, int64_t *a2, int64_t a3, int64_t *a4);

// this function is interesting,
// if the play/pause button in the spotify UI is pressed, 1_pause_func gets called by 2_pause_func.
// however, if its played/paused by the media key, this function gets called.
// this function gets called in both cases, resuming and pausing, unlike most of the other ones.
#define OFFSET_MEDIA_PAUSE_PLAY_FUNC 0x15A1C4
//void __fastcall sub_14015A1C4(__int64 **a1, int a2)
extern void(__fastcall *og_media_pause_play_func)(int64_t **a1, int a2);
void __fastcall hk_media_pause_play_func(int64_t **a1, int a2);

