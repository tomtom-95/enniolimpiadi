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
GetLayout(LayoutData *data) {
    data->arena_frame->pos = 0;

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
        Clay_OnHover(HandleMainWindowInteraction, (intptr_t)data);
        LayoutHeaderBar(data);
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
            switch (data->tab) {
                case TAB_PLAYERS:
                {
                    LayoutPlayersWindow(data);
                } break;
                case TAB_TOURNAMENTS:
                {
                    LayoutTournamentsWindow(data);
                } break;
                case TAB_NEW_PLAYER:
                {
                    LayoutAddPlayerWindow(data);
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
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
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
        1024,
        768,
        "Enniolimpiadi",
        FLAG_WINDOW_RESIZABLE |
        FLAG_WINDOW_HIGHDPI |
        FLAG_MSAA_4X_HINT |
        FLAG_VSYNC_HINT
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
        .str.len = cstring_len("Enter player name"),
        .str.str = arena_push(&arena_permanent, max_str_len)
    };
    memcpy(
        text_box_data.str.str,
        "Enter player name",
        cstring_len("Enter player name")
    );

    LayoutData data = {
        .arena_frame = &arena_frame,
        .arena_permanent = &arena_permanent,
        .player_map = player_map,
        .player_state = &player_state,
        .name_state = &name_state,
        .name_chunk_state = &name_chunk_state,
        .text_box_data = text_box_data,
        .profilePicture = profilePicture,
        .my_model = my_model,
        .my_texture = my_texture
    };

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

        Clay_RenderCommandArray renderCommands = GetLayout(&data);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }
    // This function is new since the video was published
    Clay_Raylib_Close();
}
