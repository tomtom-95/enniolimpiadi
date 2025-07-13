#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "arena.c"
#include "registration.c"

#include "raylib/raylib.h"
#include "clay.h"

static Clay_ElementId g_lastClicked = {0};

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
    RegistrationMap *player_map;
    RegistrationMap *tournament_map;
    RegistrationState *registration_state;
    NameState *name_state;
    NameChunkState *name_chunk_state;
    Tab tab;
    String feedbackOnAddPlayerButton;
    String feedbackOnAddTournamentButton;
    Name *selectedTournamentChart;
    TextBoxData addPlayerTextBoxData;
    TextBoxData addTournamentTextBoxData;
    Clay_ElementId last_element_clicked;
    s32 playerIdx;
    Clay_ElementId clickedPlayerId;
} LayoutData;

typedef struct {
    RegistrationMap *primary_map;
    RegistrationMap *link_map;
    String primary_str;
    String link_str;
} RegistrationPair;

typedef enum {
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
    CUSTOM_LAYOUT_TEXTBOX
} CustomLayoutElementType;

typedef enum {
    ADD_PLAYER_TEXTBOX,
    ADD_TOURNAMENT_TEXTBOX
} TextBoxType;

typedef struct {
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct {
    CustomLayoutElementType type;
    TextBoxType textBoxType;
    union {
        CustomLayoutElement_3DModel model;
        TextBoxData *textBoxData;
    } customData;
} CustomLayoutElement;


const int FONT_ID_BODY_16 = 0;
Font fonts[1];

Clay_Color black = {0, 0, 0, 255};
Clay_Color white = {255, 255, 255, 255};

Clay_Color blue = {33, 31, 41, 255};
Clay_Color blue_ligth = {63, 61, 71, 255};

Clay_Color gray = {90, 90, 90, 255};
Clay_Color gray_light = {140, 140, 140, 255};
Clay_Color gray_lighter = {160, 160, 160, 255};

Clay_Color violet = {120, 90, 210, 255};
Clay_Color violet_light = {140, 110, 230, 255};

Clay_Color yellow = {255, 255, 0, 255};

Clay_Sizing layoutExpand = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };

Clay_String
Clay_String_from_String(String string)
{
    Clay_String clay_string = {
        .isStaticallyAllocated = false,
        .length = (s32)string.len,
        .chars = (const char *)string.str
    };
    return clay_string;
}

String strFeedbackAddPlayer1 = string_from_lit_comp("Cannot create player without name");
String strFeedbackAddPlayer2 = string_from_lit_comp("Player already created");
String strFeedbackAddPlayer3 = string_from_lit_comp("Player successfully created");

String strFeedbackAddTournament1 = string_from_lit_comp("Cannot create tournament without name");
String strFeedbackAddTournament2 = string_from_lit_comp("Tournament already created");
String strFeedbackAddTournament3 = string_from_lit_comp("Tournament successfully created");

Clay_String addPlayerTextBoxStr = CLAY_STRING("AddPlayerTextBox");
Clay_String addTournamentTextBoxStr = CLAY_STRING("AddTournamentTextBox");

Clay_String floatingMenuStrId = CLAY_STRING("FloatingMenu");
Clay_String floatingMenuEnrollStrId = CLAY_STRING("FloatingMenuEnroll");
Clay_String floatingMenuWithdrawStrId = CLAY_STRING("FloatingMenuWithdraw");
Clay_String floatingMenuDeleteStrId = CLAY_STRING("FloatingMenuDelete");
Clay_String floatingMenuEnrollListStrId = CLAY_STRING("FloatingMenuEnrollList");
Clay_String floatingMenuWithdrawListStrId = CLAY_STRING("FloatingMenuWithdrawList");

LayoutData layoutData;

#endif // UI_UTILS_H
