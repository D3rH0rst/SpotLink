#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <TlHelp32.h>

#define TARGET_EXECUTABLE_NAME "Notepad.exe" // "spotify.exe"
#define TARGET_WNDCLASS_NAME   "Notepad"     // "Chrome_WidgetWin_1"
#define TARGET_DLL_PATH        ".\\SpotLink.dll"

DWORD GetTargetPID(void);
BOOL FileExists(LPCTSTR szPath);
BOOL CheckTargetWindow(DWORD dwOwnerPID);

int main(void) {
	DWORD target_pid = GetTargetPID();

	if (!target_pid) {
		printf("Failed to retrieve target_pid\n");
		return 1;
	}

	printf("target_pid (%s): %ld\n", TARGET_EXECUTABLE_NAME, target_pid);

	if (!FileExists(TARGET_DLL_PATH)) {
		printf("Could not find `%s`\n", TARGET_DLL_PATH);
		return 1;
	}

	char full_dll_path[MAX_PATH];
	DWORD dll_path_length = GetFullPathName(TARGET_DLL_PATH, MAX_PATH, full_dll_path, NULL);

	if (dll_path_length == 0) {
		printf("Failed to get full dll path for %s\n", TARGET_DLL_PATH);
		return 1;
	}

	printf("Target DLL `%s` full path: %s length: %ld\n", TARGET_DLL_PATH, full_dll_path, dll_path_length);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, target_pid);
	if (!hProcess) {
		printf("Failed to OpenProcess on %ld: %ld\n", target_pid, GetLastError());
		return 1;
	}

	PVOID pRemoteString = VirtualAllocEx(hProcess, NULL, dll_path_length + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!pRemoteString) {
		printf("Failed to allocate memory in the target process\n");
		CloseHandle(hProcess);
		return 1;
	}
	printf("Allocated memory for the dll string at 0x%p...\n", pRemoteString);

	if (!WriteProcessMemory(hProcess, pRemoteString, full_dll_path, dll_path_length + 1, NULL)) {
		printf("Failed to write target process memory\n");
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	printf("Wrote the dll string into target memory...\n");

	PVOID pLoadLibraryA = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (!pLoadLibraryA) {
		printf("Failed to GetProcAddress of `LoadLibraryA` in ´kernel32.dll`\n");
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	printf("Retrieved address for LoadLibraryA at 0x%p...\n", pLoadLibraryA);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)pLoadLibraryA, pRemoteString, 0, NULL);
	if (!hThread) {
		printf("Failed to CreateRemoteThread\n");
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	else {
		printf("Created remote thread for LoadLibraryA in target process...\n");
		WaitForSingleObject(hThread, 4000);
		ResumeThread(hProcess);
		CloseHandle(hThread);
	}


	printf("Successfully injected `%s` into `%s`\n", full_dll_path, TARGET_EXECUTABLE_NAME);
	VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
	CloseHandle(hProcess);

	return 0;
}

BOOL FileExists(LPCTSTR szPath) {
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

DWORD GetTargetPID(void) {
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD targetPID = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		printf("Failed to create process snapshot handle\n");
		return 0;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)) {
		printf("Failed to get process with Process32First\n");
		CloseHandle(hProcessSnap);
		return 0;
	}

	do {
		if (strcmp(pe32.szExeFile, TARGET_EXECUTABLE_NAME) == 0) {
			if (CheckTargetWindow(pe32.th32ProcessID)) {
				targetPID = pe32.th32ProcessID;
				break;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));
	
	CloseHandle(hProcessSnap);
	if (targetPID == 0) {
		printf("Could not find any `%s` containing a window with class `%s`\n", TARGET_EXECUTABLE_NAME, TARGET_WNDCLASS_NAME);
	}
	return targetPID;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
	char class_name[100];
	GetClassName(hwnd, class_name, sizeof(class_name));
	if (strcmp(class_name, TARGET_WNDCLASS_NAME) == 0) {
		*(BOOL*)lParam = TRUE;
		return FALSE;
	}
	return TRUE;
}

BOOL CheckTargetWindow(DWORD dwOwnerPID) {
	HANDLE hThreadSnap;
	THREADENTRY32 te32;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		printf("Failed to create thread snapshot handle\n");
		return FALSE;
	}

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)) {
		printf("Failed to get Thread with Thread32First\n");
		CloseHandle(hThreadSnap);
		return FALSE;
	}

	BOOL found = FALSE;
	do {
		if (te32.th32OwnerProcessID == dwOwnerPID) {
			EnumThreadWindows(te32.th32ThreadID, EnumThreadWndProc, (LPARAM)&found);
			if (found) break;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);

	return found;
}