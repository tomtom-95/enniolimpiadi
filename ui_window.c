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
UpdateCursorPos(TextBoxData *textBoxData) {
    char strCursor[textBoxData->strLenMax];
    memcpy(strCursor, textBoxData->strUser.str, textBoxData->cursorIdx);
    strCursor[textBoxData->cursorIdx] = '\0';
    textBoxData->cursorPos = MeasureTextEx(
        textBoxData->font, strCursor, textBoxData->fontSize, 0
    );
}

void
UpdateHighlightPos(TextBoxData *textBoxData) {
    char strHighlight[textBoxData->strLenMax];
    memcpy(strHighlight, textBoxData->strUser.str, textBoxData->highlightIdx);
    strHighlight[textBoxData->highlightIdx] = '\0';
    textBoxData->highlightPos = MeasureTextEx(
        textBoxData->font, strHighlight, textBoxData->fontSize, 0
    );
}

void
TextBoxWrite(TextBoxData *textBoxData, int key) {
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;
    memmove(p_start + 1, p_end, strUser->len - end);

    strUser->str[start] = (u8)key;
    strUser->len = strUser->len + start - end + 1;

    *cursorIdx = start + 1;
    *highlightIdx = textBoxData->cursorIdx;
}

void
TextBoxDelete(TextBoxData *textBoxData) {
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

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
TextBoxCopy(LayoutData *layoutData, TextBoxData *textBoxData) {
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    u16 start = Min(*cursorIdx, *highlightIdx);
    u16 end = Max(*cursorIdx, *highlightIdx);

    u8 *selectedText = arena_push(layoutData->arena_frame, end - start + 1);
    memcpy(selectedText, textBoxData->strUser.str + start, end - start);
    selectedText[end - start] = '\0';

    return selectedText;
}

void
TextBoxPaste(TextBoxData *textBoxData, u8 *clipboardText) {
    s16 textLen = (s16)(cstring_len(clipboardText) - 1);
    String *strUser = &textBoxData->strUser;

    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);

    s16 start = (s16)Min(*cursorIdx, *highlightIdx);
    s16 end = (s16)Max(*cursorIdx, *highlightIdx);

    u8 *p_start = strUser->str + start;
    u8 *p_end = strUser->str + end;

    s16 delta = textLen - end + start;
    memmove(p_end + delta, p_end, textBoxData->strUser.len - end);
    memcpy(p_start, clipboardText, textLen);

    textBoxData->strUser.len += delta;
    textBoxData->cursorIdx = end + delta;
    textBoxData->highlightIdx = end + delta;
}


