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
LayoutAddPlayerWindow(LayoutData *data) {
    CLAY({
        .id = CLAY_ID("UsernameContainer"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0)
            },
            .childGap = 8
        }
    }) {
        CLAY({
            .id = CLAY_ID("UsernameInput"),
            .backgroundColor = blue,
            .cornerRadius = CLAY_CORNER_RADIUS(6),
            .border = {
                .width = {.left = 1, .right = 1, .top = 1, .bottom = 1},
                .color = Clay_Hovered() ? violet_light : violet
            },
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(40)
                },
                .padding = CLAY_PADDING_ALL(12)
            }
        }) {
            CustomLayoutElement *modelData = (
                arena_push(data->arena_frame, sizeof(CustomLayoutElement))
            ); 
            *modelData = (CustomLayoutElement) {
                .type = CUSTOM_LAYOUT_ELEMENT_RAY_TEXTBOX
            };
            CLAY({
                .id = CLAY_ID("CustomTextBox"),
                .backgroundColor = white,
                .layout = {
                    .sizing = layoutExpand
                },
                .custom = {
                    .customData = modelData
                },
            }) {}
            // Clay_OnHover(HandleTextBoxInteraction, (intptr_t)data);
            // Clay_String textbox_string = {
            //     .isStaticallyAllocated = true,
            //     .length = (s32)data->text_box_data.str.len,
            //     .chars = (const char *)data->text_box_data.str.str
            // };
            // CLAY_TEXT(textbox_string, CLAY_TEXT_CONFIG({
            //     .fontId = FONT_ID_BODY_16,
            //     .fontSize = 16,
            //     .textColor = white
            // }));
        }
    }
    Clay_Color current_add_player_button_color;
    if (data->add_player_button_data.add_player_button_state == ADD_PLAYER_BUTTON_NOT_PRESSED) {
        current_add_player_button_color = violet_light;
    }
    else {
        current_add_player_button_color = violet;
    }
    CLAY({
        .id = CLAY_ID("AddPlayerButton"),
        .backgroundColor = (
            Clay_Hovered() ? current_add_player_button_color : violet
        ),
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
        // Clay_OnHover(HandleAddPlayerButtonInteraction, (intptr_t)data);
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

void
LayoutCustomElement(LayoutData *data) {

}

// void
// LayoutCustomElement(LayoutData *data) {
//     CustomLayoutElement *modelData = (
//         arena_push(data->arena_frame, sizeof(CustomLayoutElement))
//     );
//     *modelData = (CustomLayoutElement) { 
//         .type = CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
//         .customData.model = {
//             .model = data->my_model,
//             .texture = data->my_texture,
//             .scale = 0.3f,
//         }
//     };
//     CLAY({ .id = CLAY_ID("CustomElementWindow"),
//         .backgroundColor = violet,
//         .layout = {
//             .layoutDirection = CLAY_LEFT_TO_RIGHT,
//             .sizing = layoutExpand,
//             .padding = CLAY_PADDING_ALL(16),
//             .childGap = 16
//         }
//     }) {
//         CLAY({
//             .id = CLAY_ID("Bridge"),
//             .backgroundColor = header_button_background_color,
//             .layout = {
//                 .sizing = layoutExpand
//             },
//             .custom = {
//                 .customData = modelData
//             },
//         }) {}
//         CLAY({
//             .id = CLAY_ID("Bridge2"),
//             .backgroundColor = violet,
//             .layout = {
//                 .sizing = layoutExpand
//             },
//             .custom = {
//                 .customData = modelData
//             },
//         }) {}
//     }
// }

#endif
