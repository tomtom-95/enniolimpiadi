#ifndef UI_INTERACTION_C
#define UI_INTERACTION_C

#include "raylib/raylib.h"

#include "arena.c"
#include "registration.c"

#include "clay.h"
#include "ui_utils.h"

void
HandlePlayersButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_PLAYERS;
    }
}

void
HandleTournamentsButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data; 
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->tab = TAB_TOURNAMENTS;
    }
}

void
HandleNewPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &(layoutData->text_box_data);

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
HandleTextBoxV2Interaction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxDataV2 *textBoxDataV2 = &layoutData->textBoxDataV2;

    Clay_ElementId textContainerV2Id = Clay_GetElementId(CLAY_STRING("TextContainerV2"));
    Clay_ElementData textContainerV2Data = Clay_GetElementData(textContainerV2Id);

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
        textBoxDataV2->frameCounter = 0;

        // Calculate cursorIdx on user click based on clicked position
        u16 cursorIdx = 0;
        float subStringLen = 0;
        char subString[textBoxDataV2->strUser.len];
        float characterLen = MeasureTextEx(textBoxDataV2->font, "A", textBoxDataV2->fontSize, 0).x;

        float delta = pointerData.position.x - textContainerV2Data.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && cursorIdx < textBoxDataV2->strUser.len) {
            ++cursorIdx;

            memcpy(subString, textBoxDataV2->strUser.str, cursorIdx);
            subString[cursorIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxDataV2->font, subString, textBoxDataV2->fontSize, 0).x;
        }

        layoutData->textBoxDataV2.cursorIdx = cursorIdx;
        textBoxDataV2->highlightIdx = textBoxDataV2->cursorIdx;
    }
    else if (pointerData.state == CLAY_POINTER_DATA_PRESSED) {
        textBoxDataV2->isTrackpadHeld = true;

        u16 highlightIdx = 0;
        float subStringLen = 0;
        char subString[textBoxDataV2->strUser.len];
        float characterLen = MeasureTextEx(textBoxDataV2->font, "A", textBoxDataV2->fontSize, 0).x;

        float delta = pointerData.position.x - textContainerV2Data.boundingBox.x;
        while (delta - subStringLen > characterLen / 2 && highlightIdx < textBoxDataV2->strUser.len) {
            ++highlightIdx;

            memcpy(subString, textBoxDataV2->strUser.str, highlightIdx);
            subString[highlightIdx] = '\0';

            subStringLen = MeasureTextEx(textBoxDataV2->font, subString, textBoxDataV2->fontSize, 0).x;
        }

        layoutData->textBoxDataV2.highlightIdx = highlightIdx;
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    log_info("%s\n", layoutData->textBoxDataV2.strUser);
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
        player_create(
            layoutData->name_chunk_state, layoutData->player_state, layoutData->player_map,
            layoutData->textBoxDataV2.strUser
        );
    }
}

void
HandleMainWindowInteraction(Clay_ElementId element_id, Clay_PointerData pointer_data, intptr_t data) {
    LayoutData *user_data = (LayoutData *)data;
    if (pointer_data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        user_data->last_element_clicked = element_id;
    }
}

#endif
