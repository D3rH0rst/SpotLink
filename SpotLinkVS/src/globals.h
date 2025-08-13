#pragma once

#include <Windows.h>
#include <CustomControls.h>
#include <stdio.h>

typedef struct {
	HMODULE hDll;
	HANDLE hSpotify;
	// HANDLE hLibcef; /* Not used as of now */
#ifdef CONSOLE
	FILE *pConsole;
#endif
#ifdef LOGFILE
	FILE *pLogfile;
#endif
	HWND spotlinkHwnd;
	HWND logHwnd;
	HWND logLabelHwnd;
	Accordion *pAccordion;
	DLGTEMPLATE *pRHDlgTemplate;
	// const int window_width;  // maybe have this to make window resizable later on
	// const int window_height; // maybe have this to make window resizable later on
} SpotLinkCtx;

extern SpotLinkCtx *g_pCtx;