#include "logging.h"
#include <stdarg.h>
#include <string.h>
#include <tchar.h>


#define MAX_LOG_MSG_LENGTH 256
#define MAX_FORMATTED_BUFFER_LENGTH 1024

#define PRUNE_THRESHOLD 0.95f
#define PRUNE_AMOUNT    0.90f

TCHAR formatted_message[MAX_FORMATTED_BUFFER_LENGTH];
HWND* log_window;
HWND* debug_label;
FILE* log_file;

int log_length;

TCHAR old_text[MAX_EDIT_BUFFER_SIZE + 1];


void _vlog_msg(int level, const TCHAR *text, va_list args) {
	TCHAR buffer[MAX_LOG_MSG_LENGTH] = { 0 };

	switch (level) {
	case LOG_INFO: { errno_t err; if ((err = _tcscpy_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("INFO: ")))) __debugbreak(); }    break;
	case LOG_SUCCESS: _tcscpy_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("SUCCESS: ")); break;
	case LOG_WARNING: _tcscpy_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("WARNING: ")); break;
	case LOG_ERROR:   _tcscpy_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("ERROR: "));   break;
	case LOG_DEBUG:   _tcscpy_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("DEBUG: "));   break;
	default: break; // No Log level
	}
	unsigned int textSize = (unsigned int)_tcslen(text);
	memcpy(buffer + _tcslen(buffer), text, (textSize < (MAX_LOG_MSG_LENGTH - 12)) ? textSize : (MAX_LOG_MSG_LENGTH - 12));
	_tcscat_s(buffer, sizeof(buffer) / sizeof(*buffer), TEXT("\r\n"));
	int message_len = _vsntprintf(formatted_message, MAX_FORMATTED_BUFFER_LENGTH, buffer, args);

	if (log_window && *log_window) {
		log_length = GetWindowTextLength(*log_window);

		if (log_length >= (int)(MAX_EDIT_BUFFER_SIZE * PRUNE_THRESHOLD)) {

			int prune_amount = (int)(MAX_EDIT_BUFFER_SIZE * PRUNE_AMOUNT);

			GetWindowText(*log_window, old_text, MAX_EDIT_BUFFER_SIZE);

			TCHAR* new_text = old_text + prune_amount;
			TCHAR* first_newline = _tcschr(new_text, TEXT('\n'));
			if (first_newline) {
				new_text = first_newline + 1; // Start after the newline
			}
			SetWindowText(*log_window, new_text);
		}

		log_length = GetWindowTextLength(*log_window);
		SendMessage(*log_window, EM_SETSEL, (WPARAM)log_length, (LPARAM)log_length);
		SendMessage(*log_window, EM_REPLACESEL, 0, (LPARAM)formatted_message);
	}

	if (debug_label && *debug_label) {
		TCHAR buf[50];
		_stprintf_s(buf, sizeof(buf) / sizeof(*buf), TEXT("DebugLog length: %d"), log_length);
		SetWindowText(*debug_label, buf);
	}

	if (log_file) {
		_fputts(formatted_message, log_file);
		fflush(log_file);
	}

#ifdef CONSOLE
	_tprintf(TEXT("%s"), formatted_message);
#endif
}

void _log_msg(int level, const TCHAR* text, ...) {
	va_list args;
	va_start(args, text);

	_vlog_msg(level, text, args);

	va_end(args);
}

void log_sep(void) {
	_log_msg(LOG_NOLEVEL, TEXT("----------------------------------------------------------------------------"));
}

void set_log_window(HWND* window) { log_window = window; }
void set_log_file(FILE* file) { log_file = file; }
void set_debug_label(HWND* label) { debug_label = label; }