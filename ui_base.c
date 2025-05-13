#ifndef UI_BASE_C
#define UI_BASE_C

#include "clay.h"
#include "arena.c"

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

#endif // UI_BASE_C
