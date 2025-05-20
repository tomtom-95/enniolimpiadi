#ifndef UI_WINDOW_C
#define UI_WINDOW_C

#include <stdlib.h>

#include "arena.c"
#include "registration.c"

#include "clay.h"
#include "raylib/raylib.h"
#include "ui_interaction.c"

void
LayoutPlayersWindow(LayoutData *data) {
    for (int j = 0; j < data->player_map->bucket_count; j++) {
        PlayerNode **players = data->player_map->players + j;
        while (*players) {
            Clay_String player_string = {
                .isStaticallyAllocated = false,
                .length = (*players)->player_name.size,
                .chars = (const char *)((*players)->player_name.str),
            };
            CLAY_TEXT(player_string, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 24,
                .textColor = text_color
            }));
            players = &((*players)->next);
        }
    }
}

void
LayoutTournamentsWindow(LayoutData *data) {
    TournamentNamesArray tournament_names_array = (
        list_tournaments(data->arena_frame, data->player_map)
    );
    for (u64 i = 0; i < tournament_names_array.count; i++) {
        Clay_String tournament_string = {
            .isStaticallyAllocated = false,
            .length = tournament_names_array.names[i].size,
            .chars = (const char *)(tournament_names_array.names[i].str),
        };
        CLAY_TEXT(tournament_string, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 24,
            .textColor = text_color
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
            .backgroundColor = (
                Clay_Hovered() ? background_color_on_hover : background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(6),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(40)
                },
                .padding = CLAY_PADDING_ALL(12)
            }
        }) {
            Clay_OnHover(HandleTextBoxInteraction, (intptr_t)(&(data->text_box_data)));
            String *tmp_str = arena_push(data->arena_frame, sizeof(String));
            if ((data->text_box_data).letter_count == 0) {
                String player_name = string_from_cstring((u8 *)"Enter a player name");
                memcpy(tmp_str, &player_name, sizeof(String));
            }
            else {
                if (Clay_Hovered()) {
                    memcpy(
                        tmp_str->str,
                        (data->text_box_data).name,
                        (data->text_box_data).letter_count+1
                    );
                    tmp_str->size = (data->text_box_data).letter_count;
                    if ((((data->text_box_data).frame_counter/60)%2) == 1) {
                        tmp_str->str[tmp_str->size] = '|';
                        tmp_str->str[++(tmp_str->size)] = '\0';
                    }
                }
            }
            Clay_String textbox_string = {
                .isStaticallyAllocated = true,
                .length = tmp_str->size,
                .chars = tmp_str->str
            };
            CLAY_TEXT(textbox_string, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = text_color
            }));
        }
    }
    CLAY({
        .id = CLAY_ID("AddPlayerButton"),
        .backgroundColor = (
            Clay_Hovered() ? add_player_button_color_on_hover : add_player_button_color
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
        Clay_OnHover(HandleAddPlayerButtonInteraction, (intptr_t)data);
        CLAY_TEXT(CLAY_STRING("Add Player"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = text_color,
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
