#ifndef UI_WINDOW_C
#define UI_WINDOW_C

#include "ui_window.h"

void
HandleClayErrors(Clay_ErrorData errorData)
{
    printf("%s", errorData.errorText.chars);
}

Clay_RenderCommandArray 
GetLayout(void)
{
    layoutData.arena_frame->pos = 0;

    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("MainWindow"),
        .backgroundColor = blue,
        .layout = {
            .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
            .padding = CLAY_PADDING_ALL(16)
        }
    }) {
        Clay_OnHover(HandleMainWindowInteraction, (intptr_t)&layoutData);
        CLAY({
            .id = CLAY_ID("TournamentHeaderBar"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(40), .width = CLAY_SIZING_GROW(0) },
                .childGap = 16, .padding = { 16, 16, 0, 0 }, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = gray,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            String str = string_from_lit_comp("Selected tournament: ");
            String tournament_string = string_from_name(layoutData.arena_frame, layoutData.selectedTournament);
            CLAY_TEXT(Clay_String_from_String(string_cat(layoutData.arena_frame, str, tournament_string)), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16, .fontSize = 16, .textColor = white
            }));
        }
        LayoutHeaderBar(&layoutData);
        CLAY({
            .id = CLAY_ID("MainContent"),
            .backgroundColor = gray,
            .layout = {
                .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
                .padding = CLAY_PADDING_ALL(16)
            },
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            switch (layoutData.tab) {
                case TAB_TOURNAMENTS:
                {
                    LayoutTournamentsWindow();
                } break;
                case TAB_PLAYERS:
                {
                    LayoutPlayersWindow();
                } break;
                case TAB_NEW_PLAYER:
                {
                    LayoutAddPlayerWindow();
                } break;
                case TAB_NEW_TOURNAMENT:
                {
                    LayoutAddTournamentWindow();
                } break;
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += layoutData.yOffset;
    }

    return renderCommands;
}

void
LayoutPlayerBox(u32 idx, String str)
{
    CLAY({
        .id = CLAY_IDI("playerchart", idx),
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIT(100), .height = 20 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
        },
        .backgroundColor = white,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
    }) {
        CLAY_TEXT(Clay_String_from_String(str), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = black
        }));
    }
}

void
RecursiveLayout(u32 numPlayers, u32 idx)
{
    // if numPlayers is a power of two
    if (numPlayers == 1) {
        CLAY({
            .layout = {
                .sizing = { .width = CLAY_SIZING_PERCENT(1), .height = CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
            },
            .backgroundColor = yellow
        }) {
            String str = string_from_lit_comp("tusorella");
            LayoutPlayerBox(idx, str);
        }
    }
    else {
        u32 v = numPlayers;      // 32-bit word input to count zero bits on right
        u32 c = 32;              // c will be the number of zero bits on the right
        v &= -(signed)(v);
        if (v) c--;
        if (v & 0x0000FFFF) c -= 16;
        if (v & 0x00FF00FF) c -= 8;
        if (v & 0x0F0F0F0F) c -= 4;
        if (v & 0x33333333) c -= 2;
        if (v & 0x55555555) c -= 1;

        u32 panes = c + 1;

        float left = 1 - 1 / (float)panes;
        float right = 1 - left;
        CLAY({
            .layout = {
                .sizing = layoutExpand, .layoutDirection = CLAY_LEFT_TO_RIGHT
            },
            .backgroundColor = blue_ligth
        }) {
            CLAY({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_PERCENT(left), .height = CLAY_SIZING_GROW(0) },
                    .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}, 
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                },
                .backgroundColor = yellow
            }) {
                RecursiveLayout(numPlayers / 2, 2 * idx + 1);
                RecursiveLayout(numPlayers / 2, 2 * idx + 2);
            }
            CLAY({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_PERCENT(right), .height = CLAY_SIZING_GROW(0) },
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                },
                .backgroundColor = yellow
            }) {
                LayoutPlayerBox(idx, string_from_lit_comp(""));
            }
        }
    }
}

