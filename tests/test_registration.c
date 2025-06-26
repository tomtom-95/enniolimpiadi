#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../string.c"
#include "../utils.c"
#include "../names.c"
#include "../registration.c"

int main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena *arena = arena_alloc(KiloByte(1));

    NameState name_state = { .arena = arena, .first_free = NULL };
    NameChunkState name_chunk_state = { .arena = arena, .first_free = NULL };
    PlayerState player_state = { .arena = arena, .first_free = NULL };
    TournamentState tournament_state = { .arena = arena, .first_free = NULL };

    PlayerMap player_map = {0};
    TournamentMap tournament_map = {0};

    String giulio = string_from_cstring_lit("Giulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");
    String newriccardo = string_from_cstring_lit("New Riccardo");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    player_map_init(arena, &player_map, 1);
    tournament_map_init(arena, &tournament_map, 1);

    player_enroll_v2(&player_map, &tournament_map, giulio, ping_pong,
        &player_state, &tournament_state, &name_state, &name_chunk_state);
}
