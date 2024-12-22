#ifndef INCLUDE_SIGSCANNER_H
#define INCLUDE_SIGSCANNER_H

#include <Windows.h>
#include <inttypes.h>

uint64_t scan_pattern(HINSTANCE base, const char *pattern);
uint64_t scan_pattern_ex(HINSTANCE base, const char *pattern, int skip_count);
#endif //INCLUDE_SIGSCANNER_H