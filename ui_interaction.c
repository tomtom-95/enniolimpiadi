#ifndef UI_INTERACTION_C
#define UI_INTERACTION_C

#include "raylib/raylib.h"

#include "arena.c"
#include "registration.c"

#include "clay.h"
#include "ui_utils.h"

void
HandlePlayersButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_PLAYERS;
    }
}

void
HandleTournamentsButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData  = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_TOURNAMENTS;
    }
}

void
HandleNewPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_NEW_PLAYER;
    } 
}

void
HandleNewTournamentButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_NEW_TOURNAMENT;
    } 
}

void
HandleAddPlayerTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &layoutData->addPlayerTextBoxData;

    Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("AddPlayerTextContainer"));
    Clay_ElementData textContainerData = Clay_GetElementData(textContainerId);

    u16 cursorIdx = 0;
    u16 highlightIdx = 0;

    char *subString = arena_push(layoutData->arena_frame, textBoxData->strUser.len + 1);

    float delta;
    float subStringLen = 0;
    float characterLen = MeasureTextEx(textBoxData->font, "A", textBoxData->fontSize, 0).x;
    
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
        textBoxData->frameCounter = 0;

        delta = pointerData.position.x - textContainerData.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && cursorIdx < textBoxData->strUser.len) {
            ++cursorIdx;

            memcpy(subString, textBoxData->strUser.str, cursorIdx);
            subString[cursorIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxData->font, subString, textBoxData->fontSize, 0).x;
        }

        textBoxData->cursorIdx = cursorIdx;
        textBoxData->highlightIdx = textBoxData->cursorIdx;
    }
    else if (pointerData.state == CLAY_POINTER_DATA_PRESSED) {
        textBoxData->isTrackpadHeld = true;

        delta = pointerData.position.x - textContainerData.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && highlightIdx < textBoxData->strUser.len) {
            ++highlightIdx;

            memcpy(subString, textBoxData->strUser.str, highlightIdx);
            subString[highlightIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxData->font, subString, textBoxData->fontSize, 0).x;
        }

        textBoxData->highlightIdx = highlightIdx;
    }
}

void
HandleAddTournamentTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &layoutData->addTournamentTextBoxData;

    Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("AddTournamentTextContainer"));
    Clay_ElementData textContainerData = Clay_GetElementData(textContainerId);

    u16 cursorIdx = 0;
    u16 highlightIdx = 0;

    char *subString = arena_push(layoutData->arena_frame, textBoxData->strUser.len + 1);

    float delta;
    float subStringLen = 0;
    float characterLen = MeasureTextEx(textBoxData->font, "A", textBoxData->fontSize, 0).x;
    
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
        textBoxData->frameCounter = 0;

        delta = pointerData.position.x - textContainerData.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && cursorIdx < textBoxData->strUser.len) {
            ++cursorIdx;

            memcpy(subString, textBoxData->strUser.str, cursorIdx);
            subString[cursorIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxData->font, subString, textBoxData->fontSize, 0).x;
        }

        textBoxData->cursorIdx = cursorIdx;
        textBoxData->highlightIdx = textBoxData->cursorIdx;
    }
    else if (pointerData.state == CLAY_POINTER_DATA_PRESSED) {
        textBoxData->isTrackpadHeld = true;

        delta = pointerData.position.x - textContainerData.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && highlightIdx < textBoxData->strUser.len) {
            ++highlightIdx;

            memcpy(subString, textBoxData->strUser.str, highlightIdx);
            subString[highlightIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxData->font, subString, textBoxData->fontSize, 0).x;
        }

        textBoxData->highlightIdx = highlightIdx;
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &layoutData->addPlayerTextBoxData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;

        String str = push_string_cpy(layoutData->arena_frame, textBoxData->strUser);
        string_strip(&str);
        if (str.len == 0) {
            layoutData->feedbackOnAddPlayerButton = strFeedbackAddPlayer1;
        }
        else {
            Registration *player = registration_find(layoutData->player_map, str);
            if (player) {
                layoutData->feedbackOnAddPlayerButton = strFeedbackAddPlayer2;
            }
            else {
                registration_create(layoutData->player_map, str, layoutData->registration_state,
                    layoutData->name_state, layoutData->name_chunk_state);
                layoutData->feedbackOnAddPlayerButton = strFeedbackAddPlayer3;
            }
        }
    }
}

void
HandleAddTournamentButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &layoutData->addTournamentTextBoxData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;

        String str = push_string_cpy(layoutData->arena_frame, textBoxData->strUser);
        string_strip(&str);
        if (str.len == 0) {
            layoutData->feedbackOnAddTournamentButton = strFeedbackAddTournament1;
        }
        else {
            Registration *tournament = registration_find(layoutData->tournament_map, str);
            if (tournament) {
                layoutData->feedbackOnAddTournamentButton = strFeedbackAddTournament2;
            }
            else {
                registration_create(layoutData->tournament_map, str, layoutData->registration_state,
                    layoutData->name_state, layoutData->name_chunk_state);
                layoutData->feedbackOnAddTournamentButton = strFeedbackAddTournament3;
            }
        }
    }
}

void
HandleMainWindowInteraction(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_element_clicked = element_id;
    }
}

void
HandlePlayerSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    String *player_string = (String *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = element_id;
        // layoutData.last_player_clicked = *player_string;
    }
}

void
HandleTournamentSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    String *tournament_string = (String *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = element_id;
        // layoutData.last_tournament_clicked = *tournament_string;
    }
}

void
HandleEnrollSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    RegistrationPair *pair = (RegistrationPair *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        registration_enroll_(pair->primary_map, pair->link_map, pair->primary_str,
            pair->link_str, layoutData.name_state, layoutData.name_chunk_state);
    }
}

void
HandleWithdrawSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    RegistrationPair *pair = (RegistrationPair *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        registration_withdraw_(pair->primary_map, pair->link_map, pair->primary_str,
            pair->link_str, layoutData.name_state, layoutData.name_chunk_state);
    }
}

void
HandleDeleteRegistrationInteraction(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    RegistrationPair *pair = (RegistrationPair *)data; 
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        registration_delete_(pair->primary_map, pair->link_map, pair->primary_str,
            layoutData.registration_state, layoutData.name_state, layoutData.name_chunk_state);
    }
}

void
HandleDeleteTournamentInteraction(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        String str_tournament_name = {
            .str = arena_push(layoutData->arena_frame, layoutData->last_tournament_clicked.len),
            .len = layoutData->last_tournament_clicked.len
        };
        memcpy(str_tournament_name.str, layoutData->last_tournament_clicked.str, layoutData->last_tournament_clicked.len);

        tournament_delete(layoutData->player_map, layoutData->tournament_map, str_tournament_name,
            layoutData->registration_state, layoutData->name_state, layoutData->name_chunk_state);
    }
}

#endif