void
TextBoxMoveCursorLeft(TextBoxData *textBoxData) {
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);
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
TextBoxMoveCursorRight(TextBoxData *textBoxData) {
    u16 *cursorIdx = &(textBoxData->cursorIdx);
    u16 *highlightIdx = &(textBoxData->highlightIdx);
    if (*cursorIdx == *highlightIdx && *cursorIdx < textBoxData->strUser.len) {
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
HelperTextBox(LayoutData *layoutData, TextBoxData *textBoxData, Clay_String textBoxIdStr) {
    Clay_ElementId textBoxId = Clay_GetElementId(textBoxIdStr);

    assert(textBoxData->cursorIdx <= textBoxData->strUser.len);

    if (layoutData->last_element_clicked.id == textBoxId.id) {
        textBoxData->colorBorder = gray_light;
        textBoxData->widthBorder = 3;
        ++textBoxData->frameCounter;
        textBoxData->isKeyPressedThisFrame = false;

        // Handle user writing
        int key = GetCharPressed();
        while (key > 0) {
            textBoxData->frameCounter = 0;
            if ((key >= 32) && (key <= 125) && (textBoxData->strUser.len < textBoxData->strLenMax)) {
                textBoxData->isKeyPressedThisFrame = true;
                TextBoxWrite(textBoxData, key);
                key = GetCharPressed();
            }
        }

        bool isCtrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        bool isCmdDown  = IsKeyDown(KEY_LEFT_SUPER)   || IsKeyDown(KEY_RIGHT_SUPER);

        bool isCopyCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_C);
        bool isPasteCombo = (isCtrlDown || isCmdDown) && IsKeyPressed(KEY_V);

        if (isCopyCombo) {
            textBoxData->isKeyPressedThisFrame = true;
            u8* selectedText = TextBoxCopy(layoutData, textBoxData);
            SetClipboardText(selectedText);
        }

        if (isPasteCombo) {
            textBoxData->isKeyPressedThisFrame = true;
            const char *clipboardText = GetClipboardText();
            if (clipboardText) {
                // TODO: check implementation
                TextBoxPaste(textBoxData, clipboardText);
            }
        }

        // Determine if any control key is being held
        if (IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            int currentKey = 0;
            if (IsKeyDown(KEY_BACKSPACE))  currentKey = KEY_BACKSPACE;
            else if (IsKeyDown(KEY_LEFT))  currentKey = KEY_LEFT;
            else if (IsKeyDown(KEY_RIGHT)) currentKey = KEY_RIGHT;

            if (!textBoxData->isKeyHeld || textBoxData->keyHeld != currentKey) {
                // New key press or key changed
                textBoxData->keyHeld = currentKey;
                textBoxData->keyRepeatTimer = textBoxData->keyRepeatDelay;
                textBoxData->isKeyHeld = true;

                // Immediate action
                if (currentKey == KEY_BACKSPACE) {
                    TextBoxDelete(textBoxData);
                }
                else if (currentKey == KEY_LEFT) {
                    TextBoxMoveCursorLeft(textBoxData);
                }
                else if (currentKey == KEY_RIGHT) {
                    TextBoxMoveCursorRight(textBoxData);
                }
            }
            else {
                // Handle repeat
                textBoxData->keyRepeatTimer -= GetFrameTime();
                if (textBoxData->keyRepeatTimer <= 0.0f) {
                    if (textBoxData->keyHeld == KEY_BACKSPACE) {
                        TextBoxDelete(textBoxData);
                    }
                    else if (textBoxData->keyHeld == KEY_LEFT) {
                        TextBoxMoveCursorLeft(textBoxData);
                    }
                    else if (textBoxData->keyHeld == KEY_RIGHT) {
                        TextBoxMoveCursorRight(textBoxData);
                    }
                    textBoxData->keyRepeatTimer = textBoxData->keyRepeatRate;
                }
            }

            textBoxData->isKeyPressedThisFrame = true;
            textBoxData->frameCounter = 0;
        }
        else {
            textBoxData->isKeyHeld = false;
            textBoxData->keyHeld = 0;
            textBoxData->keyRepeatTimer = 0.0f;
        }

        UpdateCursorPos(textBoxData);
        UpdateHighlightPos(textBoxData);

        string_cpy(&textBoxData->strOutput, &textBoxData->strUser);
    }
    else {
        textBoxData->colorBorder = blue;
        textBoxData->widthBorder = 1;
        textBoxData->frameCounter = textBoxData->cursorFrequency;

        if (textBoxData->strUser.len == 0) {
            string_cpy(&textBoxData->strOutput, &textBoxData->strLabel);
        }
    }
}

void
LayoutDropdownMenuItem(Clay_String text) {
    CLAY({
        .layout = { 
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(8)
        },
        .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void
LayoutAddPlayerWindow(LayoutData *layoutData) {
    TextBoxData *textBoxData = &layoutData->addPlayerTextBoxData;
    Clay_Color colorBorder = textBoxData->colorBorder;
    u16 widthBorder = textBoxData->widthBorder;

    CustomLayoutElement *customLayoutElement = (
        arena_push(layoutData->arena_frame, sizeof(CustomLayoutElement))
    );
    customLayoutElement->type = CUSTOM_LAYOUT_TEXTBOX;
    customLayoutElement->customData.textBoxData = &layoutData->addPlayerTextBoxData;
    customLayoutElement->textBoxType = ADD_PLAYER_TEXTBOX;

    CLAY({
        .id = CLAY_SID(addPlayerTextBoxStr),
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
        Clay_OnHover(HandleAddPlayerTextBoxInteraction, (intptr_t)layoutData);
        CLAY({
            .id = CLAY_ID("AddPlayerTextWrapper"),
            .layout = { .sizing = layoutExpand },
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            CLAY({
                .id = CLAY_ID("AddPlayerTextContainer"),
                .layout = { .sizing = layoutExpand },
                .custom = { .customData = customLayoutElement }
            }) {
                CLAY_TEXT(
                    Clay_String_from_String(textBoxData->strOutput), CLAY_TEXT_CONFIG({
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
    CLAY({
        .id = CLAY_ID("AddPlayerFeedback"),
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
        Clay_String feedbackString = Clay_String_from_String(layoutData->feedbackOnAddPlayerButton);
        CLAY_TEXT(feedbackString, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
}

void
LayoutAddTournamentWindow(LayoutData *layoutData) {
    TextBoxData *textBoxData = &(layoutData->addTournamentTextBoxData);
    Clay_Color colorBorder = textBoxData->colorBorder;
    u16 widthBorder = textBoxData->widthBorder;

    CustomLayoutElement *customLayoutElement = (
        arena_push(layoutData->arena_frame, sizeof(CustomLayoutElement))
    );
    customLayoutElement->type = CUSTOM_LAYOUT_TEXTBOX;
    customLayoutElement->customData.textBoxData = &layoutData->addTournamentTextBoxData;
    customLayoutElement->textBoxType = ADD_TOURNAMENT_TEXTBOX;

    CLAY({
        .id = CLAY_SID(addTournamentTextBoxStr),
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
        Clay_OnHover(HandleAddTournamentTextBoxInteraction, (intptr_t)layoutData);
        CLAY({
            .id = CLAY_ID("AddTournamentTextWrapper"),
            .layout = { .sizing = layoutExpand },
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            CLAY({
                .id = CLAY_ID("AddTournamentTextContainer"),
                .layout = { .sizing = layoutExpand },
                .custom = { .customData = customLayoutElement }
            }) {
                CLAY_TEXT(
                    Clay_String_from_String(textBoxData->strOutput), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = white
                }));
            }
        }
    }
    CLAY({
        .id = CLAY_ID("AddTournamentButton"),
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
        Clay_OnHover(HandleAddTournamentButtonInteraction, (intptr_t)layoutData);
        CLAY_TEXT(CLAY_STRING("Add Tournament"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
    CLAY({
        .id = CLAY_ID("AddTournamentFeedback"),
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
        Clay_String feedbackString = Clay_String_from_String(layoutData->feedbackOnAddTournamentButton);
        CLAY_TEXT(feedbackString, CLAY_TEXT_CONFIG({
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
            // TODO: sloppy
            String *player_string = arena_push(data->arena_frame, sizeof(String));
            String player_string_tmp = push_string_from_name(data->arena_frame, *((*player)->player_name));
            player_string->len = player_string_tmp.len;
            player_string->str = player_string_tmp.str;

            Clay_String clay_player_string = {
                .isStaticallyAllocated = false,
                .length = (s32)player_string->len,
                .chars = (const char *)(player_string->str),
            };
            CLAY({
                .id = CLAY_SID(clay_player_string),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIT(200), .height = 0 },
                    .padding = CLAY_PADDING_ALL(12)
                },
                .backgroundColor = Clay_Hovered() ? gray_lighter : gray_light,
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                Clay_OnHover(HandlePlayerSelection, (intptr_t)player_string);
                CLAY({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIT(150) },
                    },
                }) {
                    CLAY_TEXT(clay_player_string, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 24,
                        .textColor = white
                    }));
                    if (data->last_element_clicked.id == CLAY_SID(clay_player_string).id) {
                        CLAY({
                            .id = CLAY_ID("PlayerActionMenu"),
                            .floating = {
                                .attachTo = CLAY_ATTACH_TO_PARENT,
                                .attachPoints = {
                                    .parent = CLAY_ATTACH_POINT_RIGHT_CENTER
                                },
                            }
                        }) {
                            CLAY({
                                .layout = {
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                    .sizing = {
                                        .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
                                    },
                                },
                                .backgroundColor = blue,
                                .cornerRadius = CLAY_CORNER_RADIUS(8)
                            }) {
                                CLAY({
                                    .id = CLAY_ID("PlayerEnroll"),
                                    .layout = { 
                                        .sizing = layoutExpand,
                                        .padding = CLAY_PADDING_ALL(8)
                                    },
                                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                                }) {
                                    CLAY_TEXT(CLAY_STRING("Enroll"), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_BODY_16,
                                        .fontSize = 16,
                                        .textColor = { 255, 255, 255, 255 }
                                    }));
                                    Clay_OnHover(HandleEnrollPlayerInteraction, (intptr_t)data);
                                }
                                CLAY({
                                    .id = CLAY_ID("PlayerDelete"),
                                    .layout = { 
                                        .sizing = layoutExpand,
                                        .padding = CLAY_PADDING_ALL(8)
                                    },
                                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                                }) {
                                    Clay_OnHover(HandleDeletePlayerInteraction, (intptr_t)data);
                                    CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_BODY_16,
                                        .fontSize = 16,
                                        .textColor = { 255, 255, 255, 255 }
                                    }));
                                }
                                // LayoutDropdownMenuItem(CLAY_STRING("Enroll"));
                                // LayoutDropdownMenuItem(CLAY_STRING("Delete"));
                            }
                        }
                    }
                }
            }
            player = &((*player)->next);
        }
    }
}

void
LayoutTournamentsWindow(LayoutData *data) {
    StringList string_list = list_tournaments(data->arena_frame, data->tournament_map);
    StringNode *node = string_list.head;
    while (node) {
        Clay_String tournament_string = {
            .isStaticallyAllocated = false,
            .length = (s32)(node->str).len,
            .chars = (const char *)((node->str).str),
        };
        CLAY({
            .id = CLAY_SID(tournament_string),
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(40) },
                .padding = CLAY_PADDING_ALL(12)
            },
            .backgroundColor = Clay_Hovered() ? gray_lighter : gray_light,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleTournamentSelection, (intptr_t)data);
            CLAY_TEXT(tournament_string, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 24,
                .textColor = white
            }));
        }
        node = node->next;
    }
}

#endif
