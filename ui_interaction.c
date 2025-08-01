#ifndef UI_INTERACTION_C
#define UI_INTERACTION_C

#include "raylib/raylib.h"

#include "arena.c"
#include "registration_v2.c"

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
HandleNewTournamentButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_NEW_TOURNAMENT;
    } 
}

void
HandleAddPlayerTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    TextBoxData *textBoxData = &layoutData.addPlayerTextBoxData;

    Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("AddPlayerTextContainer"));
    Clay_ElementData textContainerData = Clay_GetElementData(textContainerId);

    u16 cursorIdx = 0;
    u16 highlightIdx = 0;

    char *subString = arena_push(layoutData.arena_frame, textBoxData->strUser.len + 1);

    float delta;
    float subStringLen = 0;
    float characterLen = MeasureTextEx(textBoxData->font, "A", textBoxData->fontSize, 0).x;
    
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = elementId;
        layoutData.feedbackOnAddPlayerButton = strFeedbackAddPlayer0; 
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
HandleAddTournamentTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    TextBoxData *textBoxData = &layoutData.addTournamentTextBoxData;

    Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("AddTournamentTextContainer"));
    Clay_ElementData textContainerData = Clay_GetElementData(textContainerId);

    u16 cursorIdx = 0;
    u16 highlightIdx = 0;

    char *subString = arena_push(layoutData.arena_frame, textBoxData->strUser.len + 1);

    float delta;
    float subStringLen = 0;
    float characterLen = MeasureTextEx(textBoxData->font, "A", textBoxData->fontSize, 0).x;
    
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = elementId;
        layoutData.feedbackOnAddTournamentButton = strFeedbackAddTournament0;
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
HandlePlayerDeletion(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    Name *player_name = (Name *)data;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        tournament_player_withdraw(&layoutData.tournament_map, layoutData.selectedTournament,
            *player_name, &layoutData.name_state);
    }
}

void
HandleTournamentDeletion(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    Name *tournament_name = (Name *)data;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        tournament_del(&layoutData.tournament_map, *tournament_name, &layoutData.name_state);

        if (are_name_equal(layoutData.selectedTournament, *tournament_name)) {
            name_free(layoutData.selectedTournament, &layoutData.name_state.name_chunk_state);
            memset(&layoutData.selectedTournament, 0, sizeof layoutData.selectedTournament);
        }
    }
}

// void
// DisplayPlayerOptionMenu(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
// {
//     Name *player_name = (Name *)data;
// }

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    TextBoxData *textBoxData = &layoutData.addPlayerTextBoxData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = elementId;

        String str = push_string_cpy(layoutData.arena_frame, textBoxData->strUser);
        string_strip(&str);
        if (str.len == 0) {
            layoutData.feedbackOnAddPlayerButton = strFeedbackAddPlayer1;
        }
        else {
            NameState temp_name_state = name_state_init(layoutData.arena_frame);

            Name player_name = name_init(str, &temp_name_state.name_chunk_state);
            u64 i = tournament_find(&layoutData.tournament_map, layoutData.selectedTournament);
            if (i == 0) {
                layoutData.feedbackOnAddPlayerButton = strFeedbackAddPlayer4;
            }
            else {
                NameNode *player_node = namelist_find(&layoutData.tournament_map.tournaments[i].players_enrolled, player_name);
                if (player_node) {
                    layoutData.feedbackOnAddPlayerButton = strFeedbackAddPlayer2;
                }
                else {
                    tournament_player_enroll(&layoutData.tournament_map,
                        layoutData.selectedTournament, player_name, &layoutData.name_state);
                    layoutData.feedbackOnAddPlayerButton = strFeedbackAddPlayer3;
                }
            }
        }
    }
}

void
HandleAddTournamentButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data)
{
    TextBoxData *textBoxData = &layoutData.addTournamentTextBoxData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData.last_element_clicked = elementId;

        NameState temp_name_state = name_state_init(layoutData.arena_frame);

        String str = push_string_cpy(layoutData.arena_frame, textBoxData->strUser);
        string_strip(&str);
        if (str.len == 0) {
            layoutData.feedbackOnAddTournamentButton = strFeedbackAddTournament1;
        }
        else {
            Name tournament_name = name_init(str, &temp_name_state.name_chunk_state);
            u64 i = tournament_find(&layoutData.tournament_map, tournament_name);
            if (i != 0) {
                layoutData.feedbackOnAddTournamentButton = strFeedbackAddTournamentAlreadyCreated;
            }
            else {
                tournament_add(&layoutData.tournament_map, tournament_name, &layoutData.name_state);
                layoutData.feedbackOnAddTournamentButton = strFeedbackAddTournament3;
            }
        }
    }
}

void
HandleMainWindowInteraction(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_element_clicked = element_id;
        layoutData.playerIdx = -1;
        layoutData.clickedPlayerId = CLAY_IDI("Player", -1);
    }
}

// void
// HandlePlayerSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
//     String *player_string = (String *)data;
//     if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
//         layoutData.last_element_clicked = element_id;
//     }
// }

void
HandlePlayerSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    u32 playerIdx = (u32)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        // layoutData.clickedPlayerId = CLAY_IDI("Player", playerIdx);
        layoutData.playerIdx = playerIdx;
    }
}

void
HandleTournamentSelection(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data)
{
    Name *tournament_name = (Name *)data;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        layoutData.last_element_clicked = element_id;
    }

    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (layoutData.selectedTournament.first_chunk) {
            name_free(layoutData.selectedTournament, &layoutData.name_state.name_chunk_state);
        }
        layoutData.selectedTournament = (
            name_copy(*tournament_name, &layoutData.name_state.name_chunk_state)
        );
    }
}

# if 0

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

#endif

#endif // UI_INTERACTION_C
