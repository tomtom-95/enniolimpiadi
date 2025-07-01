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
                case TAB_PLAYERS:
                {
                    LayoutPlayersWindow();
                } break;
                case TAB_TOURNAMENTS:
                {
                    LayoutTournamentsWindow();
                } break;
                case TAB_NEW_PLAYER:
                {
                    LayoutAddPlayerWindow(&layoutData);
                } break;
                case TAB_NEW_TOURNAMENT:
                {
                    LayoutAddTournamentWindow(&layoutData);
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
LayoutFloatingMenu_(String string, RegistrationMap *primary_map, RegistrationMap *link_map)
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
            CLAY({
                .id = CLAY_SID(floatingMenuEnrollStrId),
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
            }
            if (Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollStrId)) ||
                Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollListStrId)))
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
                        StringList stringlist = (
                            list_missing_registrations_by_str_(layoutData.arena_frame, primary_map, link_map, string)
                        );
                        // StringList stringlist = list_registrations_by_str(layoutData.arena_frame, primary_map, string);
                        StringNode *strnode = stringlist.head;
                        while (strnode) {
                            Clay_String clay_str = Clay_String_from_String(strnode->str);
                            CLAY({
                                .layout = { 
                                    .sizing = layoutExpand,
                                    .padding = CLAY_PADDING_ALL(8)
                                },
                                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                                .cornerRadius = CLAY_CORNER_RADIUS(5)
                            }) {
                                // Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
                                CLAY_TEXT(clay_str, CLAY_TEXT_CONFIG({
                                    .fontId = FONT_ID_BODY_16,
                                    .fontSize = 16,
                                    .textColor = { 255, 255, 255, 255 }
                                }));
                            }
                            strnode = strnode->next;
                        }
                    }
                }
            }
            CLAY({
                .id = CLAY_SID(floatingMenuWithdrawStrId),
                .layout = { 
                    .sizing = layoutExpand,
                    .padding = CLAY_PADDING_ALL(8)
                },
                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                CLAY_TEXT(CLAY_STRING("Withdraw"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = { 255, 255, 255, 255 }
                }));
                if (Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawStrId)) ||
                    Clay_PointerOver(Clay_GetElementId(floatingMenuWithdrawListStrId)))
                {
                    CLAY({
                        .id = CLAY_SID(floatingMenuWithdrawListStrId),
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
                            // // TODO: logic to withdraw player from tournament
                        }
                    }
                }
            }
            CLAY({
                .id = CLAY_SID(floatingMenuDeleteStrId),
                .layout = { 
                    .sizing = layoutExpand,
                    .padding = CLAY_PADDING_ALL(8)
                },
                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                Clay_OnHover(HandleDeletePlayerInteraction, (intptr_t)&layoutData);
                CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = { 255, 255, 255, 255 }
                }));
            }
        }
    }
}

void
LayoutPlayerFloatingMenu(String player_string, RegistrationMap *player_map, RegistrationMap *tournament_map)
{
    LayoutFloatingMenu_(player_string, player_map, tournament_map);
}

void
LayoutTournamentFloatingMenu(String tournament_string, RegistrationMap *player_map, RegistrationMap *tournament_map)
{
    LayoutFloatingMenu_(tournament_string, tournament_map, player_map);
}

