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
    PlayerState player_state = {.arena = arena_permanent, .first_free = NULL};
    TournamentState tournament_state = {.arena = arena_permanent, .first_free = NULL};

    String giulio = string_from_cstring_lit("Giulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    PlayerMap player_map = {0};
    TournamentMap tournament_map = {0};
    player_map_init(arena_permanent, &player_map, 1);
    tournament_map_init(arena_permanent, &tournament_map, 1);

    player_create(&player_map, riccardo, &player_state, &name_state, &name_chunk_state);
    player_create(&player_map, giulio, &player_state, &name_state, &name_chunk_state);
    player_create(&player_map, mario, &player_state, &name_state, &name_chunk_state);

    tournament_create(&tournament_map, ping_pong, &tournament_state, &name_state, &name_chunk_state);
    tournament_create(&tournament_map, machiavelli, &tournament_state, &name_state, &name_chunk_state);

    player_enroll(&player_map, &tournament_map, riccardo, ping_pong,
        &player_state, &tournament_state, &name_state, &name_chunk_state);

    player_enroll(&player_map, &tournament_map, riccardo, ping_pong,
        &player_state, &tournament_state, &name_state, &name_chunk_state);

    // player_enroll(&player_map, &tournament_map, giulio, ping_pong,
    //     &player_state, &tournament_state, &name_state, &name_chunk_state);


    // player_enroll(&player_map, &tournament_map, riccardo, machiavelli,
    //     &player_state, &tournament_state, &name_state, &name_chunk_state);
}
