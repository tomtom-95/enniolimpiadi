#ifndef UI_WINDOW_C
#define UI_WINDOW_C

#include <stdlib.h>

#include "arena.c"
#include "string.c"
#include "registration.c"

#include "clay.h"
#include "raylib/raylib.h"
#include "ui_interaction.c"

void
HandleUserWriting(LayoutData *layoutData) {
    // TODO: If I am in the CLAY_POINTER_DATA_PRESSED state using backspace must delete all the
    //       characters highlighted and writing a character must first delete all the character highlighted
    TextBoxData *textBoxData = &(layoutData->text_box_data);

    String *str = &(textBoxData->str);
    u32 *highlight_start = &textBoxData->highlight_start;
    u32 *highlight_end = &textBoxData->highlight_end;
    if (textBoxData->textBoxDataState == HIGHLIGHT_STATE) {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
            textBoxData->textBoxDataState = CLICK_STATE;
            textBoxData->backspace_key_state = BACKSPACE_FIRST;
            memmove(str->str + *highlight_start, str->str + *highlight_end, str->len - *highlight_end);
            str->len -= textBoxData->highlight_end - textBoxData->highlight_start;
            textBoxData->cursor_position = *highlight_start;
            *highlight_start = 0;
            *highlight_end = 0;
        }

        int key = GetCharPressed();
        if (key > 0) {
            // TODO: delete all the stuff that is highlighted, go to CLICK_STATE and write key
            // Check next character in the queue
        }

        key = GetKeyPressed();
        if (key > 0) {
            // TODO: go to the character after cursor, go to CLICK_STATE
        }
    }
    else {
        bool someKeyPressed = false;
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
            someKeyPressed = true;
            u8 *p_cursor = textBoxData->str.str + textBoxData->cursor_position;
            if (textBoxData->backspace_key_state == BACKSPACE_NOT_PRESSED) {
                if (str->len != 0) {
                    memmove(p_cursor - 1, p_cursor, textBoxData->str.len - textBoxData->cursor_position);
                    --(str->len);
                    --(textBoxData->cursor_position);
                    textBoxData->frame_counter = 0;
                }
                textBoxData->backspace_key_state = BACKSPACE_FIRST; 
            }
            else if (textBoxData->backspace_key_state == BACKSPACE_FIRST) {
                textBoxData->frame_timer += GetFrameTime();
                if (textBoxData->frame_timer > 0.5f) {
                    textBoxData->frame_timer = 0;
                    textBoxData->backspace_key_state = BACKSPACE_SECOND;
                } 
            }
            else {
                textBoxData->frame_timer += GetFrameTime();
                if (textBoxData->frame_timer > 0.03f) {
                    textBoxData->frame_timer = 0;
                    if (str->len != 0) {
                        memmove(p_cursor - 1, p_cursor, textBoxData->str.len - textBoxData->cursor_position);
                        --(str->len);
                        --(textBoxData->cursor_position);
                        textBoxData->frame_counter = 0;
                    }
                } 
            }
        }
        else {
            textBoxData->backspace_key_state = BACKSPACE_NOT_PRESSED;
        }

        int key = GetCharPressed();
        while (key > 0) {
            someKeyPressed = true;
            u8 *p_cursor = textBoxData->str.str + textBoxData->cursor_position;
            memmove(p_cursor + 1, p_cursor, textBoxData->str.len - textBoxData->cursor_position);

            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (str->len < textBoxData->max_str_len)) {
                str->str[(textBoxData->cursor_position)++] = (u8)key;
                ++textBoxData->str.len;
                textBoxData->frame_counter = 0;
            }

            // Check next character in the queue
            key = GetCharPressed();
        }

        key = GetKeyPressed();
        while (key > 0) {
            someKeyPressed = true;
            if (key == KEY_RIGHT) {
                if (textBoxData->cursor_position < textBoxData->str.len) {
                    ++textBoxData->cursor_position;
                }
                textBoxData->frame_counter = 0;
            }
            else if (key == KEY_LEFT) {
                if (textBoxData->cursor_position != 0) {
                    --textBoxData->cursor_position;
                }
                textBoxData->frame_counter = 0;
            }
            key = GetKeyPressed();
        }

        if (someKeyPressed) {
            Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(CLAY_ID("PlayerTextBox"));
            if (scrollData.found && scrollData.scrollPosition) {
                char strCursor[layoutData->text_box_data.cursor_position + 1];
                memcpy(strCursor, layoutData->text_box_data.str.str, layoutData->text_box_data.cursor_position);
                strCursor[layoutData->text_box_data.cursor_position] = '\0';

                float cursorX = MeasureTextEx(
                    fonts[layoutData->text_box_data.font_id], strCursor,
                    (float)layoutData->text_box_data.fontSize, 0
                ).x;

                float scrollAreaWidth = scrollData.scrollContainerDimensions.width;
                float currentScrollX = -scrollData.scrollPosition->x;  // Clay uses negative scroll
                float padding = 20.0f;

                // Check if cursor is out of view to the right
                if (cursorX > currentScrollX + scrollAreaWidth - padding) {
                    float newScrollX = cursorX - scrollAreaWidth;
                    *scrollData.scrollPosition = (Clay_Vector2){ .x = -newScrollX, .y = 0 };
                }

                // Check if cursor is out of view to the left
                else if (cursorX < currentScrollX - padding) {
                    float newScrollX = cursorX + padding;
                    if (newScrollX < 0) {
                        newScrollX = 0;
                    }
                    *scrollData.scrollPosition = (Clay_Vector2){ .x = -newScrollX, .y = 0 };
                }
            }
        }
    }
}

