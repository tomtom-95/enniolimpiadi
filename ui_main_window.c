#ifndef UI_LAYOUT_C
#define UI_LAYOUT_C

#include "clay.h"
#include "registration.c"
#include "arena.c"
#include "ui_base.c"

#include <stdlib.h>

Clay_RenderCommandArray 
LayoutMainWindow(ClayVideoDemo_Data *data) {
    data->arena_frame->pos = 0;

    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };

    // Build UI here
    CLAY({ .id = CLAY_ID("OuterContainer"),
        .backgroundColor = {43, 41, 51, 255 },
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Child elements go inside braces
        CLAY({ .id = CLAY_ID("HeaderBar"),
            .layout = {
                .sizing = {
                    .height = CLAY_SIZING_FIXED(60),
                    .width = CLAY_SIZING_GROW(0)
                },
                .padding = { 16, 16, 0, 0 },
                .childGap = 16,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_CENTER
                }
            },
            .backgroundColor = contentBackgroundColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            // Header buttons go here
            CLAY({ .id = CLAY_ID("FileButton"),
                .layout = { .padding = { 16, 16, 8, 8 }},
                .backgroundColor = {140, 140, 140, 255 },
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                CLAY_TEXT(CLAY_STRING("File"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = { 255, 255, 255, 255 }
                }));

                bool fileMenuVisible =
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileButton")))
                    ||
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileMenu")));

                // Below has been changed slightly to fix the small bug where the menu would dismiss when mousing over the top gap
                if (fileMenuVisible) {
                    CLAY({ .id = CLAY_ID("FileMenu"),
                        .floating = {
                            .attachTo = CLAY_ATTACH_TO_PARENT,
                            .attachPoints = {
                                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
                            },
                        },
                        .layout = {
                            .padding = {0, 0, 8, 8 }
                        }
                    }) {
                        CLAY({
                            .layout = {
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .sizing = {
                                    .width = CLAY_SIZING_FIXED(200)
                                },
                            },
                            .backgroundColor = {40, 40, 40, 255 },
                            .cornerRadius = CLAY_CORNER_RADIUS(8)
                        }) {
                            // Render dropdown items here
                            RenderDropdownMenuItem(CLAY_STRING("New"));
                            RenderDropdownMenuItem(CLAY_STRING("Open"));
                            RenderDropdownMenuItem(CLAY_STRING("Close"));
                        }
                    }
                }
            }
            RenderHeaderButton(CLAY_STRING("Edit"));
            CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0) }}}) {}
            RenderHeaderButton(CLAY_STRING("Upload"));
            RenderHeaderButton(CLAY_STRING("Media"));
            RenderHeaderButton(CLAY_STRING("Support"));
        }

        CLAY({
            .id = CLAY_ID("LowerContent"),
            .layout = { .sizing = layoutExpand, .childGap = 16 }
        }) {
            CLAY({
                .id = CLAY_ID("Sidebar"),
                .backgroundColor = contentBackgroundColor,
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 8,
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(250),
                        .height = CLAY_SIZING_GROW(0)
                    }
                }
            }) {
                Clay_LayoutConfig sidebarButtonLayout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16)
                };

                Clay_String player_string = CLAY_STRING("Players");
                Clay_String tournament_string = CLAY_STRING("Tournaments");

                CLAY({
                    .id = CLAY_ID("PlayersTab"),
                    .layout = sidebarButtonLayout,
                    .backgroundColor = (Clay_Color) { 120, 120, 120, Clay_Hovered() ? 120 : 0 },
                    .cornerRadius = CLAY_CORNER_RADIUS(8) 
                }) {
                    Clay_OnHover(HandleSidebarInteraction, (intptr_t)PLAYERS_TAB);
                    CLAY_TEXT(player_string, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 20,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                }

                CLAY({
                    .id = CLAY_ID("TournamentsTab"),
                    .layout = sidebarButtonLayout,
                    .backgroundColor = (Clay_Color) { 120, 120, 120, Clay_Hovered() ? 120 : 0 },
                    .cornerRadius = CLAY_CORNER_RADIUS(8) 
                }) {
                    Clay_OnHover(HandleSidebarInteraction, (intptr_t)TOURNAMENTS_TAB);
                    CLAY_TEXT(tournament_string, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 20,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                }
            }

            CLAY({ .id = CLAY_ID("MainContent"),
                .backgroundColor = contentBackgroundColor,
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 16,
                    .padding = CLAY_PADDING_ALL(16),
                    .sizing = layoutExpand
                }
            }) {
                if (open_tab == PLAYERS_TAB) {
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
                                .textColor = COLOR_WHITE
                            }));
                            players = &((*players)->next);
                        }
                    }
                    CLAY({ .id = CLAY_ID("New Player Button"),
                        .backgroundColor = (Clay_Color) {
                            120, 120, 120, Clay_Hovered() ? 120 : 0
                        },
                        .cornerRadius = CLAY_CORNER_RADIUS(8),
                        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 16,
                            .padding = CLAY_PADDING_ALL(16),
                            .sizing = {
                                .height = CLAY_SIZING_FIXED(60),
                                .width = CLAY_SIZING_GROW(0)
                            },
                        }
                    }) {
                        Clay_OnHover(HandleTextButtonInteraction, (intptr_t)WINDOW_NEW_PLAYER);
                        CLAY_TEXT(CLAY_STRING("New Awesome Player"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 24,
                            .textColor = COLOR_WHITE
                        }));
                    };
                }
                else if (open_tab == TOURNAMENTS_TAB) {
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
                            .textColor = COLOR_WHITE
                        }));
                    }
                    CLAY({ .id = CLAY_ID("New Tournament Button"),
                        .backgroundColor = (Clay_Color) {
                            120, 120, 120, Clay_Hovered() ? 120 : 0
                        },
                        .cornerRadius = CLAY_CORNER_RADIUS(8),
                        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 16,
                            .padding = CLAY_PADDING_ALL(16),
                            .sizing = {
                                .height = CLAY_SIZING_FIXED(60),
                                .width = CLAY_SIZING_GROW(0)
                            },
                        }
                    }) {
                        Clay_OnHover(HandleTextButtonInteraction, (intptr_t)WINDOW_NEW_TOURNAMENT);
                        CLAY_TEXT(CLAY_STRING("New Tournament"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 24,
                            .textColor = COLOR_WHITE
                        }));
                    };
                }
            }
        }
    }


    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
    }
    return renderCommands;
}

#endif // UI_LAYOUT_C
