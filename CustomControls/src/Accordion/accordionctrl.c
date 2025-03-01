#include "accordionctrl.h"

#include <stdio.h>
#include <tchar.h>

#define ITEMS_INIT_CAP 20

void (*log_func)(const TCHAR* format, ...) = (void (*)(const TCHAR* format, ...))(void*)_tprintf;
BOOL initialized = FALSE;

typedef struct {
    HWND hHeader;
    HWND hChild;
    int child_height;
    const TCHAR* child_wnd_class;
    const TCHAR* child_wnd_name;
    BOOL expanded;
    void* item_data;
} AccordionItem;

struct Accordion {
    int pos_x;
    int pos_y;

    int scroll_pos;
    int prev_scroll_pos;

    int width;
    int height;

    int header_height;
    int header_width;

    HWND hParent;
    HWND hWnd;
    HINSTANCE hInstance;

    AccordionItem* items;
    size_t items_size;
    size_t items_cap;
};

int _AccordionInit_Internal_(HINSTANCE hInstance, void (*log_funcion)(const TCHAR*, ...)) {
    if (log_funcion)
        log_func = log_funcion;

    if (RegisterAccordionClass(hInstance) != 0) return 1;

    initialized = TRUE;

    return 0;
}

int _AccordionUnInit_Internal_(HINSTANCE hInstance) {
    UnregisterAccordionClass(hInstance);
    initialized = FALSE;

    return 0;
}

int AccordionGetTotalHeight(Accordion* accordion) {
    int total_height = 0;

    for (size_t i = 0; i < accordion->items_size; i++) {
        total_height += accordion->header_height;
        total_height += accordion->items[i].expanded * accordion->items[i].child_height;
    }

    return total_height;
}

void AccordionRepositionItems(Accordion* accordion) {
    int pos_y = accordion->scroll_pos;

    for (size_t i = 0; i < accordion->items_size; i++) {
        AccordionItem* item = &accordion->items[i];

        SetWindowPos(item->hHeader, NULL, 0, pos_y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        pos_y += accordion->header_height;

        if (item->expanded) {
            SetWindowPos(item->hChild, NULL, 0, pos_y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
            pos_y += item->child_height;
        }
    }

    //SetWindowPos(accordion->hWnd, NULL, 0, 0, accordion->header_width, pos_y + 1, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
}

void AccordionScroll(Accordion* accordion, int delta) {
    if (delta == 0) return;

    if (delta > 0 && accordion->scroll_pos == 0) return;

    int total_height = AccordionGetTotalHeight(accordion);
    int max_scroll = -(total_height - accordion->height) - 1;

    if (max_scroll >= 0 && delta < 0) return;

    if (delta < 0 && accordion->scroll_pos == max_scroll) return;

    accordion->prev_scroll_pos = accordion->scroll_pos;

    if (delta < 0) {
        accordion->scroll_pos = max(accordion->scroll_pos + delta, max_scroll);
    }
    else {
        accordion->scroll_pos = min(accordion->scroll_pos + delta, 0);
    }

    //int scroll_delta = accordion->scroll_pos - accordion->prev_scroll_pos;

    AccordionRepositionItems(accordion);
    InvalidateRect(accordion->hWnd, NULL, TRUE);
}

void AccordionToggleItem(Accordion* accordion, int index) {
    AccordionItem* item = &accordion->items[index];
    TCHAR titlebuf[100];
    GetWindowText(item->hHeader, titlebuf, sizeof(titlebuf) / sizeof(*titlebuf));
    if (!item->expanded) {
        titlebuf[0] = TEXT('V');
        ShowWindow(item->hChild, SW_SHOW);
        item->expanded = TRUE;
    }
    else {
        titlebuf[0] = TEXT('>');
        ShowWindow(item->hChild, SW_HIDE);
        item->expanded = FALSE;
    }

    SetWindowText(item->hHeader, titlebuf);

    AccordionRepositionItems(accordion);

    // redraw the windows to avoid old buttons
    InvalidateRect(accordion->hWnd, NULL, TRUE);
}

LRESULT CALLBACK AccordionWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrush = NULL;

    Accordion* accordion = (Accordion*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  
    switch (uMsg) {
    case WM_CREATE:
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        accordion = (Accordion*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)accordion);
    }
    break;
    case WM_COMMAND:
        AccordionToggleItem(accordion, LOWORD(wParam));
        SetFocus(NULL);

        break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;

        // Set the background mode to transparent
        SetBkMode(hdcStatic, TRANSPARENT);

        // Return the parent window's background brush
        if (!hBrush) {
            hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // Matches default background
        }
        return (LRESULT)hBrush;
    }
    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 20; // 20 is scrollpixels, make it a #define later
        AccordionScroll(accordion, delta);
        //InvalidateRect(accordion->hWnd, NULL, TRUE);
        //UpdateWindow(accordion->hWnd);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1)); // Clear the area
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int RegisterAccordionClass(HINSTANCE hInst) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = AccordionWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WC_ACCORDION;
    wc.hIconSm = NULL;

    if (!RegisterClassEx(&wc)) {
        log_func(TEXT("Failed to register accordion window (RegisterClassEx): %ld"), GetLastError());
        return 1;
    }
    return 0;
}