//    CLAY({
//        .id = CLAY_SID(*floating_menu_element_id),  //.id = CLAY_ID("PlayerFloatingMenu"),
//        .floating = {
//            .attachTo = CLAY_ATTACH_TO_PARENT,
//            .attachPoints = {
//                .parent = CLAY_ATTACH_POINT_RIGHT_CENTER
//            },
//        }
//    }) {
//        CLAY({
//            .layout = {
//                .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                .sizing = {
//                    .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                },
//            },
//            .backgroundColor = blue,
//            .cornerRadius = CLAY_CORNER_RADIUS(8)
//        }) {
//            CLAY({
//                .id = CLAY_SID(*enroll_element_id), // CLAY_ID("PlayerEnroll"),
//                .layout = { 
//                    .sizing = layoutExpand,
//                    .padding = CLAY_PADDING_ALL(8)
//                },
//                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                .cornerRadius = CLAY_CORNER_RADIUS(5)
//            }) {
//                CLAY_TEXT(CLAY_STRING("Enroll"), CLAY_TEXT_CONFIG({
//                    .fontId = FONT_ID_BODY_16,
//                    .fontSize = 16,
//                    .textColor = { 255, 255, 255, 255 }
//                }));
//            }
//            if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("PlayerEnroll"))) ||
//                Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FloatingTournamentList"))))
//            {
//                CLAY({
//                    .id = CLAY_SID(*floating_list_id), // CLAY_ID("FloatingTournamentList"),
//                    .floating = {
//                        .attachTo = CLAY_ATTACH_TO_PARENT,
//                        .attachPoints = {
//                            .parent = CLAY_ATTACH_POINT_RIGHT_TOP
//                        },
//                    }
//                }) {
//                    CLAY({
//                        .layout = {
//                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                            .sizing = {
//                                .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                            },
//                        },
//                        .backgroundColor = blue,
//                        .cornerRadius = CLAY_CORNER_RADIUS(8)
//                    }) {
//                        if (layout_player_floating_menu) {
//                            Player *player = player_find(layoutData.player_map, *player_string);
//                            for (u64 idx = 0; idx < layoutData.tournament_map->bucket_count; ++idx) {
//                                Tournament *tournament = layoutData.tournament_map->tournaments[idx]; 
//                                while (tournament) {
//                                    if (!namelist_find(&tournament->player_names, *player_string)) {
//                                        String str_tournament = push_string_from_name(
//                                            layoutData.arena_frame, *(tournament->tournament_name)
//                                        );
//                                        Clay_String clay_str_tournament = Clay_String_from_String(str_tournament);
//                                        CLAY({
//                                            .layout = { 
//                                                .sizing = layoutExpand,
//                                                .padding = CLAY_PADDING_ALL(8)
//                                            },
//                                            .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                                            .cornerRadius = CLAY_CORNER_RADIUS(5)
//                                        }) {
//                                            PlayerTournamentPair *pair = (
//                                                arena_push(layoutData.arena_frame, sizeof(PlayerTournamentPair))
//                                            );
//                                            pair->str_player = *player_string;
//                                            pair->str_tournament = str_tournament;
//                                            Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
//                                            CLAY_TEXT(clay_str_tournament, CLAY_TEXT_CONFIG({
//                                                .fontId = FONT_ID_BODY_16,
//                                                .fontSize = 16,
//                                                .textColor = { 255, 255, 255, 255 }
//                                            }));
//                                        }
//                                    }
//                                    tournament = tournament->next;
//                                }
//                            }
//                        }
//                        else {
//
//                        }
//                    }
//                }
//            }
//            CLAY({
//                .id = CLAY_ID("PlayerWithdraw"),
//                .layout = { 
//                    .sizing = layoutExpand,
//                    .padding = CLAY_PADDING_ALL(8)
//                },
//                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                .cornerRadius = CLAY_CORNER_RADIUS(5)
//            }) {
//                CLAY_TEXT(CLAY_STRING("Withdraw"), CLAY_TEXT_CONFIG({
//                    .fontId = FONT_ID_BODY_16,
//                    .fontSize = 16,
//                    .textColor = { 255, 255, 255, 255 }
//                }));
//                if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("PlayerWithdraw"))) ||
//                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FloatingEnrolledTournamentList"))))
//                {
//                    CLAY({
//                        .id = CLAY_ID("FloatingEnrolledTournamentList"),
//                        .floating = {
//                            .attachTo = CLAY_ATTACH_TO_PARENT,
//                            .attachPoints = {
//                                .parent = CLAY_ATTACH_POINT_RIGHT_TOP
//                            },
//                        }
//                    }) {
//                        CLAY({
//                            .layout = {
//                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                                .sizing = {
//                                    .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                                },
//                            },
//                            .backgroundColor = blue,
//                            .cornerRadius = CLAY_CORNER_RADIUS(8)
//                        }) {
//                            // TODO: logic to withdraw player from tournament
//                            Player *player = player_find(layoutData.player_map, *player_string);
//                            Name *tournament_name = player->tournament_names.first_name;
//                            while (tournament_name) {
//                                String str_tournament = (
//                                    push_string_from_name(layoutData.arena_frame, *tournament_name)
//                                );
//                                Clay_String clay_str_tournament = Clay_String_from_String(str_tournament);
//                                CLAY({
//                                    .layout = { 
//                                        .sizing = layoutExpand,
//                                        .padding = CLAY_PADDING_ALL(8)
//                                    },
//                                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                                    .cornerRadius = CLAY_CORNER_RADIUS(5)
//                                }) {
//                                    CLAY_TEXT(clay_str_tournament, CLAY_TEXT_CONFIG({
//                                        .fontId = FONT_ID_BODY_16,
//                                        .fontSize = 16,
//                                        .textColor = { 255, 255, 255, 255 }
//                                    }));
//                                }
//                                tournament_name = tournament_name->next;
//                            }
//                        }
//                    }
//                }
//            }
//            CLAY({
//                .id = CLAY_ID("PlayerDelete"),
//                .layout = { 
//                    .sizing = layoutExpand,
//                    .padding = CLAY_PADDING_ALL(8)
//                },
//                .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                .cornerRadius = CLAY_CORNER_RADIUS(5)
//            }) {
//                Clay_OnHover(HandleDeletePlayerInteraction, (intptr_t)&layoutData);
//                CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
//                    .fontId = FONT_ID_BODY_16,
//                    .fontSize = 16,
//                    .textColor = { 255, 255, 255, 255 }
//                }));
//            }
//        }
//    }
//}

