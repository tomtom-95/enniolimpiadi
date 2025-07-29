#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "raylib/clay_renderer_raylib.c"

#include "ui_utils.h"
#include "ui_window.c"
#include "ui_header.c"
#include "ui_interaction.c"

#include "utils.c"
#include "names.c"
#include "namelist.c"
#include "drawing.c"
#include "registration_v2.c"

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
        clayMemory, (Clay_Dimensions){ .width = GetScreenWidth(), .height = GetScreenHeight() },
        (Clay_ErrorHandler){ HandleClayErrors }
    );

    Clay_SetDebugModeEnabled(true);

    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/FiraCode-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Ctx ctx = {0};
    ctx_init(&ctx);

    Arena *arena_permanent = arena_alloc(MegaByte(20));
    Arena *arena_frame = arena_alloc(MegaByte(20));

    NameState name_state = name_state_init(arena_permanent);
    TournamentMap tournament_map = tournament_map_init(arena_permanent, 16);

    // Tournaments
    Name ping_pong   = name_init(string_from_cstring_lit("Ping Pong"), &name_state.name_chunk_state);
    Name machiavelli = name_init(string_from_cstring_lit("Machiavelli"), &name_state.name_chunk_state);

    // Players
    Name persona1  = name_init(string_from_cstring_lit("Persona1"), &name_state.name_chunk_state);
    Name persona2  = name_init(string_from_cstring_lit("Persona2"), &name_state.name_chunk_state);
    Name persona3  = name_init(string_from_cstring_lit("Persona3"), &name_state.name_chunk_state);
    Name persona4  = name_init(string_from_cstring_lit("Persona4"), &name_state.name_chunk_state);
    Name persona5  = name_init(string_from_cstring_lit("Persona5"), &name_state.name_chunk_state);
    Name persona6  = name_init(string_from_cstring_lit("Persona6"), &name_state.name_chunk_state);
    Name persona7  = name_init(string_from_cstring_lit("Persona7"), &name_state.name_chunk_state);
    Name persona8  = name_init(string_from_cstring_lit("Persona8"), &name_state.name_chunk_state);
    Name persona9  = name_init(string_from_cstring_lit("Persona9"), &name_state.name_chunk_state);
    Name persona10 = name_init(string_from_cstring_lit("Persona10"), &name_state.name_chunk_state);

    tournament_add(&tournament_map, ping_pong, &name_state);
    tournament_add(&tournament_map, machiavelli, &name_state);

    // TODO: how can I track player not registered to any tournament?
    // tournament_player_enroll(&tournament_map, ping_pong, persona1, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona2, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona3, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona4, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona5, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona6, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona7, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona8, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona9, &name_state);
    tournament_player_enroll(&tournament_map, ping_pong, persona10, &name_state);

    ////////////////////////////////////////////////////////////////
    // layoutData initialization

    String playerStrOutput     = { .len = 0, .str = arena_push(arena_permanent, strLenMax) };
    String playerStrUser       = { .len = 0, .str = arena_push(arena_permanent, strLenMax) };
    String tournamentStrOutput = { .len = 0, .str = arena_push(arena_permanent, strLenMax) };
    String tournamentStrUser   = { .len = 0, .str = arena_push(arena_permanent, strLenMax) };

    layoutData = (LayoutData){
        .arena_frame     = arena_frame,
        .arena_permanent = arena_permanent,
        .tournament_map  = tournament_map,
        .name_state      = name_state,
        .playerIdx       = -1,
    };

    layoutData.addPlayerTextBoxData = (TextBoxData) {
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

    layoutData.addTournamentTextBoxData = (TextBoxData) {
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
        // if (layoutData.selectedTournamentChart) {
        //     String str_tournament_name = (
        //         string_from_name(layoutData.arena_frame, *layoutData.selectedTournamentChart)
        //     );
        //     // StringList str_list = (
        //     //     list_registrations_by_str(layoutData.arena_frame,
        //     //         layoutData.tournament_map, str_tournament_name)
        //     // );

        //     DrawBezierCurves(8 - 1); 
        //     // DrawBezierCurves(str_list.len); 
        // }

        EndDrawing();
    }

    Clay_Raylib_Close();
}
