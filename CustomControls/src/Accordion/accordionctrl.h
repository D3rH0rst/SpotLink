#pragma once

#include <Windows.h>

#define WC_ACCORDION TEXT("AccortionCtrl")
#define WM_GET_CHILD_HEIGHT (WM_USER + 1)
#define ACCORDION_DEF_CHILD_HEIGHT 40

typedef struct Accordion Accordion;

typedef const TCHAR* (*ItemTextCallback)(void*);

int RegisterAccordionClass(HINSTANCE hInst);
void UnregisterAccordionClass(HINSTANCE hInst);

Accordion* AccordionCreate(
    int pos_x, int pos_y,
    int width, int height,
    int header_height,
    HWND hParent, HINSTANCE hInstance);

void AccordionDestroy(Accordion* accordion);

int AccordionAddItem(Accordion* accordion, void* item,
    const TCHAR* item_wnd_class, const TCHAR* item_wnd_name,
    const TCHAR* header_text, DWORD dwChildStyles);

int AccordionAddItemArray(Accordion* accordion,
    void* items, int items_count, int items_stride, const TCHAR* item_wnd_class,
    ItemTextCallback cb_wnd_name, ItemTextCallback cb_header_text, DWORD dwChildStyles);
