#ifndef UI_WINDOW_C
#define UI_WINDOW_C

#include <stdlib.h>

#include "utils.c"
#include "arena.c"
#include "string.c"
#include "registration.c"

#include "clay.h"
#include "raylib/raylib.h"
#include "ui_interaction.c"

void
UpdateCursorPos(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);

    char strCursor[textBoxDataV2->strLenMax];
    memcpy(strCursor, textBoxDataV2->strUser.str, textBoxDataV2->cursorIdx);
    strCursor[textBoxDataV2->cursorIdx] = '\0';
    textBoxDataV2->cursorPos = MeasureTextEx(
        textBoxDataV2->font, strCursor, textBoxDataV2->fontSize, 0
    );
}

void
UpdateHighlightPos(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);

    char strHighlight[textBoxDataV2->strLenMax];
    memcpy(strHighlight, textBoxDataV2->strUser.str, textBoxDataV2->highlightIdx);
    strHighlight[textBoxDataV2->highlightIdx] = '\0';
    textBoxDataV2->highlightPos = MeasureTextEx(
        textBoxDataV2->font, strHighlight, textBoxDataV2->fontSize, 0
    );
}

void
TextBoxV2Write(LayoutData *layoutData, int key) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    String *strUser = &textBoxDataV2->strUser;

    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;
    memmove(p_start + 1, p_end, strUser->len - end);

    strUser->str[start] = (u8)key;
    strUser->len = strUser->len + start - end + 1;

    *cursorIdx = start + 1;
    *highlightIdx = textBoxDataV2->cursorIdx;
}

void
TextBoxV2Delete(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    String *strUser = &textBoxDataV2->strUser;

    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    if (start == end && start > 0) {
        --start;
    }

    if (start != end) {
        u8 *p_start = strUser->str + start;
        u8 *p_end = strUser->str + end;

        memmove(p_start, p_end, strUser->len - end);
        strUser->len -= (end - start);
    }

    *cursorIdx = start;
    *highlightIdx = start;
}

u8 *
TextBoxV2Copy(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);

    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *selectedText = arena_push(layoutData->arena_frame, end - start + 1);
    memcpy(selectedText, textBoxDataV2->strUser.str + start, end - start);
    selectedText[end - start] = '\0';

    return selectedText;
}

void
TextBoxV2Paste(LayoutData *layoutData, u8 *clipboardText) {
    s16 textLen = (s16)(cstring_len(clipboardText) - 1);

    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    String *strUser = &textBoxDataV2->strUser;

    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);

    s16 start = (s16)Min(*cursorIdx, *highlightIdx);
    s16 end = (s16)Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;

    s16 delta = textLen - end + start;
    memmove(p_end + delta, p_end, textBoxDataV2->strUser.len - end);
    memcpy(p_start, clipboardText, textLen);

    textBoxDataV2->strUser.len += delta;
    textBoxDataV2->cursorIdx = end + delta;
    textBoxDataV2->highlightIdx = end + delta;
}


void
TextBoxV2MoveCursorLeft(TextBoxDataV2 *textBoxDataV2) {
    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);
    if (*cursorIdx == *highlightIdx && *cursorIdx > 0) {
        --(*cursorIdx);
        --(*highlightIdx);
    }
    else {
        u16 idx = Min(*cursorIdx, *highlightIdx);
        *cursorIdx = idx;
        *highlightIdx = idx;
    }
}

void
TextBoxV2MoveCursorRight(TextBoxDataV2 *textBoxDataV2) {
    u16 *cursorIdx = &(textBoxDataV2->cursorIdx);
    u16 *highlightIdx = &(textBoxDataV2->highlightIdx);
    if (*cursorIdx == *highlightIdx && *cursorIdx < textBoxDataV2->strUser.len) {
        ++(*cursorIdx);
        ++(*highlightIdx);
    }
    else {
        u16 idx = Max(*cursorIdx, *highlightIdx);
        *cursorIdx = idx;
        *highlightIdx = idx;
    }
}

