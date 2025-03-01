#include "CustomControls.h"

int _AccordionInit_Internal_(HINSTANCE hInstance, void (*log_funcion)(const char*, ...));
int _AccordionUnInit_Internal_(HINSTANCE hInstance);

int CustomControlsInit(HINSTANCE hInstance, CCTRL_LOG log_function) {
	if (_AccordionInit_Internal_(hInstance, log_function) != 0) return 1;

	return 0;
}

int CustomControlsUnInit(HINSTANCE hInstance) {
	if (_AccordionUnInit_Internal_(hInstance) != 0) return 1;

	return 0;
}