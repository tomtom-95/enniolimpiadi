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
    TAB_NEW_TOURNAMENT,
    TAB_CUSTOM
} Tab;

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
    TextBoxData text_box_data;
    Texture2D profilePicture;
    Clay_ElementId last_clicked;
    Model my_model;
} LayoutData;

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

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

typedef enum
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct
{
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    } customData;
} CustomLayoutElement;

#endif