void
LayoutPlayerEnrollListFloating(String str_player_name, TournamentMap *tournament_map)
{
    CLAY({
        .id = CLAY_SID(floatingMenuEnrollListStrId),
        .floating = {
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .attachPoints = {
                .parent = CLAY_ATTACH_POINT_RIGHT_TOP
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
            NameState temp_name_state = name_state_init(layoutData.arena_frame);
            Name player_name = name_init(str_player_name, &temp_name_state.name_chunk_state);

            NameList *tournaments = list_tournaments_not_joined_by_player(tournament_map,
                player_name, &temp_name_state, layoutData.arena_frame);

            NameNode *tournament = tournaments->sentinel.next;
            for (u64 i = 0; i < tournaments->len; ++i) {
                Name name_tournament = tournament->name;
                String str_tournament = string_from_name(layoutData.arena_frame, name_tournament);
                Clay_String clay_str_tournament = Clay_String_from_String(str_tournament);

                CLAY({
                    .layout = { 
                        .sizing = layoutExpand,
                        .padding = CLAY_PADDING_ALL(8)
                    },
                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                }) {
                    // Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
                    CLAY_TEXT(clay_str_tournament, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                }

                tournament = tournament->next;
            }
        }
    }
}
// 
// void
// LayoutFloatingWithdrawList_(String string, RegistrationMap *primary_map, RegistrationMap *link_map)
// {
//     CLAY({
//         .id = CLAY_SID(floatingMenuWithdrawListStrId),
//         .floating = {
//             .attachTo = CLAY_ATTACH_TO_PARENT,
//             .attachPoints = {
//                 .parent = CLAY_ATTACH_POINT_RIGHT_TOP
//             },
//         }
//     }) {
//         CLAY({
//             .layout = {
//                 .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                 .sizing = {
//                     .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                 },
//             },
//             .backgroundColor = blue,
//             .cornerRadius = CLAY_CORNER_RADIUS(8)
//         }) {
//             StringList stringlist = (
//                 list_registrations_by_str(layoutData.arena_frame, primary_map, string)
//             );
//             StringNode *strnode = stringlist.head;
//             while (strnode) {
//                 Clay_String clay_str_registration = Clay_String_from_String(strnode->str);
//                 CLAY({
//                     .layout = { 
//                         .sizing = layoutExpand,
//                         .padding = CLAY_PADDING_ALL(8)
//                     },
//                     .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                     .cornerRadius = CLAY_CORNER_RADIUS(5)
//                 }) {
//                     RegistrationPair *pair = arena_push(layoutData.arena_frame, sizeof(RegistrationPair));
//                     pair->primary_str = push_string_cpy(layoutData.arena_frame, string);
//                     pair->link_str = push_string_cpy(layoutData.arena_frame, strnode->str);
//                     pair->primary_map = primary_map;
//                     pair->link_map = link_map;
//                     Clay_OnHover(HandleWithdrawSelection, (intptr_t)pair);
//                     CLAY_TEXT(clay_str_registration, CLAY_TEXT_CONFIG({
//                         .fontId = FONT_ID_BODY_16,
//                         .fontSize = 16,
//                         .textColor = { 255, 255, 255, 255 }
//                     }));
//                 }
//                 strnode = strnode->next;
//             }
//         }
//     }
// }
// 

// void
// LayoutFloatingMenu_(String string, RegistrationMap *primary_map, RegistrationMap *link_map)
// {
//     CLAY({
//         .id = CLAY_SID(floatingMenuStrId),
//         .floating = {
//             .attachTo = CLAY_ATTACH_TO_PARENT,
//             .attachPoints = {
//                 .parent = CLAY_ATTACH_POINT_RIGHT_CENTER
//             },
//         }
//     }) {
//         CLAY({
//             .layout = {
//                 .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                 .sizing = {
//                     .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                 },
//             },
//             .backgroundColor = blue,
//             .cornerRadius = CLAY_CORNER_RADIUS(8)
//         }) {
//             CLAY({
//                 .id = CLAY_SID(floatingMenuEnrollStrId),
//                 .layout = { 
//                     .sizing = layoutExpand,
//                     .padding = CLAY_PADDING_ALL(8)
//                 },
//                 .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                 .cornerRadius = CLAY_CORNER_RADIUS(5)
//             }) {
//                 CLAY_TEXT(CLAY_STRING("Enroll"), CLAY_TEXT_CONFIG({
//                     .fontId = FONT_ID_BODY_16,
//                     .fontSize = 16,
//                     .textColor = { 255, 255, 255, 255 }
//                 }));
//                 if (Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollStrId)) ||
//                     Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollListStrId)))
//                 {
//                     LayoutFloatingEnrollList_(string, primary_map, link_map);
//                 }
//             }
//             CLAY({
//                 .id = CLAY_SID(floatingMenuWithdrawStrId),
//                 .layout = { 
//                     .sizing = layoutExpand,
//                     .padding = CLAY_PADDING_ALL(8)
//                 },
//                 .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                 .cornerRadius = CLAY_CORNER_RADIUS(5)
//             }) {
//                 CLAY_TEXT(CLAY_STRING("Withdraw"), CLAY_TEXT_CONFIG({
//                     .fontId = FONT_ID_BODY_16,
//                     .fontSize = 16,
//                     .textColor = { 255, 255, 255, 255 }
//                 }));
//                 if (Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawStrId)) ||
//                     Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawListStrId)))
//                 {
//                     LayoutFloatingWithdrawList_(string, primary_map, link_map);
//                 }
//             }
//             CLAY({
//                 .id = CLAY_SID(floatingMenuDeleteStrId),
//                 .layout = { 
//                     .sizing = layoutExpand,
//                     .padding = CLAY_PADDING_ALL(8)
//                 },
//                 .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                 .cornerRadius = CLAY_CORNER_RADIUS(5)
//             }) {
//                 RegistrationPair *pair = arena_push(layoutData.arena_frame, sizeof(RegistrationPair));
//                 pair->primary_map = primary_map;
//                 pair->link_map = link_map;
//                 pair->primary_str = push_string_cpy(layoutData.arena_frame, string);
//                 Clay_OnHover(HandleDeleteRegistrationInteraction, (intptr_t)pair);
//                 CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
//                     .fontId = FONT_ID_BODY_16,
//                     .fontSize = 16,
//                     .textColor = { 255, 255, 255, 255 }
//                 }));
//             }
//         }
//     }
// }

void
LayoutPlayerFloatingMenu(String player_string, TournamentMap *tournament_map)
{
    CLAY({
        .id = CLAY_SID(floatingMenuStrId),
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
            // CLAY({
            //     .id = CLAY_SID(floatingMenuEnrollStrId),
            //     .layout = { 
            //         .sizing = layoutExpand,
            //         .padding = CLAY_PADDING_ALL(8)
            //     },
            //     .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
            //     .cornerRadius = CLAY_CORNER_RADIUS(5)
            // }) {
            //     CLAY_TEXT(CLAY_STRING("Enroll"), CLAY_TEXT_CONFIG({
            //         .fontId = FONT_ID_BODY_16,
            //         .fontSize = 16,
            //         .textColor = { 255, 255, 255, 255 }
            //     }));
            //     if (Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollStrId)) ||
            //         Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollListStrId)))
            //     {
            //         LayoutPlayerEnrollListFloating(player_string, tournament_map);
            //     }
            // }
            // CLAY({
            //     .id = CLAY_SID(floatingMenuWithdrawStrId),
            //     .layout = { 
            //         .sizing = layoutExpand,
            //         .padding = CLAY_PADDING_ALL(8)
            //     },
            //     .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
            //     .cornerRadius = CLAY_CORNER_RADIUS(5)
            // }) {
            //     CLAY_TEXT(CLAY_STRING("Withdraw"), CLAY_TEXT_CONFIG({
            //         .fontId = FONT_ID_BODY_16,
            //         .fontSize = 16,
            //         .textColor = { 255, 255, 255, 255 }
            //     }));
            //     if (Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawStrId)) ||
            //         Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawListStrId)))
            //     {
            //         // LayoutFloatingWithdrawList_(string, primary_map, link_map);
            //     }
            // }
            CLAY({
                .id = CLAY_SID(floatingMenuDeleteStrId),
                .layout = { 
                    .sizing = layoutExpand,
                    .padding = CLAY_PADDING_ALL(8)
                },
                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                // Clay_OnHover(HandleDeleteRegistrationInteraction, (intptr_t)pair);
                CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = { 255, 255, 255, 255 }
                }));
            }
        }
    }
}

// 
// void
// LayoutTournamentFloatingMenu(String tournament_string, RegistrationMap *player_map, RegistrationMap *tournament_map)
// {
//     LayoutFloatingMenu_(tournament_string, tournament_map, player_map);
// }

void
LayoutAddPlayerWindow(void) {
    TextBoxData *textBoxData = &layoutData.addPlayerTextBoxData;
    Clay_Color colorBorder = textBoxData->colorBorder;
    u16 widthBorder = textBoxData->widthBorder;

    CustomLayoutElement *customLayoutElement = (
        arena_push(layoutData.arena_frame, sizeof(CustomLayoutElement))
    );
    customLayoutElement->type = CUSTOM_LAYOUT_TEXTBOX;
    customLayoutElement->customData.textBoxData = &layoutData.addPlayerTextBoxData;
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
        Clay_OnHover(HandleAddPlayerTextBoxInteraction, (intptr_t)0); 
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
        Clay_OnHover(HandleAddPlayerButtonInteraction, (intptr_t)0);
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
        Clay_String feedbackString = (
            Clay_String_from_String(layoutData.feedbackOnAddPlayerButton)
        );
        CLAY_TEXT(feedbackString, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
}

void
LayoutAddTournamentWindow(void)
{
    TextBoxData *textBoxData = &layoutData.addTournamentTextBoxData;
    Clay_Color colorBorder = textBoxData->colorBorder;
    u16 widthBorder = textBoxData->widthBorder;

    CustomLayoutElement *customLayoutElement = (
        arena_push(layoutData.arena_frame, sizeof(CustomLayoutElement))
    );
    customLayoutElement->type = CUSTOM_LAYOUT_TEXTBOX;
    customLayoutElement->customData.textBoxData = &layoutData.addTournamentTextBoxData;
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
        Clay_OnHover(HandleAddTournamentTextBoxInteraction, (intptr_t)0);
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
        Clay_OnHover(HandleAddTournamentButtonInteraction, (intptr_t)0);
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
        Clay_String feedbackString = Clay_String_from_String(layoutData.feedbackOnAddTournamentButton);
        CLAY_TEXT(feedbackString, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = white,
        }));
    }
}

