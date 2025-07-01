#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../string.c"
#include "../utils.c"
#include "../names.c"
#include "../registration.c"

int main(void) {
    Ctx ctx = {0};
    ctx_init(&ctx);

    Arena *arena_permanent = arena_alloc(MegaByte(1));

    NameState name_state = { .arena = arena_permanent, .first_free = NULL };
    NameChunkState name_chunk_state = {.arena = arena_permanent, .first_free = NULL};
    RegistrationState registration_state = { .arena = arena_permanent, .first_free = NULL };

    String giulio = string_from_cstring_lit("Giulio");
    String newgiulio = string_from_cstring_lit("NewGiulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");

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

    player_delete(&player_map, &tournament_map, riccardo, &registration_state, &name_state, &name_chunk_state);
    player_rename(&player_map, &tournament_map, &giulio, &newgiulio, &name_state, &name_chunk_state);

    Temp temp = scratch_get(0, 0);

    StringList newgiuliolist = list_missing_tournaments_by_str(temp.arena, &player_map, &tournament_map, newgiulio);
    StringList missing_players = list_missing_players_by_str(temp.arena, &player_map, &tournament_map, ping_pong);

    scratch_release(temp);
}
