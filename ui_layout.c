#include "clay.h"
#include "registration.c"

#include <stdlib.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = { 255, 255, 255, 255};

void RenderHeaderButton(Clay_String text) {
    CLAY({
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void RenderDropdownMenuItem(Clay_String text) {
    CLAY({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

typedef struct {
    Clay_String title;
} Document;

typedef struct {
    Clay_String content;
} ButtonClickMe;

typedef struct {
    Document *documents;
    uint32_t length;
} DocumentArray;

typedef struct {
    ButtonClickMe *buttonsClickMe;
    uint32_t length;
} ButtonClickMeArray;

Document documentsRaw[2];
ButtonClickMe buttonsClickMeRaw[2];

DocumentArray documents = {
    .length = 2,
    .documents = documentsRaw
};

ButtonClickMeArray buttonsClickMe = {
    .length = 2,
    .buttonsClickMe = buttonsClickMeRaw
};

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

typedef struct {
    int32_t selectedDocumentIndex;
    float yOffset;
    ClayVideoDemo_Arena frameArena;
    PlayerMap *player_map;
    Arena *arena;
    int32_t button_index;
} ClayVideoDemo_Data;

typedef struct {
    int32_t requestedDocumentIndex;
    int32_t* selectedDocumentIndex;
} SidebarClickData;

typedef struct {
    int32_t* button_index;
} ButtonClickMeData;

// Define a structure to hold the text display data
typedef struct {
    const char *normalText;
    const char *pressedText;
    bool isPressed;
} TextButtonData;

void HandleSidebarInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    SidebarClickData *clickData = (SidebarClickData*)userData;
    // If this button was clicked
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (
            clickData->requestedDocumentIndex >= 0 &&
            clickData->requestedDocumentIndex < documents.length
        ) {
            // Select the corresponding document
            *clickData->selectedDocumentIndex = clickData->requestedDocumentIndex;
        }
    }
}

// Handler function for the button interaction
void HandleTextButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    ButtonClickMeData *data = (ButtonClickMeData*)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        *(data->button_index) = (*(data->button_index) + 1) % 2;
    } 
}


Clay_RenderCommandArray CreateLayout(ClayVideoDemo_Data *data) {
    data->frameArena.offset = 0;

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
                for (int i = 0; i < documents.length; i++) {
                    Document document = documents.documents[i];
                    Clay_LayoutConfig sidebarButtonLayout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16)
                    };

                    if (i == data->selectedDocumentIndex) {
                        CLAY({
                            .layout = sidebarButtonLayout,
                            .backgroundColor = {120, 120, 120, 255 },
                            .cornerRadius = CLAY_CORNER_RADIUS(8)
                        }) {
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = { 255, 255, 255, 255 }
                            }));
                        }
                    } else {
                        SidebarClickData *clickData = (
                            (SidebarClickData *)(data->frameArena.memory + data->frameArena.offset)
                        );
                        *clickData = (SidebarClickData) {
                            .requestedDocumentIndex = i,
                            .selectedDocumentIndex = &data->selectedDocumentIndex
                        };
                        data->frameArena.offset += sizeof(SidebarClickData);
                        CLAY({
                            .layout = sidebarButtonLayout,
                            .backgroundColor = (Clay_Color) { 120, 120, 120, Clay_Hovered() ? 120 : 0 },
                            .cornerRadius = CLAY_CORNER_RADIUS(8) 
                        }) {
                            Clay_OnHover(HandleSidebarInteraction, (intptr_t)clickData);
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = { 255, 255, 255, 255 }
                            }));
                        }
                    }
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
                // TODO(tommaso): Main content must change depending on the sidebar button pressed
                //                if players button is pressed -> display players name
                //                if tournament button is pressed -> display tournaments name
                for (int i = 0; i < data->player_map->bucket_count; i++) {
                    Players *players = data->player_map->players + i;
                    if (players->head) {
                        PlayerNode *player = players->head;
                        while (player) {
                            Clay_String player_string = {
                                .isStaticallyAllocated = false,
                                .length = player->player_name->size,
                                .chars = (const char *)(player->player_name->str),
                            };
                            CLAY_TEXT(player_string, CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 24,
                                .textColor = COLOR_WHITE
                            }));
                            player = player->next;
                        }
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
                    ButtonClickMeData *buttonClickMeData = (
                        (ButtonClickMeData *)(data->frameArena.memory + data->frameArena.offset)
                    );
                    *buttonClickMeData = (ButtonClickMeData) {
                        .button_index = &(data->button_index)
                    };
                    data->frameArena.offset += sizeof(ButtonClickMeData);

                    Clay_OnHover(HandleTextButtonInteraction, (intptr_t)buttonClickMeData);

                    Clay_String button_text = buttonsClickMe.buttonsClickMe[data->button_index].content;
                    CLAY_TEXT(button_text, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 24,
                        .textColor = COLOR_WHITE
                    }));
                };
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
    }
    return renderCommands;
}