void
LayoutTournamentsWindow(void)
{
    NameState temp_name_state = name_state_init(layoutData.arena_frame);
    NameList *tournament_names = (
        list_all_tournaments(&layoutData.tournament_map,
            &temp_name_state, layoutData.arena_frame)
    );
    CLAY({
        .id = CLAY_ID("TEST"),
        .layout = {
            .sizing = layoutExpand, .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = 16
        },
    }) {
        CLAY({
            .id = CLAY_ID("TournamentSidebar"),
            .layout = {
                .padding = CLAY_PADDING_ALL(5),
                .sizing = { .width = CLAY_SIZING_FIT(200), .height = CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
            },
            .backgroundColor = blue,
            .cornerRadius = CLAY_CORNER_RADIUS(5),
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            NameNode *tournament_node = tournament_names->sentinel.next;
            for (u32 tournamentIdx = 0; tournamentIdx < tournament_names->len; ++tournamentIdx) {
                String tournament_string = string_from_name(layoutData.arena_frame, tournament_node->name);
                CLAY({
                    .id = CLAY_IDI("Tournament", tournamentIdx),
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    },
                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                }) {
                    Clay_OnHover(HandleTournamentSelection, (intptr_t)&tournament_node->name);
                    CLAY({
                        .layout = {
                            .padding = CLAY_PADDING_ALL(4),
                            .sizing = layoutExpand,
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .cornerRadius = CLAY_CORNER_RADIUS(5),
                        .backgroundColor = Clay_Hovered() ? gray_lighter : gray
                    }) {
                        CLAY_TEXT(Clay_String_from_String(tournament_string), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 24,
                            .textColor = white
                        }));
                    }
                }
                tournament_node = tournament_node->next;
            }
        }
        // if (layoutData.selectedTournamentChart) {
        //     u32 numPlayers = 8;
        //     RecursiveLayout(numPlayers, 0);
        //     // LayoutTournamentChart(layoutData.selectedTournamentChart);
        // }
    }
}

