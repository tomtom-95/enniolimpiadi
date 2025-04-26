#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../tmutils.h"
#include "../log.c"
#include "../tmhashmap.c"

void
test_tmhashmap_add_pippo_2(
    Arena *arena_hashmap,
    Arena *arena_string
)
{
    /* Try to add the same string twice to the hashmap, test that correctly return error */
    HashMap hash_map = hashmap_init(arena_hashmap, 16);

    /* "Pippo" is hashed to 13 */
    String player_0 = tmstring_arena_write(arena_string, str_lit("Pippo"));

    hashmap_add(
        arena_hashmap, hash_map, player_0, 42
    );
    uint8_t res = hashmap_add(
        arena_hashmap, hash_map, player_0, 43
    );

    if (res == 0) {
        log_error("test failed");
    }

    if (hash_map.bucket_state[13].first_bucket_node->data == 42) {
        log_info("test succeded");
    }
    else {
        log_error("test failed");
    }
}

void
test_tmhashmap_add_pippo(
    Arena *arena_hashmap,
    Arena *arena_string
)
{
    HashMap hash_map = hashmap_init(arena_hashmap, 16);

    /* "Pippo" is hashed to 13 */
    String player_0 = tmstring_arena_write(arena_string, str_lit("Pippo"));

    uint8_t res = hashmap_add(
        arena_hashmap, hash_map, player_0, 42
    );
    if (res != 0) {
        log_error("test failed");
    }

    if (hash_map.bucket_state[13].first_bucket_node->data == 42) {
        log_info("test succeded");
    }
    else {
        log_error("test failed");
    }
}

int main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("tmhashmap tests started");

    Arena arena_hashmap = arena_alloc(MegaByte(1));
    if (!arena_hashmap.data) {
        log_error("test failed");
    }

    Arena arena_string = arena_alloc(MegaByte(1));
    if (!arena_string.data) {
        log_error("test failed");
    }

    test_tmhashmap_add_pippo(&arena_hashmap, &arena_string);
    test_tmhashmap_add_pippo_2(&arena_hashmap, &arena_string);
}
