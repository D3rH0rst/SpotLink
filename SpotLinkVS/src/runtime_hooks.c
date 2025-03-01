#include "hooking.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <intrin.h>

#include "logging.h"

#define RH_SHADOW_SPACE 0x20
#define RH_FUNC_SIZE 256
#define RH_MAX_PAGES 5
#define RH_CALLEDSTR_LEN 256

uint8_t reg_bytes[] = { 0x4D, 0x55, 0x45, 0x4D };

const TCHAR* ret_str = TEXT("ret = 0x%llX");

DWORD rh_page_size;
uint16_t max_func_index;

void* pages[RH_MAX_PAGES];
size_t pages_size = 0;
size_t func_index;


FILE* rh_logfile;

int rh_alloc_new_page(void) {
	pages[pages_size] = VirtualAlloc(NULL, rh_page_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!pages[pages_size]) {
		log_msg(LOG_ERROR, "Failed to allocate memory for hk_func");
		return 1;
	}

	pages_size++;

	func_index = 0;

	return 0;
}

int rh_init(void) {
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	rh_page_size = sys_info.dwPageSize;

	max_func_index = rh_page_size / RH_FUNC_SIZE;

	if (rh_alloc_new_page() != 0) return 1;

	if (fopen_s(&rh_logfile, "./rh_log.txt", "w")) {
		log_msg(LOG_ERROR, "Failed to open rh_log.txt");
		return 1;
	}

	return 0;
}

void rh_clean(void) {
	for (size_t i = 0; i < pages_size; i++) {
		VirtualFree(pages[i], 0, MEM_FREE);
	}
	pages_size = 0;
	func_index = 0;

	if (rh_logfile)
		fclose(rh_logfile);
}

uint16_t get_arg_reg(uint8_t i) {
	switch (i) {
	case 0:
		return 0x484D;
	case 1:
		return 0x4855;
	case 2:
		return 0x4C45;
	case 3:
		return 0x4C4D;
	default:
		return -1;
	}
}

int generate_called_string(Hook* h) {
	h->runtime_hook->called_str = malloc(RH_CALLEDSTR_LEN * sizeof(TCHAR));
	if (!h->runtime_hook->called_str) {
		log_msg(LOG_ERROR, "Failed to allocate memory for h->called_str");
		return 1;
	}

	_sntprintf_s(h->runtime_hook->called_str, RH_CALLEDSTR_LEN, _TRUNCATE, TEXT("Called `%s("), h->name);
	for (int i = 0; i < h->runtime_hook->arg_count; i++) {
		if (i == 0) {
			_tcscat_s(h->runtime_hook->called_str, RH_CALLEDSTR_LEN, TEXT("0x%llX"));
		}
		else {
			_tcscat_s(h->runtime_hook->called_str, RH_CALLEDSTR_LEN, TEXT(", 0x%llX"));
		}
	}
	_tcscat_s(h->runtime_hook->called_str, RH_CALLEDSTR_LEN, TEXT(")`"));

	log_msg(LOG_INFO, "Generated string: %s", h->runtime_hook->called_str);

	return 0;
}

void rh_function_prologue(Hook* h, uint8_t** code_ptr) {

	uint8_t stack_size = h->runtime_hook->stackspace; // DEBUG:

	// function prologue
	*((uint8_t*)(*code_ptr)) = 0x55;                            // push    rbp
	(*code_ptr) += sizeof(uint8_t);

	*((uint32_t*)(*code_ptr)) = 0xE58948;                        // mov     rbp, rsp
	(*code_ptr) += sizeof(uint32_t) - 1;

	if (stack_size < 0x80) {
		*((uint32_t*)(*code_ptr)) = 0x00EC8348 | (stack_size << 24); // sub     rsp, stack_size
		(*code_ptr) += sizeof(uint32_t);
	}
	else {
		*((uint64_t*)(*code_ptr)) = 0x00EC8148 | ((uint64_t)stack_size << 24);
		(*code_ptr) += sizeof(uint64_t) - 1;
	}
}

