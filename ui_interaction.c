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

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_clicked = elementId;
    }

    if ((user_data->last_clicked).id == elementId.id) {
        text_box_data->frame_counter++;

        int MAX_INPUT_CHAR = 126;

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
            if (text_box_data->backspace_key_state == BACKSPACE_NOT_PRESSED) {
                if (--(text_box_data->letter_count) < 0) {
                    text_box_data->letter_count = 0;
                }
                (text_box_data->name)[text_box_data->letter_count] = '\0';
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
                    if (--(text_box_data->letter_count) < 0) {
                        text_box_data->letter_count = 0;
                    }
                    (text_box_data->name)[text_box_data->letter_count] = '\0';
                } 
            }
        }
        else {
            text_box_data->backspace_key_state = BACKSPACE_NOT_PRESSED;
        }

        int key = GetCharPressed();
        while (key > 0) {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (text_box_data->letter_count < MAX_INPUT_CHAR))
            {
                (text_box_data->name)[text_box_data->letter_count] = (u8)key;
                (text_box_data->name)[++(text_box_data->letter_count)] = '\0';
            }
            key = GetCharPressed();  // Check next character in the queue
        }
    } // if((user_data->last_clicked).id == elementId.id)
    else {
        // TODO: should show what the user was typing (text_box_data->name) without blinking cursor
        ;
    }
}

void
HandleAddPlayerButtonInteraction(
    Clay_ElementId element_id,
    Clay_PointerData pointer_data,
    intptr_t data
) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (user_data->text_box_data.letter_count != 0) {
            String player_name = string_from_cstring(user_data->text_box_data.name);
            player_create(
                user_data->arena_permanent,
                user_data->player_map,
                player_name,
                &(user_data->player_free_list)
            );
            // TODO: display feedback message: "Player has been registered"
        }
        else {
            // TODO: print a message: cannot register player with no name
            ;
        }
    }

    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->add_player_button_data.add_player_button_state = ADD_PLAYER_BUTTON_PRESSED;
    }
    if (pointer_data.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
        user_data->add_player_button_data.add_player_button_state = ADD_PLAYER_BUTTON_NOT_PRESSED;
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
        user_data->last_clicked = element_id;
    }
}

#endif
