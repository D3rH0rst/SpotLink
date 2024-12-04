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
// if the play/pause button in the spotify UI is pressed, `1_pause_func` gets called by `2_pause_func`.
// however, if its played/paused by the media key, this function calls `1_pause_func`.
// this function gets called in both cases, resuming and pausing, unlike most of the other ones.
// It also gets called on the media keys for skip to next song and go back a song, however with a different second param
// (pause/resume => a2 = 0, forward => a2 = 6, backward => a2 = 7)
#define OFFSET_MEDIAKEY_CTRL_FUNC 0x15A1C4
//void __fastcall sub_14015A1C4(__int64 **a1, int a2)
extern void(__fastcall *og_mediakey_ctrl_func)(int64_t **a1, int a2);
void __fastcall hk_mediakey_ctrl_func(int64_t **a1, int a2);


// this function handles both pause, play, and forward, backwards. arguments and returns are always the exact same.
// it also gets called when clicking play on a new song, intersting.
// seeking also gets handled by it
#define OFFSET_3_PAUSE_PLAY_FUNC 0x4F1B50
//void *__fastcall sub_1404F1B50(_QWORD *a1, void *a2, size_t *a3, __int64 a4)
extern void*(__fastcall *og_3_pause_play_func)(uint64_t *a1, void *a2, size_t *a3, int64_t a4);
void* __fastcall hk_3_pause_play_func(uint64_t *a1, void *a2, size_t *a3, int64_t a4);

// also gets called on pause/play/seek, gets called multiple times on song switch,
// sometimes gets called randomly once
// arguments always the same
#define OFFSET_4_PAUSE_PLAY_FUNC 0xFD5CD4
//void *__fastcall sub_140FD5CD4(__int64 a1, void *a2, _QWORD *a3, __int64 a4)
extern void*(__fastcall *og_4_pause_play_func)(int64_t a1, void *a2, uint64_t *a3, int64_t a4);
void* __fastcall hk_4_pause_play_func(int64_t a1, void *a2, uint64_t *a3, int64_t a4);

// also get called on pause/play/seek, and multiple (7) times on song switch
// the third argument, a3, seems to be some sort of counter, gets incremented by one every time the function gets called
// gets called when the window is focused too, a bit annoying
#define OFFSET_5_PAUSE_PLAY_FUNC 0xBB2FBC
//char __fastcall sub_140BB2FBC(__int64 a1, __int64 a2, __int64 a3, char a4, __int64 a5)
extern char(__fastcall *og_5_pause_play_func)(int64_t a1, int64_t a2, int64_t a3, char a4, int64_t a5);
char __fastcall hk_5_pause_play_func(int64_t a1, int64_t a2, int64_t a3, char a4, int64_t a5);

//  this function always calls the above function, with the same counter and just slightly offset args
#define OFFSET_6_PAUSE_PLAY_FUNC 0xBB20E4
//__int64 __fastcall sub_140BB20E4(__int64 a1, __int64 a2, __int64 a3, char a4, _QWORD *a5)
extern int64_t(__fastcall *og_6_pause_play_func)(int64_t a1, int64_t a2, int64_t a3, char a4, uint64_t *a5);
int64_t __fastcall hk_6_pause_play_func(int64_t a1, int64_t a2, int64_t a3, char a4, uint64_t *a5);

#define OFFSET_7_PAUSE_PLAY_FUNC 0xBB2990
//__int64 __fastcall sub_140BB2990(__int64 a1)
extern int64_t(__fastcall *og_7_pause_play_func)(int64_t a1);
int64_t __fastcall hk_7_pause_play_func(int64_t a1);

// this function gets called a bunch of times on each track action, even tuning audio
// also gets called on media keys, interesting
// seems to be a general event handler, gets called on everyting i do
// first arg always the same, second arg differs.
#define OFFSET_8_PAUSE_PLAY_FUNC 0xCFD348
//__int64 __fastcall sub_140CFD348(__int64 a1, __int64 a2)
extern int64_t(__fastcall *og_8_pause_play_func)(int64_t a1, int64_t a2);
int64_t __fastcall hk_8_pause_play_func(int64_t a1, int64_t a2);


extern LONG is_critical; // needed as its a function with EnterCriticalSection
// looks like a message handler, gets called very often, on every event that happend (window move etc)
#define OFFSET_9_PAUSE_PLAY_FUNC 0x25145C
//__int64 __fastcall sub_14025145C(__int64 a1, DWORD a2, __int64 a3, __int64 a4)
extern int64_t(__fastcall *og_9_pause_play_func)(int64_t a1, uint32_t a2, int64_t a3, int64_t a4);
int64_t __fastcall hk_9_pause_play_func(int64_t a1, uint32_t a2, int64_t a3, int64_t a4);