void rh_move_args_to_stack(Hook* h, uint8_t** code_ptr) {
	char spilled_args = (h->runtime_hook->arg_count > 4) * (h->runtime_hook->arg_count - 4);
	// move register args on the stack
	for (uint8_t i = 0; i < h->runtime_hook->arg_count - spilled_args; i++) {
		uint16_t reg = get_arg_reg(i);

		*((*code_ptr)++) = (reg & 0xFF00) >> 8; // REX prefix
		*((*code_ptr)++) = 0x89;                // mov instruction
		*((*code_ptr)++) = (reg & 0x00FF) >> 0; // ModR/M byte
		*((*code_ptr)++) = 0x10 + 8 * i;        // arg_offset

		// example instruction: 48 89 4D 10   mov [rbp+arg_offset], rcx
	}
}

void rh_call_func(void* func, uint8_t** code_ptr) {
	*((uint16_t*)(*code_ptr)) = 0xB848;                          // mov rax, 0xADDR
	(*code_ptr) += sizeof(uint16_t);

	*((uint64_t*)(*code_ptr)) = (uint64_t)func;
	(*code_ptr) += sizeof(uint64_t);

	*((uint16_t*)(*code_ptr)) = 0xD0FF;						  // call rax
	(*code_ptr) += sizeof(uint16_t);
}

void rh_call_deref_func(uint64_t addr, uint8_t** code_ptr) {
	*((uint16_t*)(*code_ptr)) = 0xA148;                          // mov rax, ds:0xADDR
	(*code_ptr) += sizeof(uint16_t);

	*((uint64_t*)(*code_ptr)) = addr;
	(*code_ptr) += sizeof(uint64_t);

	*((uint16_t*)(*code_ptr)) = 0xD0FF;						  // call rax
	(*code_ptr) += sizeof(uint16_t);
}

void rh_prepare_call_hook_callback(Hook* h, uint8_t** code_ptr) {
	*((uint16_t*)(*code_ptr)) = 0xB948;                          // mov rcx 0xADDR
	(*code_ptr) += sizeof(uint16_t);

	*((uint64_t*)(*code_ptr)) = (uint64_t)h; // move the pointer to the hook itself
	(*code_ptr) += sizeof(uint64_t);
}

void rh_prepare_call_log_msg_first(Hook* h, uint8_t** code_ptr) {
	// load LOG_INFO (0) into ecx
	*((uint64_t*)(*code_ptr)) = 0x00000000B9;				      // mov ecx, 0
	(*code_ptr) += sizeof(uint32_t) + 1;

	// load string address into rdx for log_msg call
	*((uint16_t*)(*code_ptr)) = 0xBA48;                           // mov rdx, 0xADDR
	(*code_ptr) += sizeof(uint16_t);

	*((uint64_t*)(*code_ptr)) = (uint64_t)h->runtime_hook->called_str;
	(*code_ptr) += sizeof(uint64_t);

	for (int i = 0; i < h->runtime_hook->arg_count; i++) {
		if (i < 2) {
			// move args 1 and 2 into r8 and r9 respectively
			// mov [r8, r9], [rbp + [0x10, 0x18]]
			*((uint32_t*)(*code_ptr)) = 0x00008B4C | ((0x10 + i * 0x8) << 24) | ((0x45 + i * 0x8) << 16);
			(*code_ptr) += sizeof(uint32_t);
		}
		else if (i < 4) {
			// move arg 3 and 4 onto the stack position for spilled args
			uint64_t off = ((0x20 + (i - 2) * 0x8));

			// mov rax, [rbp + [0x20, 0x28]]
			*((uint32_t*)(*code_ptr)) = 0x00458B48 | (off << 24);
			(*code_ptr) += sizeof(uint32_t);

			// mov [rsp + [0x20, 0x28]], rax
			*((uint64_t*)(*code_ptr)) = 0x0024448948 | (off << 32);
			(*code_ptr) += sizeof(uint32_t) + 1;
		}
		else {
			// move the spilled args onto the stack position for spilled args
			uint64_t off = (0x30 + (i - 4) * 0x8);

			// mov rax, [rbp + off]
			*((uint32_t*)(*code_ptr)) = 0x00458B48 | (off << 24);
			(*code_ptr) += sizeof(uint32_t);
			// mov [rsp + off], rax
			*((uint64_t*)(*code_ptr)) = 0x0024448948 | (off << 32);
			(*code_ptr) += sizeof(uint32_t) + 1;
		}
	}
}

