#ifndef UI_HEADER_C
#define UI_HEADER_C

#include "clay.h"
#include "ui_interaction.c"

void
LayoutHeaderBar(LayoutData *data) {
    CLAY({
        .id = CLAY_ID("HeaderBar"),
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
        .backgroundColor = background_color_header,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY({ .id = CLAY_ID("PlayersButton"),
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = (
                Clay_Hovered() ? header_button_background_color_on_hover: header_button_background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandlePlayersButtonInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("Players"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = { 255, 255, 255, 255 }
            }));
        }
        CLAY({ .id = CLAY_ID("TournamentsButton"),
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = (
                Clay_Hovered() ? header_button_background_color_on_hover: header_button_background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleTournamentsButtonInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("Tournaments"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = { 255, 255, 255, 255 }
            }));
        }
        CLAY({ .id = CLAY_ID("NewPlayerButton"),
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = (
                Clay_Hovered() ? header_button_background_color_on_hover: header_button_background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleNewPlayerButtonInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("New Player"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = { 255, 255, 255, 255 }
            }));
        }
        CLAY({ .id = CLAY_ID("NewTournamentButton"),
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = (
                Clay_Hovered() ? header_button_background_color_on_hover: header_button_background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleNewTournamentButtonInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("New Tournament"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = { 255, 255, 255, 255 }
            }));
        }
        CLAY({ .id = CLAY_ID("CustomElementButton"),
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = (
                Clay_Hovered() ? header_button_background_color_on_hover: header_button_background_color
            ),
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            Clay_OnHover(HandleCustomElementButtonInteraction, (intptr_t)data);
            CLAY_TEXT(CLAY_STRING("Custom Element"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = { 255, 255, 255, 255 }
            }));
        }
    }
}

#endif
