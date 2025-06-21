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
    u16 strLenMax;
    u16 widthBorder;
    u16 cursorIdx;
    u16 highlightIdx;
    u16 cursorFrequency;
    u64 frameCounter;
    int fontSize;
    int keyHeld;
    float keyRepeatTimer;
    float keyRepeatDelay;
    float keyRepeatRate;
    bool isTrackpadHeld;
    bool isKeyHeld;
    bool isKeyPressedThisFrame;
    Font font;
    Vector2 cursorPos;
    Vector2 highlightPos;
    String strOutput;
    String strUser;
    String strLabel;
    Clay_Color colorBorder;
} TextBoxData;

typedef struct {
    float yOffset;
    Arena *arena_frame;
    Arena *arena_permanent;
    PlayerMap *player_map;
    PlayerState *player_state;
    NameState *name_state;
    NameChunkState *name_chunk_state;
    Tab tab;
    TextBoxData textBoxData;
    Clay_ElementId last_element_clicked;
} LayoutData;

typedef enum {
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
    CUSTOM_LAYOUT_TEXTBOX
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
        TextBoxData *textBoxData;
    } customData;
} CustomLayoutElement;


const int FONT_ID_BODY_16 = 0;
Font fonts[1];

Clay_Color white = {255, 255, 255, 255};

Clay_Color blue = {33, 31, 41, 255};
Clay_Color blue_ligth = {63, 61, 71, 255};

Clay_Color gray = {90, 90, 90, 255};
Clay_Color gray_light = {140, 140, 140, 255};
Clay_Color gray_lighter = {160, 160, 160, 255};

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
