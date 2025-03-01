#pragma once

#include <Windows.h>
#include <inttypes.h>

uint64_t scan_pattern(HINSTANCE base, const char* pattern);
uint64_t scan_pattern_ex(HINSTANCE base, const char* pattern, int skip_count);