void rh_prepare_og_func_call(Hook* h, uint8_t** code_ptr) {
	for (int i = 0; i < h->runtime_hook->arg_count; i++) {
		if (i < 4) {
			// mov [rcx, rdx, r8, r9], [rbp + [0x10, 0x18, 0x20, 0x28]]
			//4d 55 45 45
			*((uint32_t*)(*code_ptr)) = 0x00008B00 | ((0x10 + i * 0x8) << 24) | (reg_bytes[i] << 16) | (0x48 + (i > 1) * 0x4);
			(*code_ptr) += sizeof(uint32_t);
		}
		else {
			// mov rax, [rbp + 0x30]
			// mop [rsp + 0x20], rax
			uint64_t off = 0x20 + (i - 4) * 0x8;

			*((uint32_t*)(*code_ptr)) = 0x00458B48 | ((off + 0x10) << 24);
			(*code_ptr) += sizeof(uint32_t);

			*((uint64_t*)(*code_ptr)) = 0x0024448948 | (off << 32);
			(*code_ptr) += sizeof(uint32_t) + 1;
		}
	}
}

void rh_save_og_return_val(Hook* h, uint8_t** code_ptr) {
	UNREFERENCED_PARAMETER(h);
	*((uint32_t*)(*code_ptr)) = 0xF8458948;
	(*code_ptr) += sizeof(uint32_t);
}

void rh_prepare_call_log_msg_second(Hook* h, uint8_t** code_ptr) {
	UNREFERENCED_PARAMETER(h);
	// load LOG_INFO (0) into ecx
	*((uint64_t*)(*code_ptr)) = 0x00000000B9;				      // mov ecx, 0
	(*code_ptr) += sizeof(uint32_t) + 1;

	// load string address into rdx for log_msg call
	*((uint16_t*)(*code_ptr)) = 0xBA48;                           // mov rdx, 0xADDR
	(*code_ptr) += sizeof(uint16_t);

	*((uint64_t*)(*code_ptr)) = (uint64_t)ret_str;
	(*code_ptr) += sizeof(uint64_t);

	*((uint32_t*)(*code_ptr)) = 0xC08949;						  // mov r8, rax
	(*code_ptr) += sizeof(uint32_t) - 1;
}

void rh_function_epilogue(Hook* h, uint8_t** code_ptr) {

	uint8_t stack_size = h->runtime_hook->stackspace;

	// function epilogue
	*((uint32_t*)(*code_ptr)) = 0xF8458B48;                      // mov     rax, [rbp+var_8] ; move ret into rax
	(*code_ptr) += sizeof(uint32_t);

	if (stack_size < 0x80) {
		*((uint32_t*)(*code_ptr)) = 0x00C48348 | (stack_size << 24); // add     rsp, stack_size
		(*code_ptr) += sizeof(uint32_t);
	}
	else {
		*((uint64_t*)(*code_ptr)) = 0x00C48148 | ((uint64_t)stack_size << 24);
		(*code_ptr) += sizeof(uint64_t) - 1;
	}

	*((uint8_t*)(*code_ptr)) = 0x5D;                            // pop     rbp
	(*code_ptr) += sizeof(uint8_t);

	*((uint8_t*)(*code_ptr)) = 0xC3;                            // retn
	(*code_ptr) += sizeof(uint8_t);
}

