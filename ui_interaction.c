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
        if (textBoxData->str.len == 0) {
            textBoxData->floatingLabelYOffset = (playerTextBoxBoundingBox.height - fontSize) / 2;
        }
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
HandlePlayerTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
    }
}

void
HandleFloatingLabelInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
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