void
LayoutPlayersWindow(void)
{
    // NameState temp_name_state = name_state_init(layoutData.arena_frame);
    // NameList *player_names = (
    //     list_all_players(&layoutData.tournament_map,
    //         &temp_name_state, layoutData.arena_frame)
    // );

    u64 i = tournament_find(&layoutData.tournament_map, layoutData.selectedTournament);
    NameList player_names = layoutData.tournament_map.tournaments[i].players_enrolled;

    CLAY({
        .id = CLAY_ID("PlayerSidebar"),
        .layout = {
            .padding = CLAY_PADDING_ALL(5),
            .sizing = { .width = CLAY_SIZING_FIT(200), .height = CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
        },
        .backgroundColor = blue,
        .cornerRadius = CLAY_CORNER_RADIUS(5),
        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
    }) {
        NameNode *player_node = player_names.sentinel.next;
        for (u32 playerIdx = 0; playerIdx < player_names.len; ++playerIdx) {
            String player_string = string_from_name(layoutData.arena_frame, player_node->name);
            CLAY({
                .id = CLAY_IDI("Player", playerIdx),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0) , .height = CLAY_SIZING_FIT(0) },
                    .layoutDirection = { CLAY_LEFT_TO_RIGHT }
                },
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                CLAY({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIT(30), .height = CLAY_SIZING_FIT(30) },
                    },
                    .cornerRadius = CLAY_CORNER_RADIUS(5),
                    .backgroundColor = Clay_Hovered() ? black_light : black
                }) {
                    CustomLayoutElement *customLayoutElementX = (
                        arena_push(layoutData.arena_frame, sizeof(CustomLayoutElement))
                    );
                    customLayoutElementX->type = CUSTOM_LAYOUT_X;
                    Clay_OnHover(HandlePlayerDeletion, (intptr_t)&player_node->name);
                    CLAY({
                        .layout = layoutExpand,
                        .custom = { .customData = customLayoutElementX },
                    }) {}
                }
                CLAY({
                    .layout = {
                        .padding = CLAY_PADDING_ALL(4),
                        .sizing = layoutExpand,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .cornerRadius = CLAY_CORNER_RADIUS(5),
                    .backgroundColor = Clay_Hovered() ? gray_lighter : gray
                }) {
                    // Clay_OnHover(HandlePlayerSelection, (intptr_t)playerIdx);
                    CLAY_TEXT(Clay_String_from_String(player_string), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 24,
                        .textColor = white
                    }));
                }
                // CLAY({
                //     .id = CLAY_IDI("RightPointingTriangle", playerIdx),
                //     .layout = {
                //         .sizing = { .width = CLAY_SIZING_FIT(20), .height = CLAY_SIZING_FIT(30) },
                //     },
                //     .cornerRadius = CLAY_CORNER_RADIUS(5),
                //     .backgroundColor = Clay_Hovered() ? black_light : black
                // }) {
                //     Clay_OnHover(DisplayPlayerOptionMenu, (intptr_t)&player_node->name);
                //     CustomLayoutElement *customLayoutElementY = (
                //         arena_push(layoutData.arena_frame, sizeof(CustomLayoutElement))
                //     );
                //     customLayoutElementY->type = CUSTOM_LAYOUT_RIGHT_POINTING_TRIANGLE;
                //     CLAY({
                //         .layout = layoutExpand,
                //         .custom = { .customData = customLayoutElementY },
                //     }) {}
                // }
                // if (Clay_PointerOver(Clay_GetElementIdWithIndex(CLAY_STRING("RightPointingTriangle"), playerIdx))) {
                //     CLAY({
                //         // .id = CLAY_SID(floatingMenuEnrollListStrId),
                //         .floating = {
                //             .attachTo = CLAY_ATTACH_TO_PARENT,
                //             .attachPoints = {
                //                 .parent = CLAY_ATTACH_POINT_RIGHT_TOP
                //             },
                //         }
                //     }) {
                //         CLAY({
                //             .layout = {
                //                 .layoutDirection = CLAY_TOP_TO_BOTTOM,
                //                 .sizing = {
                //                     .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
                //                 },
                //             },
                //             .backgroundColor = blue,
                //             .cornerRadius = CLAY_CORNER_RADIUS(8)
                //         }) {
                //             CLAY({
                //                 .layout = { 
                //                     .sizing = layoutExpand,
                //                     .padding = CLAY_PADDING_ALL(8)
                //                 },
                //                 .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                //                 .cornerRadius = CLAY_CORNER_RADIUS(5)
                //             }) {
                //                 // Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
                //                 CLAY_TEXT(CLAY_STRING("PingPong"), CLAY_TEXT_CONFIG({
                //                     .fontId = FONT_ID_BODY_16,
                //                     .fontSize = 16,
                //                     .textColor = { 255, 255, 255, 255 }
                //                 }));
                //                 CLAY_TEXT(CLAY_STRING("Machiavelli"), CLAY_TEXT_CONFIG({
                //                     .fontId = FONT_ID_BODY_16,
                //                     .fontSize = 16,
                //                     .textColor = { 255, 255, 255, 255 }
                //                 }));
                //             }
                //         }
                //     }
                // }
            }
            player_node = player_node->next;
        }
    }
}

