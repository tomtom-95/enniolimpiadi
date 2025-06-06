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
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        Clay_OnHover(HandleMainWindowInteraction, (intptr_t)layoutData);
        LayoutHeaderBar(layoutData);
        CLAY({
            .id = CLAY_ID("MainContent"),
            .backgroundColor = gray,
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
            .layout = {
                .sizing = layoutExpand,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 16,
                .padding = CLAY_PADDING_ALL(16),
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
                    // LayoutAddTournamentWindow(data);
                } break;
                case TAB_CUSTOM:
                {
                    // LayoutCustomElement(data);
                }
            }
        }
    }

    // Layout of floating element with animation
    if (layoutData->tab == TAB_NEW_PLAYER) {
        TextBoxData *textBoxData = &(layoutData->text_box_data);

        Clay_ElementId playerTextBoxId = Clay_GetElementId(CLAY_STRING("PlayerTextBox"));
        Clay_ElementId floatingLabelId = Clay_GetElementId(CLAY_STRING("FloatingLabel"));
        Clay_ElementData playerTextBoxElementData = Clay_GetElementData(playerTextBoxId);
        Clay_ElementData floatingLabelElementData = Clay_GetElementData(floatingLabelId);

        s32 fontSize = textBoxData->fontSize;
        Clay_BoundingBox playerTextBoxBoundingBox = playerTextBoxElementData.boundingBox;
        Clay_BoundingBox floatingLabelBoundingBox = floatingLabelElementData.boundingBox;

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

        if (playerTextBoxBoundingBox.x != 0) { // PlayerTextBox has actually been rendered
            if (floatingLabelBoundingBox.x == 0) { // FloatingLabel has not been rendered yet 
                textBoxData->floatingLabelYOffset = (playerTextBoxBoundingBox.height - fontSize) / 2;
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

void
CameraInit() {
    // Camera position
    camera.position = (Vector3){ 50.0f, 50.0f, 50.0f };
    // Camera looking at point
    camera.target = (Vector3){ 0.0f, 10.0f, 0.0f };
    // Camera up vector (rotation towards target)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    // Camera field-of-view Y
    camera.fovy = 30.0f;
    // Camera mode type
    camera.projection = CAMERA_PERSPECTIVE;
}

int main(void) {
    Clay_Raylib_Initialize(
        1024, 768, "Enniolimpiadi",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    );

    CameraInit();

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
        clayRequiredMemory, malloc(clayRequiredMemory)
    );

    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors });

    Clay_SetDebugModeEnabled(true);

    Font fonts[1];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
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

    Texture2D profilePicture = LoadTexture("../resources/Ennio.jpg");

    Model my_model = LoadModel("../resources/castle.obj");
    Texture2D my_texture = LoadTexture("../resources/castle_diffuse.png");

    u32 max_str_len = 64;
    TextBoxData text_box_data = {
        .max_str_len = max_str_len,
        .fontSize = 15,
        .str.len = 0,
        .str.str = arena_push(&arena_permanent, max_str_len),
        .str_final.len = 0,
        .str_final.str = arena_push(&arena_permanent, max_str_len)
    };

    LayoutData layout_data = {
        .arena_frame = &arena_frame,
        .arena_permanent = &arena_permanent,
        .player_map = player_map,
        .player_state = &player_state,
        .name_state = &name_state,
        .name_chunk_state = &name_chunk_state,
        .text_box_data = text_box_data,
        .profilePicture = profilePicture,
        .my_model = my_model,
        .my_texture = my_texture,
    };

    Clay_ElementData elementData = (
        Clay_GetElementData(Clay_GetElementId(CLAY_STRING("PlayerTextBox")))
    );
    float yCenter = elementData.boundingBox.y + elementData.boundingBox.height / 2;
    layout_data.text_box_data.floatingLabelYTop = yCenter;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        });

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState(
            (Clay_Vector2) { mousePosition.x, mousePosition.y },
            IsMouseButtonDown(0)
        );
        Clay_UpdateScrollContainers(
            true,
            (Clay_Vector2) { scrollDelta.x, scrollDelta.y },
            GetFrameTime()
        );

        Clay_RenderCommandArray renderCommands = GetLayout(&layout_data);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts, layout_data);
        EndDrawing();
    }
    // This function is new since the video was published
    Clay_Raylib_Close();
}
