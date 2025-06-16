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
        float delta = GetFrameTime();
        textBoxDataV2->trackpadTimer -= delta;

        // Handle highlighting
        if (!textBoxDataV2->trackpadHeld) {
            textBoxDataV2->trackpadTimer = textBoxDataV2->trackpadDelay;
            textBoxDataV2->trackpadHeld = true;
        }
        else if (textBoxDataV2->trackpadTimer <= 0.0f) {
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
    else if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
        textBoxDataV2->trackpadHeld = false;
        textBoxDataV2->trackpadTimer = 0.0f;
    }
}

void
HandlePlayerTextBoxInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t data) {
    LayoutData *layoutData = (LayoutData *)data;
    TextBoxData *textBoxData = &(layoutData->text_box_data);

    // Get character size
    Font font = fonts[textBoxData->font_id];

    // Use 20 or more characters to minimize rounding errors
    const char *testStr = "AAAAAAAAAAAAAAAAAAAA";
    int testLen = (int)strlen(testStr);
    Vector2 size = MeasureTextEx(font, testStr, textBoxData->fontSize, 0);
    float characterWidthPixels = size.x / (float)testLen;

    // Get position of the mouse pointer
    Clay_Vector2 mousePosition = pointerData.position;

    // Get position of the TextContainer bounding box
    Clay_ElementData textContainerData = (
        Clay_GetElementData(Clay_GetElementId(CLAY_STRING("TextContainer")))
    );
    Clay_BoundingBox textContainerBoundingBox = textContainerData.boundingBox;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        layoutData->last_element_clicked = elementId;
        textBoxData->textBoxDataState = CLICK_STATE; 
        textBoxData->frame_counter = 0;
        textBoxData->highlight_end = 0;
        textBoxData->highlight_start = 0; 
        textBoxData->frame_timer_text_highlight = 0;
        textBoxData->highlightWidthPixels = 0;

        float delta = mousePosition.x - textContainerBoundingBox.x;

        u32 mousePositionIndex = (u32)(delta / characterWidthPixels);
        if (mousePositionIndex > textBoxData->str.len) {
            textBoxData->cursor_position = textBoxData->str.len;
        }
        else {
            textBoxData->cursor_position = mousePositionIndex;
        }
    }
    else if (pointerData.state == CLAY_POINTER_DATA_PRESSED) {
        textBoxData->frame_timer_text_highlight += GetFrameTime();
        if (textBoxData->frame_timer_text_highlight > 0.2f) {
            textBoxData->textBoxDataState = HIGHLIGHT_STATE; 

            float delta = mousePosition.x - textContainerBoundingBox.x;
            u32 mousePositionIndex = (u32)(delta / characterWidthPixels);

            u32 highlight_pos;
            if (mousePositionIndex > layoutData->text_box_data.str.len) {
                highlight_pos = textBoxData->str.len;
            }
            else {
                highlight_pos = mousePositionIndex;
            }

            if (highlight_pos > textBoxData->cursor_position) {
                textBoxData->highlight_start = textBoxData->cursor_position;
                textBoxData->highlight_end = highlight_pos;
            } 
            else {
                textBoxData->highlight_start = highlight_pos;
                textBoxData->highlight_end = textBoxData->cursor_position;
            }
        }
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
        player_create(
            layoutData->name_chunk_state, layoutData->player_state, layoutData->player_map,
            layoutData->text_box_data.str
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
