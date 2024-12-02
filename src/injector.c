#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <TlHelp32.h>

DWORD GetSpotiyPID(void);

int main(void) {
	DWORD spotify_pid = GetSpotiyPID();

	printf("spotify_pid: %ld\n", spotify_pid);

	return 0;
}



DWORD GetSpotiyPID(void) {
    
}