void UnregisterAccordionClass(HINSTANCE hInst) {
    UnregisterClass(WC_ACCORDION, hInst);
}

Accordion* AccordionCreate(
    int pos_x, int pos_y,
    int width, int height,
    int header_height,
    HWND hParent, HINSTANCE hInstance)
{
    if (!initialized) {
        log_func(TEXT("CustomControls not initialized. Call `CustomControlsInit` first"));
        return NULL;
    }

    Accordion* accordion = malloc(sizeof(Accordion));
    if (!accordion) {
        log_func(TEXT("Failed to allocate space for accordion (malloc)"));
        return NULL;
    }

    accordion->hParent = hParent;
    accordion->hInstance = hInstance;

    accordion->width = width;
    accordion->height = height;

    accordion->header_height = header_height;
    accordion->header_width = width - 1;

    accordion->pos_x = pos_x;
    accordion->pos_y = pos_y;

    accordion->scroll_pos = 0;
    accordion->prev_scroll_pos = 0;

    accordion->items_cap = ITEMS_INIT_CAP;
    accordion->items = malloc(sizeof(AccordionItem) * accordion->items_cap);
    accordion->items_size = 0;

    if (!accordion->items) {
        log_func(TEXT("Failed to allocate space for accordion->items (malloc)"));
        free(accordion);
        return NULL;
    }

    accordion->hWnd = CreateWindowEx(
        0,
        WC_ACCORDION, NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        pos_x, pos_y,
        width, height,
        hParent, NULL, hInstance, accordion
    );

    if (!accordion->hWnd) {
        log_func(TEXT("Failed to create the accordion window: %ld"), GetLastError());
        AccordionDestroy(accordion);
        return NULL;
    }

    return accordion;
}

void AccordionDestroy(Accordion* accordion) {
    free(accordion->items);
    free(accordion);
}

int AccordionAddItem(Accordion* accordion, void* item,
    const TCHAR* item_wnd_class, const TCHAR* item_wnd_name,
    const TCHAR* item_header_text, DWORD dwChildStyles)
{
    AccordionItem* aci = &accordion->items[accordion->items_size];
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    aci->item_data = item;
    aci->expanded = FALSE;
    aci->child_wnd_class = item_wnd_class;
    aci->child_wnd_name = item_wnd_name;

    TCHAR headerbuf[100] = {0};
    if (item_header_text) {
        _stprintf_s(headerbuf, sizeof(headerbuf) / sizeof(*headerbuf), TEXT("> %s"), item_header_text);
    }
    aci->hHeader = CreateWindow(
        TEXT("Button"), item_header_text ? headerbuf : TEXT("> Item"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_LEFT,
        0, (int)accordion->items_size * accordion->header_height,
        accordion->header_width - 1, accordion->header_height,
        accordion->hWnd, (HMENU)(accordion->items_size),
        accordion->hInstance, NULL
    );

    if (!aci->hHeader) {
        log_func(TEXT("Failed to create accordion item header window: %ld"), GetLastError());
        return 1;
    }

    SendMessage(aci->hHeader, WM_SETFONT, (WPARAM)hFont, TRUE);

    aci->hChild = CreateWindowEx(
        0,
        item_wnd_class, item_wnd_name,
        WS_CHILD | dwChildStyles,
        0, 0,
        accordion->header_width, 0,
        accordion->hWnd, NULL, accordion->hInstance,
        aci->item_data
    );

    if (!aci->hChild) {
        log_func(TEXT("Failed to create accordion item child window: %ld"), GetLastError());
        return 1;
    }

    SendMessage(aci->hChild, WM_SETFONT, (WPARAM)hFont, TRUE);

    aci->child_height = (int)SendMessage(aci->hChild, WM_GET_CHILD_HEIGHT, 0, 0);
    if (!aci->child_height) aci->child_height = ACCORDION_DEF_CHILD_HEIGHT;

    if (!SetWindowPos(aci->hChild, NULL, 0, 0, accordion->header_width, aci->child_height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE)) {
        log_func(TEXT("Failed to set the accordion item child window pos: %ld"), GetLastError());
        return 1;
    }

    accordion->items_size++;
    AccordionRepositionItems(accordion);
    // TODO: check if items size is larger than cap and increase if necessary

    return 0;
}

int AccordionAddItemArray(Accordion* accordion,
    void* items, int items_count, int items_stride, const TCHAR* item_wnd_class,
    ItemTextCallback cb_wnd_name, ItemTextCallback cb_header_text, DWORD dwChildStyles)
{
    for (int i = 0; i < items_count; i++) {
        void* item = (char*)items + i * items_stride;

        if (AccordionAddItem(accordion, item, item_wnd_class,
            cb_wnd_name ? cb_wnd_name(item) : NULL,
            cb_header_text ? cb_header_text(item) : NULL,
            dwChildStyles
        ) != 0) return 1;
    }

    return 0;
}