#ifndef INCLUDE_LOGGING_H
#define INCLUDE_LOGGING_H

#include <stdio.h>
#include <Windows.h>

enum LogLevel {
	LOG_INFO = 0,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_NOLEVEL
};

void log_msg(int level, const char* msg, ...);
void log_sep(void);
void set_log_window(HWND window);
void set_log_file(FILE* file);

#endif // INCLUDE_LOGGING_H