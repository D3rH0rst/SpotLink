#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <TlHelp32.h>

#define TARGET_EXECUTABLE_NAME TEXT("Spotify.exe")
#define TARGET_EXECUTABLE_PATH TEXT("%APPDATA%\\Spotify\\Spotify.exe")
#define TARGET_WNDCLASS_NAME   TEXT("Chrome_WidgetWin_1")
#define TARGET_DLL_PATH        TEXT(".\\SpotLink.dll")
#define TARGET_DLL_NAME        TEXT("SpotLink.dll")

#ifdef UNICODE
#define TARGETFUNC "LoadLibraryW"
#else
#define TARGETFUNC "LoadLibraryA"
#endif

DWORD GetTargetPID(void);
DWORD IsDLLAlreadyLoaded(DWORD targetPID);
BOOL FileExists(LPCTSTR szPath);
BOOL CheckTargetWindow(DWORD dwOwnerPID);

int InjectRunning(void);
int CreateAndInject(void);

int InjectDll(HANDLE hProcess);

int main(int argc, TCHAR **argv) {

	if (argc == 2 && _tcscmp(argv[1], TEXT("create")) == 0) {
		_tprintf(TEXT("Attempting to create spotify and inject...\n"));
		return CreateAndInject() != 0;
	}

	_tprintf(TEXT("Attempting to inject into the running spotify process...\n"));
	return InjectRunning() != 0;
}

