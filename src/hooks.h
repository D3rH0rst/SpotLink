#ifndef INCLUDE_HOOKS_H
#define INCLUDE_HOOKS_H

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

extern uint64_t spotify_base;
extern uint64_t libcef_base;


// Spotify.exe got updated, so all old hooks are worthless

#define OFFSET_ESPERANTO_PAUSE 0x4E8820
//void ***sub_1404E8820()
extern void***(*og_esperanto_pause)(void);
void*** hk_esperanto_pause(void);

#define OFFSET_TRACK_SKIPPED 0x513A80
//__int64 __fastcall sub_140513A80(__int64 a1, __int64 a2)
extern int64_t(__fastcall *og_track_skipped)(int64_t a1, int64_t a2);
int64_t __fastcall hk_track_skipped(int64_t a1, int64_t a2);

#define OFFSET_SKIP_CALLER 0x4F6AEC
//__int64 __fastcall sub_1404F6AEC(__int64 a1,__int64 a2,__int64 a3,__int64 a4,__int64 a5,__int64 a6,__int64 a7,_DWORD *a8)
extern int64_t(__fastcall *og_skip_caller)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, uint32_t a8);
int64_t __fastcall hk_skip_caller(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6, int64_t a7, uint32_t a8);

#define OFFSET_DID_PAUSE 0x1173DE0
//sub_141173DE0
//void __fastcall CMFCVisualManagerOffice2003::OnDrawTasksGroupCaption(int64_t a1, int64_t a2, int64_t a3,__int64 a4,__int64 a5,__int64 a6)
extern void(__fastcall *og_did_pause)(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6);
void __fastcall hk_did_pause(int64_t a1, int64_t a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6);


#define OFFSET_PAUSE_CALLER 0x6FE0E8
//__int64 __fastcall sub_1406FE0E8(__int64 a1, __int64 a2, _BYTE *a3)
extern int64_t(__fastcall *og_pause_caller)(int64_t a1, int64_t a2, uint8_t *a3);
int64_t __fastcall hk_pause_caller(int64_t a1, int64_t a2, uint8_t *a3);


#define OFFSET_2_PAUSE 0x6FEDFC
//__int64 __fastcall sub_1406FEDFC(__int64 a1, __int64 a2, __int64 a3)
extern int64_t(__fastcall *og_2_pause)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_2_pause(int64_t a1, int64_t a2, int64_t a3);

#define OFFSET_3_PAUSE 0x6F525C
//__int64 __fastcall sub_1406F525C(__int64 a1, __int64 a2, ULONG_PTR a3)
extern int64_t(__fastcall *og_3_pause)(int64_t a1, int64_t a2, ULONG_PTR a3);
int64_t __fastcall hk_3_pause(int64_t a1, int64_t a2, ULONG_PTR a3);

#define OFFSET_4_PAUSE 0x57FEF8
//__int64 __fastcall sub_14057FEF8(__int64 a1, __int64 a2)
extern int64_t(__fastcall *og_4_pause)(int64_t a1, int64_t a2);
int64_t __fastcall hk_4_pause(int64_t a1, int64_t a2);

#define OFFSET_5_PAUSE 0x580838
//__int64 __fastcall sub_140580838(__int64 a1, __int64 a2, __int64 a3)
extern int64_t(__fastcall *og_5_pause)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_5_pause(int64_t a1, int64_t a2, int64_t a3);



#define OFFSET_PAUSE    0x5837DA //sub_140583698
#define OFFSET_PLAY	    0x585376 //sub_140585234
#define OFFSET_NEXT     0x586E44 //sub_140586B90
#define OFFSET_PREV     0x58721D //sub_140586F20
#define OFFSET_SEEK     0x58566F //sub_140585528
#define OFFSET_SONG     0x581FFD //sub_140581DE4
#define OFFSET_REPEAT   0x5858D7 //sub_1405857C4
#define OFFSET_SHUFFLE1 0x5863F6 //sub_1405862E4
#define OFFSET_SHUFFLE2 0x587F72 //sub_140587DC0

int64_t __fastcall hk_1_arg(int64_t a1);

#endif // INCLUDE_HOOKS_H