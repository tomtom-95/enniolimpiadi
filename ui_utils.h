#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "arena.c"
#include "registration.c"

#include "raylib/raylib.h"
#include "clay.h"

typedef enum {
    TAB_PLAYERS = 0,
    TAB_TOURNAMENTS,
    TAB_NEW_PLAYER,
    TAB_NEW_TOURNAMENT
} Tab;

typedef struct {
    bool someKeyPressed;
    u32 highlight_start;
    u32 highlight_end;
    float scrollOffset;
    float click_position_start;
    float click_position_end;
    float frame_timer_text_highlight;
    float highlight_frame_timer;
    int highlightWidthPixels;
    Clay_Vector2 cursor_vector;
    u32 cursor_position;
    u32 max_str_len;
    u64 frame_counter;
    s32 fontSize;
    double frame_timer;
    int font_id;
    String str;
    float floatingLabelYOffset;
    enum TextBoxDataState {
        CLICK_STATE,
        HIGHLIGHT_STATE,
        HIGHLIGHT_STATE_IDLE
    } textBoxDataState;
    enum BackspaceKeyState {
        BACKSPACE_NOT_PRESSED = 0,
        BACKSPACE_FIRST,
        BACKSPACE_SECOND,
    } backspace_key_state;
    Clay_Color colorBorder;
} TextBoxData;

typedef struct {
    u16 strLenMax;
    u16 widthBorder;
    u16 cursorIdx;
    u16 highlightIdx;
    u16 cursorFrequency;
    u64 frameCounter;
    int fontSize;
    float backspaceTimer;
    float backspaceRepeatDelay;
    float backspaceRepeatRate;
    bool backspaceHeld;
    bool trackpadHeld;
    Font font;
    Vector2 cursorPos;
    Vector2 highlightPos;
    String strOutput;
    String strUser;
    String strLabel;
    Clay_Color colorBorder;
} TextBoxDataV2;

typedef struct {
    enum AddPlayerButtonState {
        ADD_PLAYER_BUTTON_NOT_PRESSED = 0,
        ADD_PLAYER_BUTTON_PRESSED,
    } add_player_button_state;
} AddPlayerButtonData;

typedef struct {
    float yOffset;
    Arena *arena_frame;
    Arena *arena_permanent;
    PlayerMap *player_map;
    PlayerState *player_state;
    NameState *name_state;
    NameChunkState *name_chunk_state;
    Tab tab;
    TextBoxData text_box_data;
    TextBoxDataV2 textBoxDataV2;
    Clay_ElementId last_element_clicked;
    AddPlayerButtonData add_player_button_data;
} LayoutData;

typedef enum {
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
    CUSTOM_LAYOUT_TEXTBOX,
    CUSTOM_LAYOUT_TEXTBOX_V2
} CustomLayoutElementType;

typedef struct {
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct {
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
        TextBoxDataV2 *textBoxDataV2;
    } customData;
} CustomLayoutElement;


const int FONT_ID_BODY_16 = 0;
Font fonts[1];

Clay_Color white = {255, 255, 255, 255};

// Blueish
Clay_Color blue = {33, 31, 41, 255};
Clay_Color blue_ligth = {63, 61, 71, 255};

// Gray
Clay_Color gray = {90, 90, 90, 255};
Clay_Color gray_light = {140, 140, 140, 255};
Clay_Color gray_lighter = {160, 160, 160, 255};

// Violet
Clay_Color violet = {120, 90, 210, 255};
Clay_Color violet_light = {140, 110, 230, 255};

Clay_Sizing layoutExpand = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };

Clay_String
Clay_String_from_String(String string) {
    Clay_String clay_string = {
        .isStaticallyAllocated = false,
        .length = (s32)string.len,
        .chars = (const char *)string.str
    };
    return clay_string;
}

#endif
