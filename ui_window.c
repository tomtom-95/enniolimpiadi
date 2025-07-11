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
LayoutFloatingEnrollList_(String string, RegistrationMap *primary_map, RegistrationMap *link_map)
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
            StringNode *strnode = stringlist.head;
            while (strnode) {
                Clay_String clay_str_missing_registration = Clay_String_from_String(strnode->str);
                CLAY({
                    .layout = { 
                        .sizing = layoutExpand,
                        .padding = CLAY_PADDING_ALL(8)
                    },
                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                }) {
                    RegistrationPair *pair = arena_push(layoutData.arena_frame, sizeof(RegistrationPair));
                    pair->primary_str = push_string_cpy(layoutData.arena_frame, string);
                    pair->link_str = push_string_cpy(layoutData.arena_frame, strnode->str);
                    pair->primary_map = primary_map;
                    pair->link_map = link_map;
                    Clay_OnHover(HandleEnrollSelection, (intptr_t)pair);
                    CLAY_TEXT(clay_str_missing_registration, CLAY_TEXT_CONFIG({
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

void
LayoutFloatingWithdrawList_(String string, RegistrationMap *primary_map, RegistrationMap *link_map)
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
            StringList stringlist = (
                list_registrations_by_str(layoutData.arena_frame, primary_map, string)
            );
            StringNode *strnode = stringlist.head;
            while (strnode) {
                Clay_String clay_str_registration = Clay_String_from_String(strnode->str);
                CLAY({
                    .layout = { 
                        .sizing = layoutExpand,
                        .padding = CLAY_PADDING_ALL(8)
                    },
                    .backgroundColor = Clay_Hovered() ? blue_ligth : blue,
                    .cornerRadius = CLAY_CORNER_RADIUS(5)
                }) {
                    RegistrationPair *pair = arena_push(layoutData.arena_frame, sizeof(RegistrationPair));
                    pair->primary_str = push_string_cpy(layoutData.arena_frame, string);
                    pair->link_str = push_string_cpy(layoutData.arena_frame, strnode->str);
                    pair->primary_map = primary_map;
                    pair->link_map = link_map;
                    Clay_OnHover(HandleWithdrawSelection, (intptr_t)pair);
                    CLAY_TEXT(clay_str_registration, CLAY_TEXT_CONFIG({
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
                if (Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollStrId)) ||
                    Clay_PointerOver(Clay_GetElementId(floatingMenuEnrollListStrId)))
                {
                    LayoutFloatingEnrollList_(string, primary_map, link_map);
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
                    LayoutFloatingWithdrawList_(string, primary_map, link_map);
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
                RegistrationPair *pair = arena_push(layoutData.arena_frame, sizeof(RegistrationPair));
                pair->primary_map = primary_map;
                pair->link_map = link_map;
                pair->primary_str = push_string_cpy(layoutData.arena_frame, string);
                Clay_OnHover(HandleDeleteRegistrationInteraction, (intptr_t)pair);
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
    StringList string_list = list_registrations(layoutData.arena_frame, layoutData.player_map);
    StringNode *node = string_list.head;
    CLAY({
        .id = CLAY_ID("PlayerSidebar"),
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIT(200), .height = CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
        },
        .backgroundColor = blue,
    }) {
        while (node) {
            String *string = &node->str;
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
                Clay_OnHover(HandlePlayerSelection, (intptr_t)&node->str);
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
                    if (layoutData.last_element_clicked.id == CLAY_SID(clay_string).id) {
                        LayoutPlayerFloatingMenu(*string, layoutData.player_map, layoutData.tournament_map);
                    }
                }
            }
            node = node->next;
        }
    }
}

// void
// LayoutCouple(void) {
//     CLAY({
//         .layout = {
//             .sizing = { .width = CLAY_SIZING_GROW(0), .height = 20 },
//             .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
//         },
//         .backgroundColor = yellow,
//         .cornerRadius = CLAY_CORNER_RADIUS(8),
//     }) {
//         CLAY_TEXT(CLAY_STRING("tusorella1"), CLAY_TEXT_CONFIG({
//             .fontId = FONT_ID_BODY_16,
//             .fontSize = 16,
//             .textColor = black
//         }));
//     }
//     CLAY({.layout = {.sizing = layoutExpand}}) {}
//     CLAY({
//         .layout = {
//             .sizing = { .width = CLAY_SIZING_GROW(0), .height = 20 },
//             .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
//         },
//         .backgroundColor = yellow,
//         .cornerRadius = CLAY_CORNER_RADIUS(8),
//     }) {
//         CLAY_TEXT(CLAY_STRING("tusorella2"), CLAY_TEXT_CONFIG({
//             .fontId = FONT_ID_BODY_16,
//             .fontSize = 16,
//             .textColor = black
//         }));
//     }
// }

void
LayoutSingle(void) {
    CLAY({
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = 20 },
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
        },
        .backgroundColor = yellow,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
    }) {
        CLAY_TEXT(CLAY_STRING("winner"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = black
        }));
    }
}

void
LayoutTurn(int playersNum) {
    while (playersNum > 0) {
        CLAY({.layout = {.sizing = layoutExpand}}) {}
        LayoutSingle();
        CLAY({.layout = {.sizing = layoutExpand}}) {}
        --playersNum;
    }
} 

void
LayoutTournamentsWindow(void) {
    StringList string_list = list_registrations(layoutData.arena_frame, layoutData.tournament_map);
    StringNode *node = string_list.head;
    CLAY({
        .id = CLAY_ID("TEST"),
        .layout = {
            .sizing = layoutExpand, .layoutDirection = CLAY_LEFT_TO_RIGHT, .childGap = 16
        },
    }) {
        CLAY({
            .id = CLAY_ID("TournamentSidebar"),
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIT(200), .height = CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
            },
            .backgroundColor = blue,
        }) {
            while (node) {
                String *string = &node->str;
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
                    // NOTE: node is not actually used
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
                        if (layoutData.last_element_clicked.id == CLAY_SID(clay_string).id) {
                            LayoutTournamentFloatingMenu(node->str, layoutData.player_map, layoutData.tournament_map);
                        }
                    }
                }
                node = node->next;
            }
        }
        CLAY({
            .id = CLAY_ID("TournamentGraphic"),
            .layout = {
                .sizing = layoutExpand,
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 32
            },
            .backgroundColor = violet,
        }) {
            u32 startPlayerNum = 7;
            u32 power_of_two = flp2(startPlayerNum);
            u32 remainder = (startPlayerNum - power_of_two) * 2;

            CLAY({
                .layout = {
                    .sizing = layoutExpand,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                },
            }) {
                u32 tmp = 2 * power_of_two;
                while (tmp) {
                    if (tmp > remainder) {
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
                    }
                    else {
                        CLAY({.layout = {.sizing = layoutExpand}}) {}
                        LayoutSingle();
                        CLAY({.layout = {.sizing = layoutExpand}}) {}
                    }
                    --tmp;
                }
                CLAY({.layout = {.sizing = layoutExpand}}) {}
            }
            while (power_of_two) {
                CLAY({
                    .layout = {
                        .sizing = layoutExpand,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                }) {
                    LayoutTurn(power_of_two);
                    power_of_two = power_of_two / 2;
                }
            }
        }
    }
}

#endif
