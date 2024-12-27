#ifndef INCLUDE_HOOKFUNCS_H
#define INCLUDE_HOOKFUNCS_H

#include <Windows.h>
#include <inttypes.h>
#include "hooking.h"



#define OFF_LOGGING_FUNC 0x1314240
//void sub_141314240(int a1, int a2, int a3, __int64 a4, ...)
extern Hook *hk_logging;
extern void(*og_logging_func)(int a1, int a2, int a3, const char *a4, ...);
void hk_logging_func(int a1, int a2, int a3, const char *a4, ...);


// maybe interesting event func
//void *__fastcall sub_140507464(_QWORD *a1, void *a2, size_t *a3, __int64 a4)

#define OFF_EVENT_FUNC 0x507464
//void *__fastcall sub_140507464(_QWORD *a1, void *a2, size_t *a3, __int64 a4)
extern Hook *hk_event;
extern void*(__fastcall *og_event_func)(uint64_t *a1, void *a2, size_t *a3, int64_t a4);
void *__fastcall hk_event_func(uint64_t *a1, void *a2, size_t *a3, int64_t a4);

#define OFF_TASK_EVENT_FUNC 0x151EB0
//void __fastcall sub_140151EB0(__int64 ***a1, int a2)
extern Hook *hk_task_event;
extern void(__fastcall *og_task_event_func)(int64_t ***a1, int a2);
void __fastcall hk_task_event_func(int64_t ***a1, int a2);

#define OFF_NEW_PAUSE_FUNC 0x15EF90
//void __fastcall sub_14015EF90(__int64 **a1, int a2)
extern Hook *hk_new_pause;
extern int64_t(__fastcall *og_new_pause_func)(int64_t **a1, int a2);
int64_t __fastcall hk_new_pause_func(int64_t **a1, int a2);

#define OFF_NEW_SONG_FUNC 0x59FCD4
//__int64 __fastcall sub_14059FCD4(__int64 a1, _QWORD *a2, __int64 a3, __int64 a4, __int64 a5)
extern Hook *hk_new_song;
extern int64_t(__fastcall *og_new_song_func)(int64_t a1, uint64_t *a2, int64_t a3, int64_t a4, int64_t a5);
int64_t __fastcall hk_new_song_func(int64_t a1, uint64_t *a2, int64_t a3, int64_t a4, int64_t a5);

#define SIG_PAUSE_FUNC "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 10 FC FF FF 48 81 EC F0 04 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 E0 03 00 00 4D 8B F8 48 8B F2 4C 8B F1 48 89 55 40 48 8D 4D 20 E8 ?? ?? ?? ?? 48 8B D8 45 33 E4 44 88 64 24 78 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 88 65 A4 44 88 65 AC 44 88 65 B4 44 88 65 E0 44 88 65 10 44 88 65 1C 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 45 50 48 8D 4D 58 48 89 4C 24 20 4C 89 A5 90 00 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 14 48 8B 01 48 8D 55 58 FF 10 48 89 85 90 00 00 00 48 8B 45 50 48 FF 80 88 00 00 00 49 8B BE 50 FF FF FF 48 8B 07 48 8B 98 18 01 00 00 49 8B 8E F8 FD FF FF 48 8B 01 48 8D 95 A0 00 00 00 FF 50 ?? 90 48 8B D0 48 8B CF FF D3 90 44 38 A5 D0 03 00 00 74 0C 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 49 8D 8E 38 FC FF FF 48 89 4C 24 20 48 89 74 24 28 48 8D 05 ?? ?? ?? ?? 48 89 44 24 30 0F 10 44 24 20 F3 0F 7F 44 24 38 48 8D 44 24 30 48 89 44 24 68 4C 8D 44 24 30 48 8D 95 A0 00 00 00 E8 ?? ?? ?? ?? 48 8B D0 48 8D 4C 24 70 E8 ?? ?? ?? ?? 44 38 A5 40 01 00 00 74 0E 33 D2 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 44 38 A5 10 01 00 00 74 0E 33 D2 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? 48 8D 8D B0 00 00 00 E8 ?? ?? ?? ?? 90 48 8D 4D 50 E8 ?? ?? ?? ?? 49 8B 4F 38 48 85 C9 74 60 48 8B 01 48 8D 55 80 FF 50 ?? 90 44 38 65 10 74 0B 33 D2 48 8D 4D E8 E8 ?? ?? ?? ?? 44 38 65 E0 74 0B 33 D2 48 8D 4D B8 E8 ?? ?? ?? ?? 48 8D 4D 80 E8 ?? ?? ?? ?? 90 48 8B CE E8 ?? ?? ?? ?? 48 8B 8D E0 03 00 00 48 33 CC E8 ?? ?? ?? ?? 48 81 C4 F0 04 00 00 41 5F 41 5E 41 5C 5F 5E 5B 5D C3 E8 ?? ?? ?? ??"
//__int64 __fastcall sub_140583698(__int64 a1, ULONG_PTR a2, __int64 a3)
extern Hook *hk_pause;
extern int64_t(__fastcall *og_pause_func)(int64_t a1, ULONG_PTR a2, int64_t a3);
int64_t __fastcall hk_pause_func(int64_t a1, ULONG_PTR a2, int64_t a3);

