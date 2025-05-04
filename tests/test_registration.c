#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../tm_utils.c"
#include "../registration.c"
#include "../names.c"

void
registration_test(void) {
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

    PlayerNode *player_node = player_map_find(player_map, giulio);
    small_string_print(player_node->player_name);
}

int main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/logs/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("tm_hashmap tests started");
    registration_test();
    log_info("tm_hashmap tests ended");
}
