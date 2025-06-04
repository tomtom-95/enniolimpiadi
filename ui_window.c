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
LayoutPlayerTextBoxWithFloatingLabel(LayoutData *layoutData) {
    TextBoxData *textBoxData = &(layoutData->text_box_data);
    CLAY({
        .id = CLAY_ID("FloatingLabel"),
        .backgroundColor = gray,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { .left = 5, .right = 5, .top = 0, .bottom = 0 }
        },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .attachPoints = {
                .element = CLAY_ATTACH_POINT_LEFT_TOP,
                .parent = CLAY_ATTACH_POINT_LEFT_TOP
            },
            .offset = {10, textBoxData->floatingLabelYOffset}
        }
    }) {
        Clay_OnHover(HandleFloatingLabelInteraction, (intptr_t)layoutData);
        CLAY_TEXT(CLAY_STRING("Enter new player name"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16, .fontSize = 15, .textColor = white
        }));
    }
    Clay_String str_player_name = {
        .isStaticallyAllocated = true,
        .length = (s32)textBoxData->str_final.len,
        .chars = (const char *)textBoxData->str_final.str
    };
    CLAY_TEXT(str_player_name, CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 15,
        .textColor = { 255, 255, 255, 255 }
    }));
}

void
LayoutAddPlayerWindow(LayoutData *data) {
    TextBoxData *textBoxData = &(data->text_box_data);
    Clay_Color color_border;
    u16 width_border;
    bool isTextBoxClicked = (
        data->last_element_clicked.id == Clay_GetElementId(CLAY_STRING("PlayerTextBox")).id ||
        data->last_element_clicked.id == Clay_GetElementId(CLAY_STRING("FloatingLabel")).id
    );
    bool isTextBoxHoverer = (
        Clay_PointerOver(Clay_GetElementId(CLAY_STRING("PlayerTextBox"))) ||
        Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FloatingLabel")))
    );

    if (isTextBoxClicked) {
        color_border = gray_light;
        width_border = 3;
    }
    else {
        width_border = 1;
        if (isTextBoxHoverer) {
            color_border = blue_ligth;
        }
        else {
            color_border = blue;
        }
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
        CLAY({
            .id = CLAY_ID("FloatingLabel"),
            .backgroundColor = gray,
            .cornerRadius = CLAY_CORNER_RADIUS(6),
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { .left = 5, .right = 5, .top = 0, .bottom = 0 }
            },
            .floating = {
                .attachTo = CLAY_ATTACH_TO_PARENT,
                .attachPoints = {
                    .element = CLAY_ATTACH_POINT_LEFT_TOP,
                    .parent = CLAY_ATTACH_POINT_LEFT_TOP
                },
                .offset = {10, textBoxData->floatingLabelYOffset}
            }
        }) {
            Clay_OnHover(HandleFloatingLabelInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("Enter new player name"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16, .fontSize = 15, .textColor = white
            }));
        }
        Clay_String str_player_name = {
            .isStaticallyAllocated = true,
            .length = (s32)textBoxData->str_final.len,
            .chars = (const char *)textBoxData->str_final.str
        };
        CLAY_TEXT(str_player_name, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 15,
            .textColor = { 255, 255, 255, 255 }
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

void
LayoutAddTournamentWindow(LayoutData *data) {
    CLAY({
        .id = CLAY_ID("ProfilePicture"),
        .layout = {
            .sizing = layoutExpand
        },
        .image = {
            .imageData = &(data->profilePicture),
            .sourceDimensions = {640, 640}
        }
    }) {}
}

#endif
