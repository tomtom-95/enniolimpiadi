#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"
#include "clay-video-demo.c"

#include "tm_utils.c"
#include "registration.c"
#include "names.c"

// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
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
    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    SmallString *gianni = small_string_from_cstring(&arena, (u8 *)"Gianni");
    SmallString *giulio = small_string_from_cstring(&arena, (u8 *)"Giulio");
    SmallString *ping_pong = small_string_from_cstring(&arena, (u8 *)"Ping Pong");
    SmallString *machiavelli = small_string_from_cstring(&arena, (u8 *)"Machiavelli");

    PlayerMap *player_map = player_map_init(&arena, 16);
    player_map_add(&arena, player_map, gianni);
    player_map_register(&arena, player_map, gianni, ping_pong);

    player_map_add(&arena, player_map, giulio);
    player_map_register(&arena, player_map, giulio, ping_pong);

    player_map_remove(player_map, giulio);

    player_map_add(&arena, player_map, giulio);
    player_map_register(&arena, player_map, giulio, machiavelli);

    // UI Initialization
    documents.documents[0] = (Document){
        .title = CLAY_STRING("Players"),
        .contents = CLAY_STRING("TODO: every player must be displayed here, one rectangle for player\n")
    };
    documents.documents[1] = (Document){
        .title = CLAY_STRING("Tournaments"),
        .contents = CLAY_STRING("TODO: every tournament must be displayed here, one rectangle for tournament\n")
    };

    ClayVideoDemo_Data data = {
        .frameArena = { .memory = (intptr_t)malloc(1024) },
        .player_map = player_map,
        .arena = &arena,
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

        Clay_RenderCommandArray renderCommands = CreateLayout(&data);

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }
    // This function is new since the video was published
    Clay_Raylib_Close();
}
