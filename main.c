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
        .backgroundColor = background_color,
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
            // .backgroundColor = background_color_window,
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
                    LayoutAddTournamentWindow(data);
                } break;
                case TAB_CUSTOM:
                {
                    LayoutCustomElement(data);
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

int main(void) {
    log_set_level(TMLOG_DEBUG);

    FILE *logfile = fopen("../logs/main.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, TMLOG_TRACE);

    Clay_Raylib_Initialize(
        1024, 768,
        "Enniolimpiadi",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    );

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

    // My stuff to test if I can get what I want on the UI
    Arena arena_permanent = arena_alloc(MegaByte(1));
    Arena arena_frame = arena_alloc(MegaByte(1));

    String gianni = string_from_cstring((u8 *)"Gianni");
    String giulio = string_from_cstring((u8 *)"Giulio");
    String ping_pong = string_from_cstring((u8 *)"Ping Pong");
    String machiavelli = string_from_cstring((u8 *)"Machiavelli");

    PlayerFreeList player_free_list = {.first_free_entry = NULL};
    NameFreeList name_free_list = {.first_free_entry = NULL};

    PlayerMap *player_map = player_map_init(&arena_permanent, 16);
    player_create(&arena_permanent, player_map, gianni, &player_free_list);
    player_enroll(&arena_permanent, player_map, gianni, ping_pong, &name_free_list);
    player_enroll(&arena_permanent, player_map, gianni, machiavelli, &name_free_list);

    player_create(&arena_permanent, player_map, giulio, &player_free_list);
    player_enroll(&arena_permanent, player_map, giulio, ping_pong, &name_free_list);

    Texture2D profilePicture = LoadTexture("../resources/Ennio.jpg");
    Model my_model = LoadModel("../resources/bridge.obj");
    LayoutData data = {
        .arena_frame = &arena_frame,
        .arena_permanent = &arena_permanent,
        .player_map = player_map,
        .player_free_list = player_free_list,
        .text_box_data = {0},
        .profilePicture = profilePicture,
        .my_model = my_model
    };

    while (!WindowShouldClose()) {
        // Run once per frame
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
