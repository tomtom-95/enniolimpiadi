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
    TextBoxData *textBoxData = &(layoutData->text_box_data);

    String *str = &(textBoxData->str);
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
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
            if (textBoxData->frame_timer > 0.4f) {
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
        if (key == KEY_RIGHT) {
            ++textBoxData->cursor_position;
            textBoxData->frame_counter = 0;
        }
        else if (key == KEY_LEFT) {
            --textBoxData->cursor_position;
            textBoxData->frame_counter = 0;
        }
        key = GetKeyPressed();
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
    }) {
        Clay_OnHover(HandlePlayerTextBoxInteraction, (intptr_t)data);
        CLAY({
            .id = CLAY_ID("TextContainer"),
            .layout = { .sizing = layoutExpand },
            .custom = { .customData = customLayoutElement}
        }) {}
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
