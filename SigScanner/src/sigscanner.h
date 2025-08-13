#pragma once

#include <Windows.h>
#include <inttypes.h>

void sscn_set_log_fn(void(*fn)(const TCHAR*, ...));

uint64_t sscn_scan_pattern(HINSTANCE base, const char* pattern);
uint64_t sscn_scan_pattern_ex(HINSTANCE base, const char* pattern, int skip_count);
