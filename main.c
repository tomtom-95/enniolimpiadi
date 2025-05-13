#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"
#include "ui_layout.c"
#include "ui_registration.c"

#include "utils.c"
#include "registration.c"
#include "names.c"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

Clay_RenderCommandArray 
Wrapper(ClayVideoDemo_Data *data) {
    Clay_RenderCommandArray commands;
    if (open_window == WINDOW_NEW_PLAYER) {
        commands = RenderRegistrationForm(data);
    }
    else {
        commands = RenderMainWindow(data);
    }

    return commands;
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

    // player_destroy(player_map, giulio, &player_free_list, &name_free_list);

    // UI Initialization
    documents.documents[0] = (Document) {
        .title = CLAY_STRING("Players"),
    };
    documents.documents[1] = (Document) {
        .title = CLAY_STRING("Tournaments"),
    };

    buttonsClickMe.buttonsClickMe[0] = (ButtonClickMe) {
        .content = CLAY_STRING("Add Player"),
    };

    buttonsClickMe.buttonsClickMe[1] = (ButtonClickMe) {
        .content = CLAY_STRING("Add Tournament"),
    };

    ClayVideoDemo_Data data = {
        .arena_frame = &arena_frame,
        .arena_permanent = &arena_permanent,
        .player_map = player_map,
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

        // Clay_RenderCommandArray renderCommands = RenderRegistrationForm(&data);
        Clay_RenderCommandArray renderCommands = Wrapper(&data);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }
    // This function is new since the video was published
    Clay_Raylib_Close();
}
