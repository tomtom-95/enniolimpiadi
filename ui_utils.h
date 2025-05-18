#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "arena.c"
#include "registration.c"

#include "clay.h"

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
    TAB_PLAYERS = 0,
    TAB_TOURNAMENTS,
    TAB_NEW_PLAYER,
    TAB_NEW_TOURNAMENT
} Tab;

typedef enum {
    MAIN_WINDOW = 0,
    WINDOW_NEW_PLAYER,
    WINDOW_NEW_TOURNAMENT
} Window;

typedef struct {
    double frame_timer;
    u64 frame_counter;
    s32 letter_count;
    u8 name[128];
    
    enum BackspaceKeyState {
        BACKSPACE_NOT_PRESSED = 0,
        BACKSPACE_FIRST,
        BACKSPACE_SECOND,
    } backspace_key_state;
} TextBoxData;

typedef struct {
    float yOffset;
    Arena *arena_frame;
    Arena *arena_permanent;
    PlayerMap *player_map;
    PlayerFreeList player_free_list;
    Tab tab;
    String *test_string;
    TextBoxData text_box_data;
} LayoutData;

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

Clay_Color text_color = {255, 255, 255, 255};

Clay_Color background_color = {33, 31, 41, 255};
Clay_Color background_color_on_hover = {63, 61, 71, 255};

// Gray
Clay_Color background_color_window = {90, 90, 90, 255};
Clay_Color background_color_header = {90, 90, 90, 255};

// Lighter gray
Clay_Color header_button_background_color = {140, 140, 140, 255};
Clay_Color header_button_background_color_on_hover = {160, 160, 160, 255};

// Violet
Clay_Color add_player_button_color = {120, 90, 210, 255};
Clay_Color add_player_button_color_on_hover = {140, 110, 230, 255};

#endif
