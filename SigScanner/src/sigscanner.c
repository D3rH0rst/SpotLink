#include "sigscanner.h"
#include <Psapi.h>
#include <string.h>

void(*ssn_log_fn)(const TCHAR*, ...);

static int* generate_pattern_array(const char* pattern, int* array_size) {

	int pattern_length = strlen(pattern);

	int* ret = malloc(sizeof(int) * pattern_length);
	*array_size = 0;
	if (!ret) {
		if (ssn_log_fn) ssn_log_fn(TEXT("[ERROR] Failed to allocate memory for pattern arr: %ld"), GetLastError());
		return NULL;
	}

	for (int i = 0; i < pattern_length;) {
		while (isspace(pattern[i])) i++;
		if (!pattern[i]) break;
		if (pattern[i] == '?' && pattern[i + 1] == '?') {
			ret[(*array_size)++] = -1;
			i += 2;
		}
		else if (isxdigit(pattern[i]) && isxdigit(pattern[i + 1])) {
			int temp = 0;
			if (isdigit(pattern[i]))
				temp += (pattern[i] - '0') * 0x10;
			else
				temp += (toupper(pattern[i]) - 'A' + 0xA) * 0x10;

			if (isdigit(pattern[i + 1]))
				temp += (pattern[i + 1] - '0');
			else
				temp += (toupper(pattern[i + 1]) - 'A' + 0xA);

			ret[(*array_size)++] = temp;
			i += 2;
		}
		else {
			if (ssn_log_fn) ssn_log_fn(TEXT("[ERROR] Failed to generate pattern array : Unknown characters 0x%c%c"), pattern[i], pattern[i + 1]);
			free(ret);
			return NULL;
		}

	}
	return ret;
}

void sscn_set_log_fn(void(*fn)(const TCHAR *, ...)) {
	ssn_log_fn = fn;
}

uint64_t sscn_scan_pattern(HINSTANCE base, const char* pattern) {
	return sscn_scan_pattern_ex(base, pattern, 0);
}

uint64_t sscn_scan_pattern_ex(HINSTANCE base, const char* pattern, int skip_count) {

	MODULEINFO mi;

	if (!GetModuleInformation(GetCurrentProcess(), (HMODULE)base, &mi, sizeof(mi))) {
		if (ssn_log_fn) ssn_log_fn(TEXT("[ERROR] GetModuleInformation failed: %ld"), GetLastError());
		return 0;
	}

	uint8_t* module_base = mi.lpBaseOfDll;

	int pattern_length;
	int* pattern_array = generate_pattern_array(pattern, &pattern_length);

	char found = 1;
	size_t i;
	for (i = 0; i < mi.SizeOfImage - pattern_length; i++) {
		found = 1;
		for (int j = 0; j < pattern_length; j++) {
			if (pattern_array[j] != -1 && (module_base[i + j] != (uint8_t)pattern_array[j])) {
				found = 0;
				break;
			}
		}
		if (found) {
			if (skip_count > 0) {
				skip_count--;
				continue;
			}
			free(pattern_array);
			return (uint64_t)(module_base + i);
		}
	}

	free(pattern_array);

	return 0;
}