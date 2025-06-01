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
HandleTextBoxInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    TextBoxData *text_box_data = &(user_data->text_box_data);
    String *str = &text_box_data->str;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_clicked = elementId;
    }
    text_box_data->frame_counter++;

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
        if (text_box_data->backspace_key_state == BACKSPACE_NOT_PRESSED) {
            if (--(str->len) == 0) {
                String tmp = string_from_cstring_lit("Enter a player name");
                string_cpy(str, &tmp);
            }
            text_box_data->backspace_key_state = BACKSPACE_FIRST; 
        }
        else if (text_box_data->backspace_key_state == BACKSPACE_FIRST) {
            text_box_data->frame_timer += GetFrameTime();
            if (text_box_data->frame_timer > 0.35f) {
                text_box_data->frame_timer = 0;
                text_box_data->backspace_key_state = BACKSPACE_SECOND;
            } 
        }
        else {
            text_box_data->frame_timer += GetFrameTime();
            if (text_box_data->frame_timer > 0.03f) {
                text_box_data->frame_timer = 0;
                if (--(str->len) == 0) {
                    String tmp = string_from_cstring_lit("Enter a player name");
                    string_cpy(str, &tmp);
                }
            } 
        }
    }
    else {
        text_box_data->backspace_key_state = BACKSPACE_NOT_PRESSED;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (string_cmp(*str, string_from_cstring_lit("Enter a player name"))) {
            str->str[0] = (u8)key;
            str->len = 1;
        }
        else {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (str->len < text_box_data->max_str_len)) {
                str->str[(str->len)++] = (u8)key;
            }
        }
        key = GetCharPressed();  // Check next character in the queue
    }
}

// void
// HandleAddPlayerButtonInteraction(
//     Clay_ElementId element_id,
//     Clay_PointerData pointer_data,
//     intptr_t data
// ) {
//     LayoutData *user_data = (LayoutData *)data;
//     if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
//         if (user_data->text_box_data.letter_count != 0) {
//             String player_name = string_from_cstring(user_data->text_box_data.name);
//             player_create(
//                 user_data->name_chunk_state,
//                 user_data->player_state,
//                 user_data->player_map,
//                 player_name
//             );
//             // TODO: display feedback message: "Player has been registered"
//         }
//         else {
//             // TODO: print a message: cannot register player with no name
//             ;
//         }
//     }
//     if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
//         user_data->add_player_button_data.add_player_button_state = ADD_PLAYER_BUTTON_PRESSED;
//     }
//     if (pointer_data.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
//         user_data->add_player_button_data.add_player_button_state = ADD_PLAYER_BUTTON_NOT_PRESSED;
//     }
// }

void
HandleMainWindowInteraction(
    Clay_ElementId element_id,
    Clay_PointerData pointer_data,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_clicked = element_id;
    }
}

#endif
