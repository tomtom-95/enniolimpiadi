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
    NameState name_state = {.arena = arena, .first_free = NULL};
    NameChunkState name_chunk_state = {.arena = arena, .first_free = NULL};
    PlayerState player_state = {.arena = arena, .first_free = NULL};

    String giulio = string_from_cstring_lit("Giulio");
    String riccardo = string_from_cstring_lit("Riccardo");
    String mario = string_from_cstring_lit("Mario");
    String newriccardo = string_from_cstring_lit("New Riccardo");

    String ping_pong = string_from_cstring_lit("Ping Pong");
    String machiavelli = string_from_cstring_lit("Machiavelli");

    PlayerMap *player_map = player_map_init(&arena, 1);

    player_create(&name_chunk_state, &player_state, player_map, riccardo);
    player_create(&name_chunk_state, &player_state, player_map, giulio);
    player_create(&name_chunk_state, &player_state, player_map, mario);
    player_create(&name_chunk_state, &player_state, player_map, riccardo);


    player_enroll(&name_state, &name_chunk_state, player_map, riccardo, ping_pong);
    player_enroll(&name_state, &name_chunk_state, player_map, riccardo, machiavelli);

    player_withdraw(&name_state, &name_chunk_state, player_map, riccardo, ping_pong);
    player_rename(player_map, &name_chunk_state, riccardo, newriccardo);
}
