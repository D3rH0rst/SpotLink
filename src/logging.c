#include "logging.h"
#include <stdarg.h>
#include <string.h>

#define MAX_LOG_MSG_LENGTH 256
#define MAX_FORMATTED_BUFFER_LENGTH 1024

char formatted_message[MAX_FORMATTED_BUFFER_LENGTH];
HWND log_window;
FILE* log_file;

void log_msg(int level, const char* text, ...) {
	va_list args;
	va_start(args, text);

	char buffer[MAX_LOG_MSG_LENGTH] = { 0 };
	switch (level) {
		case LOG_INFO:    strcpy(buffer, "INFO: ");    break;
		case LOG_WARNING: strcpy(buffer, "WARNING: "); break;
		case LOG_ERROR:   strcpy(buffer, "ERROR: ");   break;
		case LOG_DEBUG:   strcpy(buffer, "DEBUG: ");   break;
		default: break; // No Log level
	}
	unsigned int textSize = (unsigned int)strlen(text);
	memcpy(buffer + strlen(buffer), text, (textSize < (MAX_LOG_MSG_LENGTH - 12)) ? textSize : (MAX_LOG_MSG_LENGTH - 12));
	strcat(buffer, "\r\n");
	//vprintf(buffer, args);
	int message_len = vsnprintf(formatted_message, MAX_FORMATTED_BUFFER_LENGTH, buffer, args);

	if (log_window) {
		int index = GetWindowTextLength(log_window);
		SendMessage(log_window, EM_SETSEL, (WPARAM)index, (LPARAM)index);
		SendMessage(log_window, EM_REPLACESEL, 0, (LPARAM)formatted_message);
	}

	if (log_file) {
		fwrite(formatted_message, sizeof(char), message_len, log_file);
		fflush(log_file);
	}

#ifdef CONSOLE
	printf("%s", formatted_message);
#endif
	va_end(args);
}

void log_sep(void) {
	log_msg(LOG_NOLEVEL, "----------------------------------------------------------------------------");
}

void set_log_window(HWND window) { log_window = window; }
void set_log_file(FILE* file) { log_file = file; }