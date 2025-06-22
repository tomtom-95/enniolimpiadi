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
    TextBoxData *textBoxData = &(layoutData->textBoxData);

    Clay_ElementId playerTextBoxId = Clay_GetElementId(CLAY_STRING("PlayerTextBox"));
    Clay_ElementData playerTextBoxElementData = Clay_GetElementData(playerTextBoxId);
    Clay_BoundingBox playerTextBoxBoundingBox = playerTextBoxElementData.boundingBox;

    s32 fontSize = textBoxData->fontSize;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->tab = TAB_NEW_PLAYER;
    } 
}

void
HandleNewTournamentButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_NEW_TOURNAMENT;
    } 
}

void
HandleTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &layoutData->textBoxData;

    Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("TextContainer"));
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

        layoutData->textBoxData.cursorIdx = cursorIdx;
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

        layoutData->textBoxData.highlightIdx = highlightIdx;
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData textBoxData = layoutData->textBoxData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;

        String str = push_string_cpy(layoutData->arena_frame, textBoxData.strUser);
        string_strip(&str);

        Player *player = player_find(layoutData->player_map, layoutData->name_chunk_state, str);
        if (player) {
            log_error("Player alread registered");
        }
        else {
            player_create(layoutData->name_chunk_state, layoutData->player_state, layoutData->player_map, str);
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
    LayoutData *layoutData = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = element_id;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // TODO: open a menu of stuff that can be done
        // for now use it just for deleting
        String tmp = {
            .str = arena_push(layoutData->arena_frame, element_id.stringId.length),
            .len = element_id.stringId.length
        };
        memcpy(tmp.str, element_id.stringId.chars, element_id.stringId.length);
        tmp.len = element_id.stringId.length;

        player_delete(
            layoutData->arena_permanent,
            layoutData->name_state, layoutData->name_chunk_state,
            layoutData->player_state, layoutData->player_map, layoutData->tournament_map, tmp
        );
    }
}

#endif
