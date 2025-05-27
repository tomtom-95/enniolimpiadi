#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../string.c"
#include "../utils.c"
#include "../names.c"
#include "../registration_v2.c"

int main(void) {
    log_set_level(TMLOG_DEBUG);

    Arena arena = arena_alloc(KiloByte(1));
    NameChunkState name_chunk_state = {.arena = &arena, .first_free = NULL};
    PlayerState player_state = {.arena = &arena, .first_free = NULL};

    PlayerMap *player_map = player_map_init(&arena, 1);
    player_create(&name_chunk_state, &player_state, player_map, string_from_cstring_lit("Riccardo"));
    player_create(&name_chunk_state, &player_state, player_map, string_from_cstring_lit("Giulio"));
    player_create(&name_chunk_state, &player_state, player_map, string_from_cstring_lit("Mario"));
    player_delete(&name_chunk_state, &player_state, player_map, string_from_cstring_lit("Giulio"));
}
