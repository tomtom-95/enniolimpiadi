#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../utils.c"
#include "../registration.c"
#include "../names.c"

void
registration_test(void) {
    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    PlayerMap *players = player_map_init(&arena, 16);
    PlayerFreeList player_free_list = {.first_free_entry = NULL};
    NameFreeList name_free_list = {.first_free_entry = NULL};

    String gianni = string_from_cstring((u8 *)"Gianni");
    String giulio = string_from_cstring((u8 *)"Giulio");

    String ping_pong = string_from_cstring((u8 *)"Ping Pong");

    player_create(
        &arena,
        players,
        gianni,
        &player_free_list
    );
    player_create(
        &arena,
        players,
        giulio,
        &player_free_list
    );
    player_enroll(
        &arena,
        players,
        gianni,
        ping_pong,
        &name_free_list
    );
}

int main(void) {
    log_set_level(TMLOG_DEBUG);

    FILE *logfile = fopen("../test_registration.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, TMLOG_TRACE);

    log_info("tm_hashmap tests started");
    registration_test();
    log_info("tm_hashmap tests ended");
}