#define OFFSET_PLAY_FUNC 0x5A1130
#define SIG_PLAY_FUNC "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 10 FC FF FF 48 81 EC F0 04 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 E0 03 00 00 4D 8B F8 48 8B F2 4C 8B F1 48 89 55 40 48 8D 4D 20 E8 ?? ?? ?? ?? 48 8B D8 45 33 E4 44 88 64 24 78 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 88 65 A4 44 88 65 AC 44 88 65 B4 44 88 65 E0 44 88 65 10 44 88 65 1C 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 45 50 48 8D 4D 58 48 89 4C 24 20 4C 89 A5 90 00 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 14 48 8B 01 48 8D 55 58 FF 10 48 89 85 90 00 00 00 48 8B 45 50 48 FF 80 88 00 00 00 49 8B BE 50 FF FF FF 48 8B 07 48 8B 98 18 01 00 00 49 8B 8E F8 FD FF FF 48 8B 01 48 8D 95 A0 00 00 00 FF 50 ?? 90 48 8B D0 48 8B CF FF D3 90 44 38 A5 D0 03 00 00 74 0C 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 49 8D 8E 38 FC FF FF 48 89 4C 24 20 48 89 74 24 28 48 8D 05 ?? ?? ?? ?? 48 89 44 24 30 0F 10 44 24 20 F3 0F 7F 44 24 38 48 8D 44 24 30 48 89 44 24 68 4C 8D 44 24 30 48 8D 95 A0 00 00 00 E8 ?? ?? ?? ?? 48 8B D0 48 8D 4C 24 70 E8 ?? ?? ?? ?? 44 38 A5 40 01 00 00 74 0E 33 D2 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 44 38 A5 10 01 00 00 74 0E 33 D2 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? 48 8D 8D B0 00 00 00 E8 ?? ?? ?? ?? 90 48 8D 4D 50 E8 ?? ?? ?? ?? 49 8B 4F 38 48 85 C9 74 60 48 8B 01 48 8D 55 80 FF 50 ?? 90 44 38 65 10 74 0B 33 D2 48 8D 4D E8 E8 ?? ?? ?? ?? 44 38 65 E0 74 0B 33 D2 48 8D 4D B8 E8 ?? ?? ?? ?? 48 8D 4D 80 E8 ?? ?? ?? ?? 90 48 8B CE E8 ?? ?? ?? ?? 48 8B 8D E0 03 00 00 48 33 CC E8 ?? ?? ?? ?? 48 81 C4 F0 04 00 00 41 5F 41 5E 41 5C 5F 5E 5B 5D C3 E8 ?? ?? ?? ??"
#define SIG_PLAY_FUNC_2 "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 10 FC FF FF 48 81 EC F0 04 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 E0 03 00 00 4D 8B F8 48 8B F2 4C 8B F1 48 89 55 40 48 8D 4D 20 E8 ?? ?? ?? ?? 48 8B D8 45 33 E4 44 88 64 24 78 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 88 65 A4 44 88 65 AC 44 88 65 B4 44 88 65 E0 44 88 65 10 44 88 65 1C 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 45 50 48 8D 4D 58 48 89 4C 24 20 4C 89 A5 90 00 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 14 48 8B 01 48 8D 55 58 FF 10 48 89 85 90 00 00 00 48 8B 45 50 48 FF 80 88 00 00 00 49 8B BE 50 FF FF FF 48 8B 07 48 8B 98 18 01 00 00 49 8B 8E F8 FD FF FF 48 8B 01 48 8D 95 A0 00 00 00 FF 50 ?? 90 48 8B D0 48 8B CF FF D3 90 44 38 A5 D0 03 00 00 74 0C 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 49 8D 8E 38 FC FF FF 48 89 4C 24 20 48 89 74 24 28 48 8D 05 ?? ?? ?? ?? 48 89 44 24 30 0F 10 44 24 20 F3 0F 7F 44 24 38 48 8D 44 24 30 48 89 44 24 68 4C 8D 44 24 30 48 8D 95 A0 00 00 00 E8 ?? ?? ?? ?? 48 8B D0 48 8D 4C 24 70 E8 ?? ?? ?? ?? 44 38 A5 40 01 00 00 74 0E 33 D2 48 8D 8D 18 01 00 00 E8 ?? ?? ?? ?? 44 38 A5 10 01 00 00 74 0E 33 D2 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? 48 8D 8D B0 00 00 00 E8 ?? ?? ?? ?? 90 48 8D 4D 50 E8 ?? ?? ?? ?? 49 8B 4F 38 48 85 C9 74 60 48 8B 01 48 8D 55 80 FF 50 ?? 90 44 38 65 10 74 0B 33 D2 48 8D 4D E8 E8 ?? ?? ?? ?? 44 38 65 E0 74 0B 33 D2 48 8D 4D B8 E8 ?? ?? ?? ?? 48 8D 4D 80 E8 ?? ?? ?? ?? 90 48 8B CE E8 ?? ?? ?? ?? 48 8B 8D E0 03 00 00 48 33 CC E8 ?? ?? ?? ?? 48 81 C4 F0 04 00 00 41 5F 41 5E 41 5C 5F 5E 5B 5D C3 E8 ?? ?? ?? ?? 48 89 5C 24 18 57 48 81 EC 90 03"
//__int64 __fastcall sub_140585234(__int64 a1, ULONG_PTR a2, __int64 a3)
extern Hook *hk_play;
extern int64_t(__fastcall *og_play_func)(int64_t a1, ULONG_PTR a2, int64_t a3);
int64_t __fastcall hk_play_func(int64_t a1, ULONG_PTR a2, int64_t a3);

