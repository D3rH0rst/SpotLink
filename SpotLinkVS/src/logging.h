#pragma once

#include <stdio.h>
#include <Windows.h>

#define log_msg(level, format, ...) _log_msg(level, TEXT(format), __VA_ARGS__)

#define MAX_EDIT_BUFFER_SIZE 65535

enum LogLevel {
	LOG_INFO = 0,
	LOG_SUCCESS,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_NOLEVEL
};


void _log_msg(int level, const TCHAR* msg, ...);
void log_sep(void);
void set_log_window(HWND* window);
void set_log_file(FILE* file);
void set_debug_label(HWND* label);
