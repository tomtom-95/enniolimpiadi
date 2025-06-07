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
        if (textBoxData->backspace_key_state == BACKSPACE_NOT_PRESSED) {
            if (str->len != 0) {
                --(str->len);
            }
            textBoxData->backspace_key_state = BACKSPACE_FIRST; 
        }
        else if (textBoxData->backspace_key_state == BACKSPACE_FIRST) {
            textBoxData->frame_timer += GetFrameTime();
            if (textBoxData->frame_timer > 0.35f) {
                textBoxData->frame_timer = 0;
                textBoxData->backspace_key_state = BACKSPACE_SECOND;
            } 
        }
        else {
            textBoxData->frame_timer += GetFrameTime();
            if (textBoxData->frame_timer > 0.03f) {
                textBoxData->frame_timer = 0;
                if (str->len != 0) {
                    --(str->len);
                }
            } 
        }
    }
    else {
        textBoxData->backspace_key_state = BACKSPACE_NOT_PRESSED;
    }
    int key = GetCharPressed();
    while (key > 0) {
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (str->len < textBoxData->max_str_len)) {
            str->str[(str->len)++] = (u8)key;
        }
        key = GetCharPressed();  // Check next character in the queue
    }

    String *str_final = &(textBoxData->str_final);
    string_cpy(str_final, &(textBoxData->str));
    if (((textBoxData->frame_counter/40)%2) == 1) {
        str_final->str[(str_final->len)++] = (u8)'_';
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

    if (isTextBoxClicked) {
        width_border = 3;
        color_border = gray_light;
        ++data->text_box_data.frame_counter;
        HandleUserWriting(data);
    }
    else {
        width_border = 1;
        color_border = isTextBoxHovered ? blue_ligth : blue;
        data->text_box_data.frame_counter = 0;
    }

    CLAY({
        .id = CLAY_ID("PlayerTextBox"),
        .backgroundColor = gray,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .border = {
            .color = color_border,
            .width = {width_border, width_border, width_border, width_border}
        },
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIT(40)
            },
            .padding = CLAY_PADDING_ALL(12)
        }
    }) {
        Clay_OnHover(HandlePlayerTextBoxInteraction, (intptr_t)data);
        Clay_String str_player_name = {
            .isStaticallyAllocated = true,
            .length = (s32)textBoxData->str_final.len,
            .chars = (const char *)textBoxData->str_final.str
        };
        CLAY_TEXT(str_player_name, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = (u16)textBoxData->fontSize,
            .textColor = white
        }));
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