int CreateAndInject(void) {
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	TCHAR expandedPath[MAX_PATH];

	// Expand the %APPDATA% variable
	if (!ExpandEnvironmentStrings(TARGET_EXECUTABLE_PATH, expandedPath, MAX_PATH)) {
		_tprintf(TEXT("ExpandEnvironmentStrings failed (%lu)\n"), GetLastError());
		return 1;
	}

	BOOL success = CreateProcess(expandedPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	if (!success) {
		_tprintf(TEXT("CreateProcess failed: %ld\n"), GetLastError());
		return 1;
	}

	_tprintf("Process created, attemting to inject...\n");

	if (InjectDll(pi.hProcess) != 0) {
		_tprintf(TEXT("DLL injection failed\n"));
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return 1;
	}

	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

int InjectRunning(void) {
	if (!FileExists(TARGET_DLL_PATH)) {
		_tprintf(TEXT("Could not find target dll `%s`\n"), TARGET_DLL_PATH);
		return 1;
	}

	DWORD target_pid = GetTargetPID();

	if (!target_pid) {
		_tprintf(TEXT("Failed to retrieve target pid\n"));
		return 1;
	}

	_tprintf(TEXT("target pid (%s): %ld\n"), TARGET_EXECUTABLE_NAME, target_pid);

	if (IsDLLAlreadyLoaded(target_pid)) {
		_tprintf(TEXT("`%s` is already present in %s\n"), TARGET_DLL_NAME, TARGET_EXECUTABLE_NAME);
		return 1;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, target_pid);
	if (!hProcess) {
		_tprintf(TEXT("Failed to OpenProcess on %ld: %ld\n"), target_pid, GetLastError());
		return 1;
	}

	if (InjectDll(hProcess) != 0) {
		_tprintf(TEXT("Failed to inject the DLL\n"));
		return 1;
	}

	return 0;
}

int InjectDll(HANDLE hProcess) {
	TCHAR full_dll_path[MAX_PATH + 1];
	DWORD dll_path_length = GetFullPathName(TARGET_DLL_PATH, sizeof(full_dll_path) / sizeof(*full_dll_path), full_dll_path, NULL);

	if (dll_path_length == 0) {
		_tprintf(TEXT("Failed to get full dll path for `%s`\n"), TARGET_DLL_PATH);
		return 1;
	}

	_tprintf(TEXT("Target DLL `%s` full path: %s length: %ld\n"), TARGET_DLL_PATH, full_dll_path, dll_path_length);

	PVOID pRemoteString = VirtualAllocEx(hProcess, NULL, dll_path_length + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!pRemoteString) {
		_tprintf(TEXT("Failed to allocate memory in the target process\n"));
		CloseHandle(hProcess);
		return 1;
	}
	_tprintf(TEXT("Allocated memory for the dll string at 0x%p...\n"), pRemoteString);

	if (!WriteProcessMemory(hProcess, pRemoteString, full_dll_path, dll_path_length + 1, NULL)) {
		_tprintf(TEXT("Failed to write target process memory\n"));
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	_tprintf(TEXT("Wrote the dll string into target memory...\n"));
	HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
	if (!hKernel32) {
		_tprintf(TEXT("Failed to get kernel32.dll handle\n"));
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	PVOID pLoadLibrary = GetProcAddress(hKernel32, TARGETFUNC);
	if (!pLoadLibrary) {
		_tprintf(TEXT("Failed to GetProcAddress of `") TEXT(TARGETFUNC) TEXT("` in ´kernel32.dll`\n"));
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	_tprintf(TEXT("Retrieved address for ") TEXT(TARGETFUNC) TEXT(" at 0x%p...\n"), pLoadLibrary);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)pLoadLibrary, pRemoteString, 0, NULL);
	if (!hThread) {
		_tprintf(TEXT("Failed to CreateRemoteThread\n"));
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}
	
	_tprintf(TEXT("Created remote thread for ") TEXT(TARGETFUNC) TEXT(" in target process...\n"));
	WaitForSingleObject(hThread, 4000);
	
	DWORD exitCode;
	if (GetExitCodeThread(hThread, &exitCode) && exitCode != 0) {
		_tprintf(TEXT("DLL loaded successfully (LoadLibrary returned 0x%lx)\n"), exitCode);
	}
	else {
		_tprintf(TEXT("LoadLibrary failed or thread did not complete: %ld\n"), GetLastError());
		VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
		CloseHandle(hThread);
		return 1;
	}


	CloseHandle(hThread);


	_tprintf(TEXT("Successfully injected `%s` into `%s`\n"), full_dll_path, TARGET_EXECUTABLE_NAME);
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
		_tprintf(TEXT("Failed to create process snapshot handle\n"));
		return 0;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)) {
		_tprintf(TEXT("Failed to get process with Process32First\n"));
		CloseHandle(hProcessSnap);
		return 0;
	}

	do {
		if (_tcscmp(pe32.szExeFile, TARGET_EXECUTABLE_NAME) == 0) {
			if (CheckTargetWindow(pe32.th32ProcessID)) {
				targetPID = pe32.th32ProcessID;
				break;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	if (targetPID == 0) {
		_tprintf(TEXT("Could not find any `%s` containing a window with class `%s`\n"), TARGET_EXECUTABLE_NAME, TARGET_WNDCLASS_NAME);
	}
	return targetPID;
}

DWORD IsDLLAlreadyLoaded(DWORD targetPID) {
	HANDLE hModuleSnap;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, targetPID);

	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("Failed to create process snapshot handle\n"));
		return 0;
	}

	me32.dwSize = sizeof(MODULEENTRY32);
	
	if (!Module32First(hModuleSnap, &me32)) {
		_tprintf(TEXT("Failed to get module with Module32First\n"));
		CloseHandle(hModuleSnap);
		return 0;
	}

	do {
		if (_tcscmp(me32.szModule, TARGET_DLL_NAME) == 0) {
			return TRUE;
		}
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	
	return FALSE;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
	TCHAR class_name[100];
	GetClassName(hwnd, class_name, sizeof(class_name) / sizeof(*class_name));
	if (_tcscmp(class_name, TARGET_WNDCLASS_NAME) == 0) {
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
		_tprintf(TEXT("Failed to create thread snapshot handle\n"));
		return FALSE;
	}

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)) {
		_tprintf(TEXT("Failed to get Thread with Thread32First\n"));
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