#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "raylib/clay_renderer_raylib.c"

#include "ui_utils.h"
#include "ui_window.c"
#include "ui_header.c"
#include "ui_interaction.c"

#include "utils.c"
#include "registration.c"
#include "names.c"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

Clay_RenderCommandArray 
GetLayout(LayoutData *layoutData) {
    layoutData->arena_frame->pos = 0;

    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("MainWindow"),
        .backgroundColor = blue,
        .layout = {
            .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
            .padding = CLAY_PADDING_ALL(16)
        }
    }) {
        Clay_OnHover(HandleMainWindowInteraction, (intptr_t)layoutData);
        LayoutHeaderBar(layoutData);
        CLAY({
            .id = CLAY_ID("MainContent"),
            .backgroundColor = gray,
            .layout = {
                .sizing = layoutExpand, .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 16,
                .padding = CLAY_PADDING_ALL(16)
            },
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            switch (layoutData->tab) {
                case TAB_PLAYERS:
                {
                    LayoutPlayersWindow(layoutData);
                } break;
                case TAB_TOURNAMENTS:
                {
                    LayoutTournamentsWindow(layoutData);
                } break;
                case TAB_NEW_PLAYER:
                {
                    LayoutAddPlayerWindow(layoutData);
                } break;
                case TAB_NEW_TOURNAMENT:
                {
                    LayoutTextBoxV2(layoutData);
                } break;
            }
        }
    }

    // Layout of floating element with animation
    if (layoutData->tab == TAB_NEW_PLAYER) {
        TextBoxData *textBoxData = &(layoutData->text_box_data);

        Clay_ElementId playerTextBoxId = Clay_GetElementId(CLAY_STRING("PlayerTextBox"));
        Clay_ElementData playerTextBoxElementData = Clay_GetElementData(playerTextBoxId);
        Clay_BoundingBox playerTextBoxBoundingBox = playerTextBoxElementData.boundingBox;

        Clay_ElementId textContainerId = Clay_GetElementId(CLAY_STRING("TextContainer"));
        Clay_ElementData textContainerElementData = Clay_GetElementData(textContainerId);
        Clay_BoundingBox textContainerBoundingBox = textContainerElementData.boundingBox;

        Clay_ElementId floatingLabelId = Clay_GetElementId(CLAY_STRING("FloatingLabel"));
        Clay_ElementData floatingLabelElementData = Clay_GetElementData(floatingLabelId);
        Clay_BoundingBox floatingLabelBoundingBox = floatingLabelElementData.boundingBox;

        s32 fontSize = textBoxData->fontSize;

        // PlayerTextBox has actually been rendered
        // This code will run starting from the second time that the textbox is rendered
        if (playerTextBoxBoundingBox.x != 0) { 
            // FloatingLabel has not been rendered yet: set his position to be
            // the middle of the text box without any transition
            if (floatingLabelBoundingBox.x == 0) {
                textBoxData->floatingLabelYOffset = (playerTextBoxBoundingBox.height - fontSize) / 2;
            }
            else {
                bool isTextBoxClicked = (
                    layoutData->last_element_clicked.id == playerTextBoxId.id ||
                    layoutData->last_element_clicked.id == floatingLabelId.id
                );
                if (isTextBoxClicked) {
                    float yOffset = -(fontSize / 2);
                    if (textBoxData->floatingLabelYOffset > yOffset) {
                        textBoxData->floatingLabelYOffset -= 2;
                    }
                }
                else {
                    if (textBoxData->str.len == 0) {
                        float yOffset = (playerTextBoxBoundingBox.height - fontSize) / 2;
                        if (textBoxData->floatingLabelYOffset < yOffset) {
                            textBoxData->floatingLabelYOffset += 2;
                        }
                    }
                }
            }
            CLAY({
                .id = CLAY_ID("FloatingLabel"),
                .backgroundColor = gray,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { .left = 5, .right = 5, .top = 0, .bottom = 0 }
                },
                .floating = {
                    .parentId = playerTextBoxId.id,
                    .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
                    .attachPoints = {
                        .element = CLAY_ATTACH_POINT_LEFT_TOP,
                        .parent = CLAY_ATTACH_POINT_LEFT_TOP
                    },
                    .offset = {10, textBoxData->floatingLabelYOffset}
                }
            }) {
                Clay_OnHover(HandleFloatingLabelInteraction, (intptr_t)layoutData);
                CLAY_TEXT(CLAY_STRING("Enter new player name"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16, .fontSize = (u16)fontSize, .textColor = white
                }));
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();


    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += layoutData->yOffset;
    }

    return renderCommands;
}

