#ifndef UI_INTERACTION_C
#define UI_INTERACTION_C

#include "raylib/raylib.h"

#include "arena.c"
#include "registration.c"

#include "clay.h"
#include "ui_utils.h"

void
HandlePlayersButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_PLAYERS;
    }
}

void
HandleTournamentsButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_TOURNAMENTS;
    }
}

void
HandleNewPlayerButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_NEW_PLAYER;
    } 
}

void
HandleNewTournamentButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_NEW_TOURNAMENT;
    } 
}

void
HandleCustomElementButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_CUSTOM;
    } 
}

void
HandlePlayerTextBoxInteraction(
    Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data
) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
    }
}

void
HandleFloatingLabelInteraction(
    Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data
) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
    }
}

// void
// HandlePlayerTextBoxInputs(TextBoxData *textBoxData) {
//     String *str = &(textBoxData->str);
//     ++(textBoxData->frame_counter);
//     if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
//         if (textBoxData->backspace_key_state == BACKSPACE_NOT_PRESSED) {
//             if (str->len != 0) {
//                 --(str->len);
//             }
//             textBoxData->backspace_key_state = BACKSPACE_FIRST; 
//         }
//         else if (textBoxData->backspace_key_state == BACKSPACE_FIRST) {
//             textBoxData->frame_timer += GetFrameTime();
//             if (textBoxData->frame_timer > 0.35f) {
//                 textBoxData->frame_timer = 0;
//                 textBoxData->backspace_key_state = BACKSPACE_SECOND;
//             } 
//         }
//         else {
//             textBoxData->frame_timer += GetFrameTime();
//             if (textBoxData->frame_timer > 0.03f) {
//                 textBoxData->frame_timer = 0;
//                 if (str->len != 0) {
//                     --(str->len);
//                 }
//             } 
//         }
//     }
//     else {
//         textBoxData->backspace_key_state = BACKSPACE_NOT_PRESSED;
//     }
//     int key = GetCharPressed();
//     while (key > 0) {
//         // NOTE: Only allow keys in range [32..125]
//         if ((key >= 32) && (key <= 125) && (str->len < textBoxData->max_str_len)) {
//             str->str[(str->len)++] = (u8)key;
//         }
//         key = GetCharPressed();  // Check next character in the queue
//     }
// }

void
HandleAddPlayerButtonInteraction(
    Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data
) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
    }
}

void
HandleMainWindowInteraction(
    Clay_ElementId element_id,
    Clay_PointerData pointer_data,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_element_clicked = element_id;
    }
}

#endif
