#include "logging.h"
#include <stdarg.h>
#include <string.h>

#define MAX_LOG_MSG_LENGTH 256
#define MAX_FORMATTED_BUFFER_LENGTH 1024

#define PRUNE_THRESHOLD 0.95f
#define PRUNE_AMOUNT    0.90f

char formatted_message[MAX_FORMATTED_BUFFER_LENGTH];
HWND *log_window;
HWND *debug_label;
FILE* log_file;

int log_length;

char old_text[MAX_EDIT_BUFFER_SIZE + 1];

void log_msg(int level, const char* text, ...) {
	va_list args;
	va_start(args, text);

	char buffer[MAX_LOG_MSG_LENGTH] = { 0 };

	switch (level) {
		case LOG_INFO:    strcpy(buffer, "INFO: "   );    break;
		case LOG_SUCCESS: strcpy(buffer, "SUCCESS: "); break;
		case LOG_WARNING: strcpy(buffer, "WARNING: "); break;
		case LOG_ERROR:   strcpy(buffer, "ERROR: "  );   break;
		case LOG_DEBUG:   strcpy(buffer, "DEBUG: "  );   break;
		default: break; // No Log level
	}
	unsigned int textSize = (unsigned int)strlen(text);
	memcpy(buffer + strlen(buffer), text, (textSize < (MAX_LOG_MSG_LENGTH - 12)) ? textSize : (MAX_LOG_MSG_LENGTH - 12));
	strcat(buffer, "\r\n");
	int message_len = vsnprintf(formatted_message, MAX_FORMATTED_BUFFER_LENGTH, buffer, args);

	if (log_window && *log_window) {
		log_length = GetWindowTextLength(*log_window);

		if (log_length >= (int)(MAX_EDIT_BUFFER_SIZE * PRUNE_THRESHOLD)) {
			
			int prune_amount = (int)(MAX_EDIT_BUFFER_SIZE * PRUNE_AMOUNT);

			GetWindowText(*log_window, old_text, MAX_EDIT_BUFFER_SIZE);
			
			char *new_text = old_text + prune_amount;
			char* first_newline = strchr(new_text, '\n');
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
		char buf[50];
		sprintf(buf, "DebugLog length: %d", log_length);
		SetWindowText(*debug_label, buf);
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

void set_log_window(HWND *window) { log_window = window; }
void set_log_file(FILE* file)     { log_file = file; }
void set_debug_label(HWND *label) { debug_label = label; }