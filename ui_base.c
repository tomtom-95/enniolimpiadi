#ifndef UI_BASE_C
#define UI_BASE_C

#include "clay.h"
#include "arena.c"
#include "registration.c"

typedef struct {
    Clay_String title;
} Document;

typedef struct {
    Clay_String content;
} ButtonClickMe;

typedef struct {
    Document *documents;
    uint32_t length;
} DocumentArray;

typedef struct {
    ButtonClickMe *buttonsClickMe;
    uint32_t length;
} ButtonClickMeArray;

Document documentsRaw[2];
ButtonClickMe buttonsClickMeRaw[2];

DocumentArray documents = {
    .length = 2,
    .documents = documentsRaw
};

ButtonClickMeArray buttonsClickMe = {
    .length = 2,
    .buttonsClickMe = buttonsClickMeRaw
};

typedef enum {
    PLAYERS_TAB = 0,
    TOURNAMENTS_TAB = 1
} Tab;

typedef enum {
    MAIN_WINDOW = 0,
    WINDOW_NEW_PLAYER,
    WINDOW_NEW_TOURNAMENT
} Window;

typedef struct {
    float yOffset;
    Arena *arena_frame;
    Arena *arena_permanent;
    PlayerMap *player_map;
    String *test_string;
    bool textbox_pressed;
    int lastkeypressed;
    int letterCount;
    char name[32];
    // TODO: put in this struct all the other stuff related on what has been pressed
} ClayVideoDemo_Data;

typedef struct {
    Tab requestedTab;
    Tab* selectedTab;
} SidebarClickData;

typedef struct {
    Tab requestedTab;
    Tab* selectedTab;
} ButtonClickMeData;

// Define a structure to hold the text display data
typedef struct {
    const char *normalText;
    const char *pressedText;
    bool isPressed;
} TextButtonData;

// Globals
const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = { 255, 255, 255, 255};

Tab open_tab;
Window open_window;


void
RenderHeaderButton(Clay_String text) {
    CLAY({
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void
RenderDropdownMenuItem(Clay_String text) {
    CLAY({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void
HandleSidebarInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    Tab clickedTab = (Tab)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        open_tab = clickedTab;
    }
}

void
HandleTextButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    Window clickData = (Window)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        open_window = clickData; 
    } 
}

#endif // UI_BASE_C