int
main(void) {
    Clay_Raylib_Initialize(
        1024, 768, "Enniolimpiadi",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    );

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
        clayRequiredMemory, malloc(clayRequiredMemory)
    );

    Clay_Initialize(
        clayMemory, (Clay_Dimensions) {.width = GetScreenWidth(), .height = GetScreenHeight()},
        (Clay_ErrorHandler) { HandleClayErrors }
    );

    Clay_SetDebugModeEnabled(false);

    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/FiraCode-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Arena arena_permanent = arena_alloc(MegaByte(1));
    Arena arena_frame = arena_alloc(MegaByte(1));

    NameState name_state = {.arena = &arena_permanent, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = &arena_permanent, .first_free = NULL};
    PlayerState player_state = {.arena = &arena_permanent, .first_free = NULL};

    String giulio = string_from_cstring_lit("Giulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");
    String newriccardo = string_from_cstring_lit("New Riccardo");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    PlayerMap *player_map = player_map_init(&arena_permanent, 1);
    player_create(&name_chunk_state, &player_state, player_map, riccardo);
    player_create(&name_chunk_state, &player_state, player_map, giulio);
    player_create(&name_chunk_state, &player_state, player_map, mario);
    player_enroll(&name_state, &name_chunk_state, player_map, riccardo, ping_pong);
    player_enroll(&name_state, &name_chunk_state, player_map, riccardo, machiavelli);
    player_rename(player_map, &name_chunk_state, riccardo, newriccardo);

    u32 max_str_len = 256;
    TextBoxData text_box_data = {
        .font_id = FONT_ID_BODY_16,
        .cursor_position = 0,
        .max_str_len = max_str_len,
        .fontSize = 16,
        .str.len = 0,
        .str.str = arena_push(&arena_permanent, max_str_len),
        .scrollOffset = 0
    };

    ////////////////////////////////////////////////////////////////
    // textBoxDataV2 initialization
    u16 strLenMax = 255;
    String strOutput = {.len = 0, .str = arena_push(&arena_permanent, strLenMax)};
    String strUser = {.len = 0, .str = arena_push(&arena_permanent, strLenMax)};
    TextBoxDataV2 textBoxDataV2 = {
        .strLenMax = strLenMax,
        .cursorFrequency = 40,
        .frameCounter = 40,
        .strLabel = string_from_cstring_lit("Enter player name"),
        .strOutput = strOutput,
        .strUser = strUser,
        .backspaceHeld = false,
        .backspaceRepeatDelay = 0.4f,
        .backspaceRepeatRate = 0.03f,
        .font = fonts[FONT_ID_BODY_16],
        .fontSize = 16
    };

    ////////////////////////////////////////////////////////////////
    // layout_data initialization
    LayoutData layout_data = {
        .arena_frame = &arena_frame,
        .arena_permanent = &arena_permanent,
        .player_map = player_map,
        .player_state = &player_state,
        .name_state = &name_state,
        .name_chunk_state = &name_chunk_state,
        .text_box_data = text_box_data,
        .textBoxDataV2 = textBoxDataV2
    };

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) { .width = GetScreenWidth(), .height = GetScreenHeight() });

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));

        if (layout_data.textBoxDataV2.trackpadHeld) {
            Clay_UpdateScrollContainers(false, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());
        }
        else {
            Clay_UpdateScrollContainers(true, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());
        }

        Clay_RenderCommandArray renderCommands = GetLayout(&layout_data);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts, layout_data);
        EndDrawing();
    }

    Clay_Raylib_Close();
}