void
HelperTextBoxV2(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    Clay_ElementId textBoxV2Id = Clay_GetElementId(CLAY_STRING("TextBoxV2"));

    assert(textBoxDataV2->cursorIdx <= textBoxDataV2->strUser.len);

    if (layoutData->last_element_clicked.id == textBoxV2Id.id) {
        textBoxDataV2->colorBorder = gray_light;
        textBoxDataV2->widthBorder = 3;
        ++textBoxDataV2->frameCounter;
        textBoxDataV2->isKeyPressedThisFrame = false;

        // Handle user writing
        int key = GetCharPressed();
        while (key > 0) {
            textBoxDataV2->frameCounter = 0;
            if ((key >= 32) && (key <= 125) && (textBoxDataV2->strUser.len < textBoxDataV2->strLenMax)) {
                textBoxDataV2->isKeyPressedThisFrame = true;
                TextBoxV2Write(layoutData, key);
                key = GetCharPressed();
            }
        }

        bool isCtrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        bool isCmdDown  = IsKeyDown(KEY_LEFT_SUPER)   || IsKeyDown(KEY_RIGHT_SUPER);

        bool isCopyCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_C);
        bool isPasteCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_V);

        if (isCopyCombo) {
            textBoxDataV2->isKeyPressedThisFrame = true;
            u8* selectedText = TextBoxV2Copy(layoutData);
            SetClipboardText(selectedText);
        }

        if (isPasteCombo) {
            textBoxDataV2->isKeyPressedThisFrame = true;
            const char *clipboardText = GetClipboardText();
            if (clipboardText) {
                // TODO: check implementation
                TextBoxV2Paste(layoutData, clipboardText);
            }
        }


        // Determine if any control key is being held
        if (IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            int currentKey = 0;
            if (IsKeyDown(KEY_BACKSPACE))  currentKey = KEY_BACKSPACE;
            else if (IsKeyDown(KEY_LEFT))  currentKey = KEY_LEFT;
            else if (IsKeyDown(KEY_RIGHT)) currentKey = KEY_RIGHT;

            if (!textBoxDataV2->isKeyHeld || textBoxDataV2->keyHeld != currentKey) {
                // New key press or key changed
                textBoxDataV2->keyHeld = currentKey;
                textBoxDataV2->keyRepeatTimer = textBoxDataV2->keyRepeatDelay;
                textBoxDataV2->isKeyHeld = true;

                // Immediate action
                if (currentKey == KEY_BACKSPACE) {
                    TextBoxV2Delete(layoutData);
                }
                else if (currentKey == KEY_LEFT) {
                    TextBoxV2MoveCursorLeft(textBoxDataV2);
                }
                else if (currentKey == KEY_RIGHT) {
                    TextBoxV2MoveCursorRight(textBoxDataV2);
                }
            }
            else {
                // Handle repeat
                textBoxDataV2->keyRepeatTimer -= GetFrameTime();
                if (textBoxDataV2->keyRepeatTimer <= 0.0f) {
                    if (textBoxDataV2->keyHeld == KEY_BACKSPACE) {
                        TextBoxV2Delete(layoutData);
                    }
                    else if (textBoxDataV2->keyHeld == KEY_LEFT) {
                        TextBoxV2MoveCursorLeft(textBoxDataV2);
                    }
                    else if (textBoxDataV2->keyHeld == KEY_RIGHT) {
                        TextBoxV2MoveCursorRight(textBoxDataV2);
                    }
                    textBoxDataV2->keyRepeatTimer = textBoxDataV2->keyRepeatRate;
                }
            }

            textBoxDataV2->isKeyPressedThisFrame = true;
            textBoxDataV2->frameCounter = 0;
        }
        else {
            textBoxDataV2->isKeyHeld = false;
            textBoxDataV2->keyHeld = 0;
            textBoxDataV2->keyRepeatTimer = 0.0f;
        }

        UpdateCursorPos(layoutData);
        UpdateHighlightPos(layoutData);

        string_cpy(&textBoxDataV2->strOutput, &textBoxDataV2->strUser);
    }
    else {
        textBoxDataV2->colorBorder = blue;
        textBoxDataV2->widthBorder = 1;
        textBoxDataV2->frameCounter = textBoxDataV2->cursorFrequency;

        if (textBoxDataV2->strUser.len == 0) {
            string_cpy(&textBoxDataV2->strOutput, &textBoxDataV2->strLabel);
        }
    }
}

void
LayoutTextBoxV2(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    Clay_Color colorBorder = textBoxDataV2->colorBorder;
    u16 widthBorder = textBoxDataV2->widthBorder;

    CustomLayoutElement *customLayoutElement = (
        arena_push(layoutData->arena_frame, sizeof(CustomLayoutElement))
    );
    *customLayoutElement = (CustomLayoutElement) { .type = CUSTOM_LAYOUT_TEXTBOX_V2 };

    CLAY({
        .id = CLAY_ID("TextBoxV2"),
        .backgroundColor = gray,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .border = {
            .color = colorBorder,
            .width = { widthBorder, widthBorder, widthBorder, widthBorder }
        },
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(40) },
            .padding = CLAY_PADDING_ALL(12)
        }
    }) {
        Clay_OnHover(HandleTextBoxV2Interaction, (intptr_t)layoutData);
        CLAY({
            .id = CLAY_ID("TextWrapperV2"),
            .layout = { .sizing = layoutExpand },
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            CLAY({
                .id = CLAY_ID("TextContainerV2"),
                .layout = { .sizing = layoutExpand },
                .custom = { .customData = customLayoutElement }
            }) {
                CLAY_TEXT(
                    Clay_String_from_String(textBoxDataV2->strOutput), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = white
                }));
            }
        }
    }
    CLAY({
        .id = CLAY_ID("AddPlayerButton"),
        .backgroundColor = Clay_Hovered() ? violet_light : violet,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(46)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER
            }
        }
    }) {
        Clay_OnHover(HandleAddPlayerButtonInteraction, (intptr_t)layoutData);
        CLAY_TEXT(CLAY_STRING("Add Player"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
}

void
LayoutPlayersWindow(LayoutData *data) {
    for (int j = 0; j < data->player_map->bucket_count; j++) {
        Player **player = data->player_map->players + j;
        while (*player) {
            String player_string = name_to_string(data->arena_frame, (*player)->player_name);
            Clay_String clay_player_string = {
                .isStaticallyAllocated = false,
                .length = (s32)player_string.len,
                .chars = (const char *)(player_string.str),
            };
            CLAY_TEXT(clay_player_string, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 24,
                .textColor = white
            }));
            player = &((*player)->next);
        }
    }
}

void
LayoutTournamentsWindow(LayoutData *data) {
    StringList string_list = list_tournaments(
        data->arena_frame,
        data->player_map
    );
    StringNode *node = string_list.head;
    while (node) {
        Clay_String tournament_string = {
            .isStaticallyAllocated = false,
            .length = (s32)(node->str).len,
            .chars = (const char *)((node->str).str),
        };
        node = node->next;
        CLAY_TEXT(tournament_string, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 24,
            .textColor = white
        }));
    }
}

#endif