#define SIG_NEXT_FUNC "48 89 5C 24 20 55 56 57 41 56 41 57 48 8D AC 24 70 FE FF FF 48 81 EC 90 02 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 80 01 00 00 4D 8B F0 48 8B F2 48 8B F9 48 89 95 E0 00 00 00 48 8B 99 48 FF FF FF 48 8B 89 C0 FE FF FF 45 33 FF 48 85 C9 0F 84 1F"
//__int64 __fastcall sub_140586B90(__int64 a1, char *a2, __int64 a3)
extern Hook *hk_next;
extern int64_t(__fastcall *og_next_func)(int64_t a1, char *a2, int64_t a3);
int64_t __fastcall hk_next_func(int64_t a1, char *a2, int64_t a3);

#define SIG_PREV_FUNC "48 89 5C 24 20 55 56 57 41 56 41 57 48 8D AC 24 70 FE FF FF 48 81 EC 90 02 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 80 01 00 00 4D 8B F0 48 8B F2 48 8B F9 48 89 95 E0 00 00 00 48 8B 99 48 FF FF FF 48 8B 89 C0 FE FF FF 45 33 FF 48 85 C9 0F 84 DC 03 00 00 48 8B 01 48 8D 54 24 30 FF 50 ?? 48 8D 54 24 30 48 8B CB E8 ?? ?? ?? ?? 84 C0 0F 84 01 01 00 00 E8 ?? ?? ?? ?? 0F 57 C0 0F 11 85 E8 00 00 00 0F 57 C9 F3 0F 7F 8D F8 00 00 00 4C 8B 40 10 48 83 78 18 10 72 03 48 8B 00 48 8B D0 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? 90 48 8D 85 E8 00 00 00 48 89 44 24 40 48 8D 85 08 01 00 00 48 89 44 24 48 48 8D 54 24 40 48 8D 4C 24 20 E8 ?? ?? ?? ?? 90 44 89 7C 24 30 4C 89 7C 24 38 BA 09 00 00 00 48 8D 4C 24 50 E8 ?? ?? ?? ?? 0F 10 00 F3 0F 7F 44 24 30 4C 8D 44 24 20 48 8D 54 24 30 48 8D 4C 24 60 E8 ?? ?? ?? ?? 48 8B D0 49 8B 4E 38 48 85 C9 0F 84 1C 03 00 00 48 8B 01 FF 50 ?? 90 48 8D 4C 24 60 E8 ?? ?? ?? ?? 90 4C 8B 44 24 20 4D 8B 40 08 48 8D 54 24 20 48 8D 4C 24 20 E8 ?? ?? ?? ?? BA 40 00 00 00 48 8B 4C 24 20 E8 ?? ?? ?? ?? 90 4C 8D 0D ?? ?? ?? ?? BA 20 00 00 00 44 8D 42 E1 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? E9 87 02 00 00 48 8D 4C 24 60 E8 ?? ?? ?? ?? 48 8B D8 44 88 7D 88 48 8B D0 48 8D 4D 90 E8 ?? ?? ?? ?? 44 88 7D B4 44 88 7D BC 44 88 7D C4 44 88 7D F0 44 88 7D 20 44 88 7D 2C 48 8B CB E8 ?? ?? ?? ?? 90 48 8D 87 E0 FC FF FF 48 89 85 30 01 00 00 48 8D 8D 38 01 00 00 48 89 4C 24 20 4C 89 BD 70 01 00 00 48 8B 8F 20 FF FF FF 48 85 C9 74 1A 48 8B 01 48 8D 95 38 01 00 00 FF 10 48 89 85 70 01 00 00 48 8B 85 30 01 00 00 48 FF 80 88 00 00 00 48 8D 9F 38 FC FF FF 48 8D 44 24 40 48 89 44 24 20 0F 57 C0 F3 0F 7F 44 24 40 0F 11 85 E8 00 00 00 41 B8 07 00 00 00 4C 89 85 F8 00 00 00 48 C7 85 00 01 00 00 0F 00 00 00 48 8D 15 ?? ?? ?? ?? 48 8D 8D E8 00 00 00 E8 ?? ?? ?? ?? 44 88 BD EF 00 00 00 4C 8D 44 24 40 48 8D 95 E8 00 00 00 48 8B CB E8 ?? ?? ?? ?? 48 8D 57 C8 48 8D 8E 10 01 00 00 E8 ?? ?? ?? ?? 48 8B 8F C0 FE FF FF 48 85 C9 0F 84 B4 01 00 00 48 8B 01 48 8D 54 24 30 FF 50 ?? 48 8B 8F 50 FF FF FF 48 8B 01 FF 90 ?? ?? ?? ?? 4C 8B 44 24 30 48 8D 54 24 50 48 8B C8 E8 ?? ?? ?? ?? 48 89 5C 24 40 48 89 74 24 48 48 8D 05 ?? ?? ?? ?? 48 89 85 E8 00 00 00 0F 10 44 24 40 F3 0F 7F 85 F0 00 00 00 48 8D 85 E8 00 00 00 48 89 85 20 01 00 00 4C 8D 85 E8 00 00 00 48 8D 55 30 48 8B CB E8 ?? ?? ?? ?? 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 38 BD D0 00 00 00 74 0E 33 D2 48 8D 8D A8 00 00 00 E8 ?? ?? ?? ?? 44 38 BD A0 00 00 00 74 0B 33 D2 48 8D 4D 78 E8 ?? ?? ?? ?? 48 8D 4D 40 E8 ?? ?? ?? ?? 48 8D 4D 90 E8 ?? ?? ?? ?? 84 C0 75 68 44 38 7E 62 75 62 44 38 7D 88 74 5C 48 8B 8F 50 FF FF FF 48 8B 01 FF 90 ?? ?? ?? ?? 48 8B D8 44 38 B8 F8 05 00 00 74 40 48 8B 4F 80 48 8B 01 FF 10 84 C0 75 33 48 8D 8B 40 03 00 00 E8 ?? ?? ?? ?? 84 C0 74 23 44 38 7F 90 75 06 44 38 7F 91 74 17 48 8B 4F 80 48 8B 01 4C 8D 44 24 50 48 8D 93 40 03 00 00 FF 50 ?? 90 48 8D 8D 30 01 00 00 E8 ?? ?? ?? ?? 49 8B 4E 38 48 85 C9 74 65 48 8B 01 48 8D 55 90 FF 50 ?? 90 44 38 7D 20 74 0B 33 D2 48 8D 4D F8 E8 ?? ?? ?? ?? 44 38 7D F0 74 0B 33 D2 48 8D 4D C8 E8 ?? ?? ?? ?? 48 8D 4D 90 E8 ?? ?? ?? ?? 90 48 8B CE E8 ?? ?? ?? ?? 48 8B 8D 80 01 00 00 48 33 CC E8 ?? ?? ?? ?? 48 8B 9C 24 D8 02 00 00 48 81 C4 90 02 00 00 41 5F 41 5E 5F 5E 5D C3 E8 ?? ?? ?? ?? 90 E8 ?? ?? ?? ?? 90 E8 ?? ?? ?? ?? 90 E8 ?? ?? ?? ??"
//__int64 __fastcall sub_140586F20(__int64 a1, _BYTE *a2, __int64 a3)
extern Hook *hk_prev;
extern int64_t(__fastcall *og_prev_func)(int64_t a1, BYTE *a2, int64_t a3);
int64_t __fastcall hk_prev_func(int64_t a1, BYTE *a2, int64_t a3);

