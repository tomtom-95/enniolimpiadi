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
#include "drawing.c"

void UpdatePointerLogic(void)
{
    /* 1. Only care about the frame the button went down */
    if (Clay_GetCurrentContext()->pointerInfo.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
        return;

    /* 2. Ask Clay (public API) which elements are under the cursor */
    Clay_ElementIdArray over = Clay_GetPointerOverIds();
    if (over.length == 0)
        return;                    /* nothing hit */

    /* 3. over[0] is the front-most element; use length-1 for back-most */
    g_lastClicked = *Clay_ElementIdArray_Get(&over, over.length - 1);
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

    Clay_SetDebugModeEnabled(true);

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
    String persona1 = string_from_cstring_lit("Persona1");
    String persona2 = string_from_cstring_lit("Persona2");
    String persona3 = string_from_cstring_lit("Persona3");
    String persona4 = string_from_cstring_lit("Persona4");
    String persona5 = string_from_cstring_lit("Persona5");
    String persona6 = string_from_cstring_lit("Persona6");
    String persona7 = string_from_cstring_lit("Persona7");
    String persona8 = string_from_cstring_lit("Persona8");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    RegistrationMap player_map = {0};
    RegistrationMap tournament_map = {0};
    registration_map_init(arena_permanent, &player_map, 1);
    registration_map_init(arena_permanent, &tournament_map, 1);

    registration_create(&player_map, riccardo, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, giulio, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, mario, &registration_state, &name_state, &name_chunk_state);

    registration_create(&player_map, persona1, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona2, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona3, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona4, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona5, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona6, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona7, &registration_state, &name_state, &name_chunk_state);
    registration_create(&player_map, persona8, &registration_state, &name_state, &name_chunk_state);

    registration_create(&tournament_map, ping_pong, &registration_state, &name_state, &name_chunk_state);
    registration_create(&tournament_map, machiavelli, &registration_state, &name_state, &name_chunk_state);

    player_enroll(&player_map, &tournament_map, riccardo, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, giulio, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, mario, ping_pong, &name_state, &name_chunk_state);

    player_enroll(&player_map, &tournament_map, persona1, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona2, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona3, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona4, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona5, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona6, ping_pong, &name_state, &name_chunk_state);
    player_enroll(&player_map, &tournament_map, persona7, ping_pong, &name_state, &name_chunk_state);
    // player_enroll(&player_map, &tournament_map, persona8, ping_pong, &name_state, &name_chunk_state);

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
        .addTournamentTextBoxData = addTournamentTextBoxData,
        .playerIdx = -1
    };

    ////////////////////////////////////////////////////////////////
    // Main loop

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) { .width = GetScreenWidth(), .height = GetScreenHeight() });

        UpdatePointerLogic();

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

        // TODO: add condition && layoutData.last_element_clicked not one of the tournament name in the sidebar
        //       handling stuff by CLAY_ID is starting to become really a pain in the ...
        if (layoutData.selectedTournamentChart) {
            String str_tournament_name = (
                push_string_from_name(layoutData.arena_frame, *layoutData.selectedTournamentChart)
            );
            StringList str_list = (
                list_registrations_by_str(layoutData.arena_frame,
                    layoutData.tournament_map, str_tournament_name)
            );

            DrawBezierCurves(8 - 1); 
            // DrawBezierCurves(str_list.len); 
        }


        EndDrawing();
    }

    Clay_Raylib_Close();
}
