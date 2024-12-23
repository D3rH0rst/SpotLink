#ifndef INCLUDE_LOGGING_H
#define INCLUDE_LOGGING_H

#include <stdio.h>
#include <Windows.h>

#define MAX_EDIT_BUFFER_SIZE 65535

enum LogLevel {
	LOG_INFO = 0,
	LOG_SUCCESS,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_NOLEVEL
};



void log_msg(int level, const char* msg, ...);
void log_sep(void);
void set_log_window(HWND *window);
void set_log_file(FILE* file);
void set_debug_label(HWND *label);
#endif // INCLUDE_LOGGING_H