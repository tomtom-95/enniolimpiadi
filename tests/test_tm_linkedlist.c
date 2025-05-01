#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../tm_utils.c"
#include "../tm_string.c"
#include "../registration.c"


void
registration_test(void) {
    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    String gianni = string_write((u8 *)"Gianni");
    String ping_pong = string_write((u8 *)"Ping Pong");

    Players *players = players_init(&arena);
    Tournaments *tournaments = tournaments_init(&arena);

    players_add(&arena, players, gianni);
    players_register(&arena, players, tournaments, gianni, ping_pong);
}

int
main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/logs/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("tmlinkedlist tests started");
    registration_test();
    log_info("tmlinkedlist tests ended");
}