void rh_print_caller(Hook* h, uint8_t** code_ptr) {
	/*
	mov rcx, rbp
	add rcx, 0x08
	mov rcx, [rcx]
	call print_caller_addr
	*/

	*((uint32_t*)(*code_ptr)) = 0x00E98948; // mov rcx, rbp
	(*code_ptr) += sizeof(uint32_t) - 1; 

	*((uint32_t*)(*code_ptr)) = 0x08C18348; // add rcx, 0x08
	(*code_ptr) += sizeof(uint32_t);

	*((uint32_t*)(*code_ptr)) = 0x00098B48; // mov rcx, [rcx]
	(*code_ptr) += sizeof(uint32_t) - 1;

	rh_call_func(print_caller_addr, code_ptr);
}

void* make_rh_hk_func(Hook* h) {
	void* ret_func = (char*)pages[pages_size - 1] + (func_index * RH_FUNC_SIZE);
	log_msg(LOG_INFO, "Creating runtime hook with %d args at 0x%llX...", h->runtime_hook->arg_count, (uint64_t)ret_func);
	fprintf(rh_logfile, "Creating runtime hook with %d args at 0x%llX...\n", h->runtime_hook->arg_count, (uint64_t)ret_func);

	if (generate_called_string(h) != 0) return NULL;

	uint16_t spilled_args = (h->runtime_hook->arg_count > 2) * 2;
	spilled_args += (h->runtime_hook->arg_count > 4) * (h->runtime_hook->arg_count - 4);
	h->runtime_hook->stackspace = RH_SHADOW_SPACE + 8 + (8 * spilled_args);
	h->runtime_hook->stackspace = (h->runtime_hook->stackspace + 15) & ~15; // align to multiple of 0x10

	log_msg(LOG_INFO, "Calculated stack size as 0x%X", h->runtime_hook->stackspace);
	fprintf(rh_logfile, "Calculated stack size as 0x%X\n", h->runtime_hook->stackspace);

	// generate the function code here
	uint8_t code[512] = { 0 };
	uint8_t* code_ptr = code;
	uint16_t code_size = 0;

	rh_function_prologue(h, &code_ptr);

	rh_move_args_to_stack(h, &code_ptr);


	rh_prepare_call_hook_callback(h, &code_ptr);
	rh_call_func(hook_called_callback, &code_ptr);

	rh_call_func(log_sep, &code_ptr);

	rh_prepare_call_log_msg_first(h, &code_ptr);
	rh_call_func(_log_msg, &code_ptr);

	//rh_call_func(print_caller, &code_ptr);
	//rh_call_func(print_caller2, &code_ptr);
	rh_print_caller(h, &code_ptr); 

	rh_prepare_og_func_call(h, &code_ptr);
	rh_call_deref_func((uint64_t)&h->runtime_hook->og_func, &code_ptr);
	rh_save_og_return_val(h, &code_ptr);

	rh_prepare_call_log_msg_second(h, &code_ptr);
	rh_call_func(_log_msg, &code_ptr);

	rh_call_func(log_sep, &code_ptr);

	rh_function_epilogue(h, &code_ptr);

	code_size = code_ptr - code;

	log_msg(LOG_INFO, "Generated code: %d bytes", code_size);
	fprintf(rh_logfile, "Generated code (%d bytes):\n", code_size);
	for (int i = 0; i < code_size; i++) {
		fprintf(rh_logfile, "0x%.2X ", code[i]);
	}
	fprintf(rh_logfile, "\n");

	fflush(rh_logfile);


	memcpy(ret_func, code, code_size);

	func_index += 1 + (code_size > RH_FUNC_SIZE);

	if (func_index > max_func_index) {
		rh_alloc_new_page();
	}

	return ret_func;
}