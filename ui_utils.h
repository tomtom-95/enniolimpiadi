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
    PlayerFreeList player_free_list;
    Tab tab;
    TextBoxData text_box_data;
    Texture2D profilePicture;
    Clay_ElementId last_clicked;
    Model my_model;
    Texture2D my_texture;
    AddPlayerButtonData add_player_button_data;
} LayoutData;

const int FONT_ID_BODY_16 = 0;

Clay_Color text_color = {255, 255, 255, 255};

// Blueish
Clay_Color background_color = {33, 31, 41, 255};
Clay_Color background_color_on_hover = {63, 61, 71, 255};

// Gray
Clay_Color background_color_window = {90, 90, 90, 255};
Clay_Color background_color_header = {90, 90, 90, 255};

// Lighter gray
Clay_Color header_button_background_color = {140, 140, 140, 255};
Clay_Color header_button_background_color_on_hover = {160, 160, 160, 255};

// Violet
Clay_Color violet = {120, 90, 210, 255};
Clay_Color violet_light = {140, 110, 230, 255};

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

// Camera Raylib_camera;
// Define the camera to look into our 3d world
Camera camera = {.fovy = 30};

typedef enum
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
    CUSTOM_LAYOUT_ELEMENT_CIRCLE
} CustomLayoutElementType;

typedef struct
{
    Model model;
    Texture2D texture;
    float scale;
} CustomLayoutElement_3DModel;

typedef struct
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    } customData;
} CustomLayoutElement;

#endif
