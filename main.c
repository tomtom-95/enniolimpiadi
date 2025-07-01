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

    Ctx ctx = {0};
    ctx_init(&ctx);

    Arena *arena_permanent = arena_alloc(MegaByte(1));
    Arena *arena_frame = arena_alloc(MegaByte(1));

    NameState name_state = { .arena = arena_permanent, .first_free = NULL };
    NameChunkState name_chunk_state = { .arena = arena_permanent, .first_free = NULL };
    RegistrationState registration_state = { .arena = arena_permanent, .first_free = NULL };

    String giulio = string_from_cstring_lit("Giulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");
    String newriccardo = string_from_cstring_lit("New Riccardo");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    RegistrationMap player_map = {0};
    RegistrationMap tournament_map = {0};
    registration_map_init(arena_permanent, &player_map, 1);
    registration_map_init(arena_permanent, &tournament_map, 1);

    registration_create(&player_map, riccardo, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, giulio, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, mario, &registration_state, &name_state, &name_chunk_state);

    registration_create(&tournament_map, ping_pong, &registration_state, &name_state, &name_chunk_state);
    registration_create(&tournament_map, machiavelli, &registration_state, &name_state, &name_chunk_state);

    player_enroll(&player_map, &tournament_map, riccardo, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, giulio, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, riccardo, machiavelli, &name_state, &name_chunk_state);


    ////////////////////////////////////////////////////////////////
    // textBoxData initialization

    u16 strLenMax = 255;

    String playerStrOutput = {.len = 0, .str = arena_push(arena_permanent, strLenMax)};
    String playerStrUser = {.len = 0, .str = arena_push(arena_permanent, strLenMax)};

    TextBoxData addPlayerTextBoxData = (TextBoxData) {
        .strLenMax = strLenMax,
        .cursorFrequency = 40,
        .frameCounter = 40,
        .strLabel = string_from_cstring_lit("Enter player name"),
        .strOutput = playerStrOutput,
        .strUser = playerStrUser,
        .keyRepeatDelay = 0.4f,
        .keyRepeatRate = 0.03f,
        .isKeyHeld = false,
        .font = fonts[FONT_ID_BODY_16],
        .fontSize = 16
    };

    String tournamentStrOutput = {.len = 0, .str = arena_push(arena_permanent, strLenMax)};
    String tournamentStrUser = {.len = 0, .str = arena_push(arena_permanent, strLenMax)};

    TextBoxData addTournamentTextBoxData = (TextBoxData) {
        .strLenMax = strLenMax,
        .cursorFrequency = 40,
        .frameCounter = 40,
        .strLabel = string_from_cstring_lit("Enter tournament name"),
        .strOutput = tournamentStrOutput,
        .strUser = tournamentStrUser,
        .keyRepeatDelay = 0.4f,
        .keyRepeatRate = 0.03f,
        .isKeyHeld = false,
        .font = fonts[FONT_ID_BODY_16],
        .fontSize = 16
    };

    ////////////////////////////////////////////////////////////////
    // layoutData initialization

    layoutData = (LayoutData) {
        .arena_frame = arena_frame,
        .arena_permanent = arena_permanent,
        .player_map = &player_map,
        .tournament_map = &tournament_map,
        .registration_state = &registration_state,
        .name_state = &name_state,
        .name_chunk_state = &name_chunk_state,
        .addPlayerTextBoxData = addPlayerTextBoxData,
        .addTournamentTextBoxData = addTournamentTextBoxData
    };

    ////////////////////////////////////////////////////////////////
    // Main loop

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) { .width = GetScreenWidth(), .height = GetScreenHeight() });

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));
        Clay_UpdateScrollContainers(false, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());

        Clay_ElementId addPlayerTextBoxId = Clay_GetElementId(addPlayerTextBoxStr);
        Clay_ElementId addTournamentTextBoxId = Clay_GetElementId(addTournamentTextBoxStr);
        if (Clay_PointerOver(addPlayerTextBoxId) || Clay_PointerOver(addTournamentTextBoxId)) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }

        HelperTextBox(&layoutData.addPlayerTextBoxData, addPlayerTextBoxStr);
        HelperTextBox(&layoutData.addTournamentTextBoxData, addTournamentTextBoxStr);

        Clay_RenderCommandArray renderCommands = GetLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts, layoutData);
        EndDrawing();
    }

    Clay_Raylib_Close();
}