void
HelperTextBoxV2(LayoutData *layoutData) {
    TextBoxDataV2 *textBoxDataV2 = &(layoutData->textBoxDataV2);
    Clay_ElementId textBoxV2Id = Clay_GetElementId(CLAY_STRING("TextBoxV2"));
    Clay_ElementData textBoxV2ElementData = Clay_GetElementData(textBoxV2Id);

    if (Clay_PointerOver(textBoxV2Id)) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }

    if (layoutData->last_element_clicked.id == textBoxV2Id.id) {
        textBoxDataV2->colorBorder = gray_light;
        textBoxDataV2->widthBorder = 3;
        ++textBoxDataV2->frameCounter;

        // Handle user writing
        int key = GetCharPressed();
        while (key > 0) {
            textBoxDataV2->frameCounter = 0;

            u8 *p_cursor = textBoxDataV2->strUser.str + textBoxDataV2->cursorIdx;
            memmove(p_cursor + 1, p_cursor, textBoxDataV2->strUser.len - textBoxDataV2->cursorIdx);

            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (textBoxDataV2->strUser.len < textBoxDataV2->strLenMax)) {
                textBoxDataV2->strUser.str[(textBoxDataV2->cursorIdx)++] = (u8)key;
                ++textBoxDataV2->strUser.len;
            }

            // Check next character in the queue
            key = GetCharPressed();
        }

        // Handle continuous backspace
        if (IsKeyDown(KEY_BACKSPACE)) {
            textBoxDataV2->frameCounter = 0;

            float delta = GetFrameTime();
            textBoxDataV2->backspaceTimer -= delta;

            if (!textBoxDataV2->backspaceHeld) {
                if (textBoxDataV2->cursorIdx > 0) {
                    textBoxDataV2->cursorIdx--;
                    u8 *p_cursor = textBoxDataV2->strUser.str + textBoxDataV2->cursorIdx;
                    memmove(p_cursor, p_cursor + 1, textBoxDataV2->strUser.len - textBoxDataV2->cursorIdx - 1);
                    textBoxDataV2->strUser.len--;
                    textBoxDataV2->strUser.str[textBoxDataV2->strUser.len] = '\0';
                }
                textBoxDataV2->backspaceTimer = textBoxDataV2->backspaceRepeatDelay;
                textBoxDataV2->backspaceHeld = true;
            }
            else if (textBoxDataV2->backspaceTimer <= 0.0f) {
                if (textBoxDataV2->cursorIdx > 0) {
                    textBoxDataV2->cursorIdx--;
                    u8 *p_cursor = textBoxDataV2->strUser.str + textBoxDataV2->cursorIdx;
                    memmove(p_cursor, p_cursor + 1, textBoxDataV2->strUser.len - textBoxDataV2->cursorIdx - 1);
                    textBoxDataV2->strUser.len--;
                    textBoxDataV2->strUser.str[textBoxDataV2->strUser.len] = '\0';
                }
                textBoxDataV2->backspaceTimer = textBoxDataV2->backspaceRepeatRate;
            }
        }
        else {
            textBoxDataV2->backspaceHeld = false;
            textBoxDataV2->backspaceTimer = 0.0f;
        }

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
    HelperTextBoxV2(layoutData);

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
        },
        .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() },
    }) {
        Clay_OnHover(HandleTextBoxV2Interaction, (intptr_t)layoutData);
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

void
LayoutAddPlayerWindow(LayoutData *data) {
    TextBoxData *textBoxData = &(data->text_box_data);
    Clay_Color color_border;
    u16 width_border;

    Clay_ElementId playerTextBoxId = Clay_GetElementId(CLAY_STRING("PlayerTextBox"));
    Clay_ElementId floatingLabelId = Clay_GetElementId(CLAY_STRING("FloatingLabel"));

    bool isTextBoxHovered = (
        Clay_PointerOver(playerTextBoxId) ||
        Clay_PointerOver(floatingLabelId)
    );

    bool isTextBoxClicked = (
        data->last_element_clicked.id == playerTextBoxId.id ||
        data->last_element_clicked.id == floatingLabelId.id
    );

    if (isTextBoxHovered) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }

    if (isTextBoxClicked) {
        width_border = 3;
        color_border = gray_light;
        ++data->text_box_data.frame_counter;
        HandleUserWriting(data);
    }
    else {
        width_border = 1;
        color_border = isTextBoxHovered ? blue_ligth : blue;
        data->text_box_data.frame_counter = 40;
    }

    CustomLayoutElement *customLayoutElement = arena_push(data->arena_frame, sizeof(CustomLayoutElement));
    *customLayoutElement = (CustomLayoutElement) {
        .type = CUSTOM_LAYOUT_TEXTBOX
    };

    CLAY({
        .id = CLAY_ID("PlayerTextBox"),
        .backgroundColor = gray,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .border = {
            .color = color_border,
            .width = {width_border, width_border, width_border, width_border}
        },
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(40) },
            .padding = CLAY_PADDING_ALL(12)
        },
        .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
    }) {
        CLAY({
            .id = CLAY_ID("TextContainer"),
            .layout = { .sizing = layoutExpand },
            .custom = { .customData = customLayoutElement }
        }) {
            Clay_OnHover(HandlePlayerTextBoxInteraction, (intptr_t)data);
            char tmpstr[256];
            memcpy(tmpstr, data->text_box_data.str.str, data->text_box_data.str.len);
            tmpstr[data->text_box_data.str.len] = '\0';
            Clay_String tmp_string = {
                .isStaticallyAllocated = false,
                .length = (s32)data->text_box_data.str.len,
                .chars = (const char *)(data->text_box_data.str.str),
            };
            CLAY_TEXT(tmp_string, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = (float)data->text_box_data.fontSize,
                .textColor = white
            }));
        }
    }
    CLAY({
        .id = CLAY_ID("AddPlayerButton"),
        .backgroundColor = Clay_Hovered() ? violet_light : violet,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(56)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER
            }
        }
    }) {
        Clay_OnHover(HandleAddPlayerButtonInteraction, (intptr_t)data);
        CLAY_TEXT(CLAY_STRING("Add Player"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
}

#endif