#define SIG_SEEK_FUNC "48 89 5C 24 20 55 56 57 41 56 41 57 48 8D AC 24 80 FE FF FF 48 81 EC 80 02 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 70 01 00 00 4D 8B F0"
//__int64 __fastcall sub_140585528(__int64 a1, __int64 a2, __int64 a3)
extern Hook *hk_seek;
extern int64_t(__fastcall *og_seek_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_seek_func(int64_t a1, int64_t a2, int64_t a3);

#define SIG_SONG_FUNC "48 89 5C 24 20 55 56 57 41 56 41 57 48 8D AC 24 C0 FD FF FF 48 81 EC 40 03 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 30 02 00 00 4D 8B F9"
//const struct _GUID *__fastcall sub_140581DE4(__int64 a1, const struct _GUID *a2, __int64 a3, __int64 a4, __int64 a5, __int64 a6)
extern Hook *hk_song;
extern const GUID*(__fastcall *og_song_func)(int64_t a1, const GUID *a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6);
const GUID* __fastcall hk_song_func(int64_t a1, const GUID *a2, int64_t a3, int64_t a4, int64_t a5, int64_t a6);

#define SIG_REPEAT_FUNC "48 89 5C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 90 FD FF FF 48 81 EC 70 03 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 60 02 00 00 4D 8B E8 4C 8B FA 4C 8B F1 48 89 95 00 02 00 00 48 8D 8D D0 00 00 00 E8 ?? ?? ?? ?? 48 8B D8 33 FF 40 88 7C 24 38 48 8B D0 48 8D 4C 24 40 E8 ?? ?? ?? ?? 40 88 7C 24 64 40 88 7C 24 6C 40 88 7C 24 74 40 88 7D A0 40 88 7D D0 40 88 7D DC 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 85 10 02 00 00 48 8D 8D 18 02 00 00 48 89 4C 24 20 48 89 BD 50 02 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 1A 48 8B 01 48 8D 95 18 02 00 00 FF 10 48 89 85 50 02 00 00 48 8B 85 10 02 00 00 48 FF 80 88 00 00 00 4D 8D A6 38 FC FF FF 4C 89 64 24 20 4C 89 7C 24 28 48 8D 05 ?? ?? ?? ?? 48 89 45 E0 0F 10 44 24 20 F3 0F 7F 45 E8 48 8D 45 E0 48 89 45 18 4C 8D 45 E0 48 8D 55 20 49 8B CC E8 ?? ?? ?? ?? 48 8B D0 48 8D 4C 24 30 E8 ?? ?? ?? ?? 40 38 BD C0 00 00 00 74 0E 33 D2 48 8D 8D 98 00 00 00 E8 ?? ?? ?? ?? 40 38 BD 90 00 00 00 74 0B 33 D2 48 8D 4D 68 E8 ?? ?? ?? ?? 48 8D 4D 30 E8 ?? ?? ?? ?? 48 8D 4C 24 40 E8 ?? ?? ?? ?? 84 C0 0F 85 D5 00 00 00 4D 8B B6 28 FF FF FF 49 8B 1E E9 AA 00 00 00 48 8B B3 B8 09 00 00 48 85 F6 74 5B 48 8B 06 48 8B B8 90 00 00 00 49 8B D7 48 8D 8D F0 00 00 00 E8 ?? ?? ?? ?? 4C 8B C0 48 8D 55 20 48 8B CE FF D7 33 FF 40 38 BD C0 00 00 00 74 0E 33 D2 48 8D 8D 98 00 00 00 E8 ?? ?? ?? ?? 40 38 BD 90 00 00 00 74 0B 33 D2 48 8D 4D 68 E8 ?? ?? ?? ?? 48 8D 4D 30 E8 ?? ?? ?? ?? 48 8B 43 10 40 38 78 19 74 1E 48 8B 43 08 EB 0D 48 3B 58 10 75 0D 48 8B D8 48 8B 40 08 40 38 78 19 74 ED 48 8B D8 EB 1B 48 8B D8 48 8B 08 40 38 79 19 75 0F 48 8B D9 48 8B 01 48 8B C8 40 38 78 19 74 F1 49 3B DE 0F 85 4D FF FF FF 8A 45 DC F6 D8 1B D2 23 55 D8 49 8B CC E8 ?? ?? ?? ?? 90 48 8D 8D 10 02 00 00 E8 ?? ?? ?? ?? 49 8B 4D 38 48 85 C9 74 6C 48 8B 01 48 8D 54 24 40 FF 50 ?? 90 40 38 7D D0 74 0B 33 D2 48 8D 4D A8 E8 ?? ?? ?? ?? 40 38 7D A0 74 0C 33 D2 48 8D 4C 24 78 E8 ?? ?? ?? ?? 48 8D 4C 24 40 E8 ?? ?? ?? ?? 90 49 8B CF E8 ?? ?? ?? ?? 48 8B 8D 60 02 00 00 48 33 CC E8 ?? ?? ?? ?? 48 8B 9C 24 C8 03 00 00 48 81 C4 70 03 00 00 41 5F 41 5E 41 5D 41 5C 5F 5E 5D C3 E8 ?? ?? ?? ??"
//__int64 __fastcall sub_1405857C4(__int64 a1, __int64 a2, __int64 a3)
extern Hook *hk_repeat;
extern int64_t(__fastcall *og_repeat_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_repeat_func(int64_t a1, int64_t a2, int64_t a3);

#define SIG_SHUFFLE1_FUNC "48 89 5C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 E0 FD FF FF 48 81 EC 20 03 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 10 02 00 00 4D 8B E8 4C 8B FA 4C 8B F1 48 89 95 B0 01 00 00 48 8D 8D D0 00 00 00 E8 ?? ?? ?? ?? 48 8B D8 45 33 E4 44 88 64 24 78 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 88 65 A4 44 88 65 AC 44 88 65 B4 44 88 65 E0 44 88 65 10 44 88 65 1C 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 85 C0 01 00 00 48 8D 8D C8 01 00 00 48 89 4C 24 20 4C 89 A5 00 02 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 1A 48 8B 01 48 8D 95 C8 01 00 00 FF 10 48 89 85 00 02 00 00 48 8B 85 C0 01 00 00 48 FF 80 88 00 00 00 49 8D 8E 38 FC FF FF 48 89 4C 24 20 4C 89 7C 24 28 48 8D 05 ?? ?? ?? ?? 48 89 44 24 30 0F 10 44 24 20 F3 0F 7F 44 24 38 48 8D 44 24 30 48 89 44 24 68 4C 8D 44 24 30 48 8D 55 20 E8 ?? ?? ?? ?? 48 8B D0 48 8D 4C 24 70 E8 ?? ?? ?? ?? 44 38 A5 C0 00 00 00 74 0E 33 D2 48 8D 8D 98 00 00 00 E8 ?? ?? ?? ?? 44 38 A5 90 00 00 00 74 0B 33 D2 48 8D 4D 68 E8 ?? ?? ?? ?? 48 8D 4D 30 E8 ?? ?? ?? ?? 48 8D 4D 80 E8 ?? ?? ?? ?? 84 C0 0F 85 BD 00 00 00 4D 8B B6 28 FF FF FF 49 8B 1E E9 A5 00 00 00 48 8B B3 B8 09 00 00 48 85 F6 74 56 48 8B 06 48 8B 78 78 49 8B D7 48 8D 8D F0 00 00 00 E8 ?? ?? ?? ?? 4C 8B C0 48 8D 55 20 48 8B CE FF D7 44 38 A5 C0 00 00 00 74 0E 33 D2 48 8D 8D 98 00 00 00 E8 ?? ?? ?? ?? 44 38 A5 90 00 00 00 74 0B 33 D2 48 8D 4D 68 E8 ?? ?? ?? ?? 48 8D 4D 30 E8 ?? ?? ?? ?? 48 8B 43 10 44 38 60 19 74 1E 48 8B 43 08 EB 0D 48 3B 58 10 75 0D 48 8B D8 48 8B 40 08 44 38 60 19 74 ED 48 8B D8 EB 1B 48 8B D8 48 8B 08 44 38 61 19 75 0F 48 8B D9 48 8B 01 48 8B C8 44 38 60 19 74 F1 49 3B DE 0F 85 52 FF FF FF 48 8D 8D C0 01 00 00 E8 ?? ?? ?? ?? 49 8B 4D 38 48 85 C9 74 69 48 8B 01 48 8D 55 80 FF 50 ?? 90 44 38 65 10 74 0B 33 D2 48 8D 4D E8 E8 ?? ?? ?? ?? 44 38 65 E0 74 0B 33 D2 48 8D 4D B8 E8 ?? ?? ?? ?? 48 8D 4D 80 E8 ?? ?? ?? ?? 90 49 8B CF E8 ?? ?? ?? ?? 48 8B 8D 10 02 00 00 48 33 CC E8 ?? ?? ?? ?? 48 8B 9C 24 78 03 00 00 48 81 C4 20 03 00 00 41 5F 41 5E 41 5D 41 5C 5F 5E 5D C3 E8 ?? ?? ?? ??"
//__int64 __fastcall sub_1405862E4(__int64 a1, __int64 a2, __int64 a3)
extern Hook *hk_shuffle1;
extern int64_t(__fastcall *og_shuffle1_func)(int64_t a1, int64_t a2, int64_t a3);
int64_t __fastcall hk_shuffle1_func(int64_t a1, int64_t a2, int64_t a3);

#define SIG_SHUFFLE2_FUNC "48 89 5C 24 18 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 FC FF FF 48 81 EC D0 04 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 C0 03 00 00 4D 8B E9 49 8B F0 4C 8B FA 4C 8B F1 48 8D 8D E0 00 00 00 E8 ?? ?? ?? ?? 48 8B D8 45 33 E4 44 88 65 88 48 8B D0 48 8D 4D 90 E8 ?? ?? ?? ?? 44 88 65 B4 44 88 65 BC 44 88 65 C4 44 88 65 F0 44 88 65 20 44 88 65 2C 48 8B CB E8 ?? ?? ?? ?? 90 49 8D 86 E0 FC FF FF 48 89 85 70 03 00 00 48 8D 8D 78 03 00 00 48 89 4C 24 20 4C 89 A5 B0 03 00 00 49 8B 8E 20 FF FF FF 48 85 C9 74 1A 48 8B 01 48 8D 95 78 03 00 00 FF 10 48 89 85 B0 03 00 00 48 8B 85 70 03 00 00 48 FF 80 88 00 00 00 49 8D 9E 28 FF FF FF 4C 8B C6 48 8D 54 24 28 48 8B CB E8 ?? ?? ?? ?? 4C 8B C6 48 8B 7C 24 38 48 8B D7 48 8B CB E8 ?? ?? ?? ?? 84 C0 74 13 48 3B 3B 74 11 48 8D 4F 40 49 8B D7 E8 ?? ?? ?? ?? EB 03 48 8B 3B 49 8B 8E 50 FF FF FF 48 8B 01 FF 90 ?? ?? ?? ?? 48 05 38 09 00 00 48 8B 50 10 48 83 78 18 10 72 03 48 8B 00 4C 8B 46 10 48 8B CE 48 83 7E 18 10 72 03 48 8B 0E 4C 3B C2 0F 85 C4 00 00 00 48 8B D0 E8 ?? ?? ?? ?? 85 C0 0F 85 B4 00 00 00 49 8D 9E 38 FC FF FF 48 89 5C 24 28 4C 89 7C 24 30 48 89 74 24 38 48 8D 05 ?? ?? ?? ?? 48 89 44 24 40 0F 10 44 24 28 0F 11 44 24 48 F2 0F 10 4C 24 38 F2 0F 11 4C 24 58 48 8D 44 24 40 48 89 44 24 78 4C 8D 44 24 40 48 8D 55 30 48 8B CB E8 ?? ?? ?? ?? 48 8B D0 48 8D 4D 80 E8 ?? ?? ?? ?? 44 38 A5 D0 00 00 00 74 0E 33 D2 48 8D 8D A8 00 00 00 E8 ?? ?? ?? ?? 44 38 A5 A0 00 00 00 74 0B 33 D2 48 8D 4D 78 E8 ?? ?? ?? ?? 48 8D 4D 40 E8 ?? ?? ?? ?? 48 8D 4D 90 E8 ?? ?? ?? ?? 84 C0 75 77 8A 45 2C F6 D8 1B D2 23 55 28 48 8B CB E8 ?? ?? ?? ?? EB 63 48 3B 3B 74 5E 48 8B BF B8 09 00 00 48 8B 07 48 8B 58 30 49 8B D7 48 8D 8D 00 01 00 00 E8 ?? ?? ?? ?? 4C 8B C0 48 8D 55 30 48 8B CF FF D3 44 38 A5 D0 00 00 00 74 0E 33 D2 48 8D 8D A8 00 00 00 E8 ?? ?? ?? ?? 44 38 A5 A0 00 00 00 74 0B 33 D2 48 8D 4D 78 E8 ?? ?? ?? ?? 48 8D 4D 40 E8 ?? ?? ?? ?? 90 48 8D 8D 70 03 00 00 E8 ?? ?? ?? ?? 49 8B 4D 38 48 85 C9 74 60 48 8B 01 48 8D 55 90 FF 50 ?? 90 44 38 65 20 74 0B 33 D2 48 8D 4D F8 E8 ?? ?? ?? ?? 44 38 65 F0 74 0B 33 D2 48 8D 4D C8 E8 ?? ?? ?? ?? 48 8D 4D 90 E8 ?? ?? ?? ?? 48 8B 8D C0 03 00 00 48 33 CC E8 ?? ?? ?? ?? 48 8B 9C 24 20 05 00 00 48 81 C4 D0 04 00 00 41 5F 41 5E 41 5D 41 5C 5F 5E 5D C3 E8 ?? ?? ?? ??"
//__int64 __fastcall sub_140587DC0(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
extern Hook *hk_shuffle2;
extern int64_t(__fastcall *og_shuffle2_func)(int64_t a1, int64_t a2, int64_t a3, int64_t a4);
int64_t __fastcall hk_shuffle2_func(int64_t a1, int64_t a2, int64_t a3, int64_t a4);


#endif //INCLUDE_HOOKFUNCS_H