void
LayoutPlayerBoxTournamentChart(String str, u32 round, u32 num) {
    String str_label = string_from_cstring_lit("label");
    String str_underscore = string_from_cstring_lit("_");

    String *str_round = arena_push(layoutData.arena_frame, sizeof(String));
    str_round->str = arena_push(layoutData.arena_frame, 11);
    str_round->len = snprintf(str_round->str, 11, "%u", round);

    String *str_num = arena_push(layoutData.arena_frame, sizeof(String));
    str_num->str = arena_push(layoutData.arena_frame, 11);
    str_num->len = snprintf(str_num->str, 11, "%u", num);

    String tmp0 = string_cat(layoutData.arena_frame, str_label, str_underscore);
    String tmp1 = string_cat(layoutData.arena_frame, tmp0, *str_round);
    String tmp2 = string_cat(layoutData.arena_frame, tmp1, str_underscore);
    String final = string_cat(layoutData.arena_frame, tmp2, *str_num);

    CLAY({
        .id = CLAY_SID(Clay_String_from_String(final)),
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = 20 },
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
        },
        .backgroundColor = yellow,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
    }) {
        if (str.len != 0) {
            Clay_String clay_str = Clay_String_from_String(str);
            CLAY_TEXT(clay_str, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = black
            }));
        }
    }
}