// void
// LayoutTournamentFloatingMenu(String *tournament_string) {
//     CLAY({
//         .id = CLAY_ID("TournamentFloatingMenu"),
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
//                 .id = CLAY_ID("TournamentEnroll"),
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
//             }
//             if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("TournamentEnroll"))) ||
//                 Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FloatingPlayerList"))))
//             {
//                 CLAY({
//                     .id = CLAY_ID("FloatingPlayerList"),
//                     .floating = {
//                         .attachTo = CLAY_ATTACH_TO_PARENT,
//                         .attachPoints = {
//                             .parent = CLAY_ATTACH_POINT_RIGHT_TOP
//                         },
//                     }
//                 }) {
//                     CLAY({
//                         .layout = {
//                             .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                             .sizing = {
//                                 .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)
//                             },
//                         },
//                         .backgroundColor = blue,
//                         .cornerRadius = CLAY_CORNER_RADIUS(8)
//                     }) {
//                         Tournament *tournament = tournament_find(layoutData.tournament_map, *tournament_string);
//                         for (u64 idx = 0; idx < layoutData.player_map->bucket_count; ++idx) {
//                             Player *player = layoutData.player_map->players[idx]; 
//                             while (player) {
//                                 if (!namelist_find(&player->tournament_names, *tournament_string)) {
//                                     String str_player = (
//                                         push_string_from_name(layoutData.arena_frame, *(player->player_name))
//                                     );
//                                     Clay_String clay_str_player = Clay_String_from_String(str_player);
//                                     CLAY({
//                                         .layout = { 
//                                             .sizing = layoutExpand,
//                                             .padding = CLAY_PADDING_ALL(8)
//                                         },
//                                         .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                                         .cornerRadius = CLAY_CORNER_RADIUS(5)
//                                     }) {
//                                         PlayerTournamentPair *pair = (
//                                             arena_push(layoutData.arena_frame, sizeof(PlayerTournamentPair))
//                                         );
//                                         pair->str_player = str_player;
//                                         pair->str_tournament = *tournament_string;
//                                         Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
//                                         CLAY_TEXT(clay_str_player, CLAY_TEXT_CONFIG({
//                                             .fontId = FONT_ID_BODY_16,
//                                             .fontSize = 16,
//                                             .textColor = { 255, 255, 255, 255 }
//                                         }));
//                                     }
//                                 }
//                                 player = player->next;
//                             }
//                         }
//                     }
//                 }
//             }
//             CLAY({
//                 .id = CLAY_ID("TournamentDelete"),
//                 .layout = { 
//                     .sizing = layoutExpand,
//                     .padding = CLAY_PADDING_ALL(8)
//                 },
//                 .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
//                 .cornerRadius = CLAY_CORNER_RADIUS(5)
//             }) {
//                 Clay_OnHover(HandleDeleteTournamentInteraction, (intptr_t)&layoutData);
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
LayoutPlayersWindow(void) {
    for (int j = 0; j < layoutData.player_map->bucket_count; j++) {
        Registration **player = layoutData.player_map->registrations + j;
        while (*player) {
            // TODO: sloppy
            String *player_string = arena_push(layoutData.arena_frame, sizeof(String));
            String player_string_tmp = push_string_from_name(layoutData.arena_frame, *((*player)->registration_name));
            player_string->len = player_string_tmp.len;
            player_string->str = player_string_tmp.str;

            Clay_String clay_player_string = Clay_String_from_String(player_string_tmp);
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
                    if (layoutData.last_element_clicked.id == CLAY_SID(clay_player_string).id) {
                        LayoutPlayerFloatingMenu(*player_string, layoutData.player_map, layoutData.tournament_map);
                    }
                }
            }
            player = &((*player)->next);
        }
    }
}

void
LayoutTournamentsWindow(void) {
    StringList string_list = list_registrations(layoutData.arena_frame, layoutData.tournament_map);
    StringNode *node = string_list.head;
    while (node) {
        Clay_String clay_string = Clay_String_from_String(node->str);
        CLAY({
            .id = CLAY_SID(clay_string),
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIT(200), .height = 0 },
                .padding = CLAY_PADDING_ALL(12)
            },
            .backgroundColor = Clay_Hovered() ? gray_lighter : gray_light,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleTournamentSelection, (intptr_t)node);
            CLAY({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIT(150) },
                },
            }) {
                CLAY_TEXT(clay_string, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 24,
                    .textColor = white
                }));
                if (string_cmp(layoutData.last_tournament_clicked, node->str)) {
                    LayoutTournamentFloatingMenu(node->str, layoutData.player_map, layoutData.tournament_map);
                }
            }
        }
        node = node->next;
    }
}

#endif