void
LayoutRounds(StringList players)
{
    StringNode *str_node_player = players.head;
    u32 playersNum = players.len;

    u32 power_of_two = flp2(playersNum);
    u32 remainder = (playersNum - power_of_two) * 2;
    u32 tmp = 2 * power_of_two;

    if (playersNum != power_of_two) {
        // Layout of the first turn
        CLAY({
            .layout = {
                .sizing = layoutExpand,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        }) {
            while (tmp > remainder) {
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                CLAY({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = 20 },
                        .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
                    },
                    .backgroundColor = black,
                    .cornerRadius = CLAY_CORNER_RADIUS(8),
                }) {}
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                --tmp;
            }

            while (tmp) {
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                LayoutPlayerBoxTournamentChart(str_node_player->str, 0, tmp);
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                --tmp;
                str_node_player = str_node_player->next;
            }
        }
    }

    // Layout of all the other turns
    u32 round = 0;
    while (power_of_two) {
        ++round;
        CLAY({
            .layout = {
                .sizing = layoutExpand,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        }) {
            u32 tmp = power_of_two;
            while (tmp > 0) {
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                if (str_node_player) {
                    LayoutPlayerBoxTournamentChart(str_node_player->str, round, tmp);
                    str_node_player = str_node_player->next;
                }
                else {
                    LayoutPlayerBoxTournamentChart(string_from_lit_comp(""), round, tmp);
                }
                CLAY({.layout = {.sizing = layoutExpand}}) {}
                --tmp;
            }
            power_of_two = power_of_two / 2;
        }
    }
}

// void
// LayoutTournamentChart(Name *tournament_name)
// {
//     CLAY({
//         .id = CLAY_ID("TournamentChart"),
//         .layout = {
//             .sizing = layoutExpand,
//             .layoutDirection = CLAY_LEFT_TO_RIGHT,
//             .childGap = 32
//         },
//         .backgroundColor = violet,
//     }) {
//         // Retrieve all the players that are enrolled to the tournament
//         String str_tournament_name = push_string_from_name(layoutData.arena_frame, *tournament_name);
//         StringList players = (
//             list_registrations_by_str(layoutData.arena_frame, layoutData.tournament_map, str_tournament_name)
//         );
// 
//         LayoutRounds(players);
//     }
// }
// 


#endif
