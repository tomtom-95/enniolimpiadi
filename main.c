#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>

#include "log.c"

#include "tm_utils.c"
#include "tm_arena.c"
#include "tm_string.c"
#include "tm_hashmap.c"

#include "registration.c"


int main(void)
{
    log_set_level(LOG_INFO);

    FILE *logfile = fopen("app.log", "w");
    if (!logfile)
    {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("Application started");
    
    Arena arena_string = arena_alloc(MegaByte(1));
    if (!arena_string.data) {
        return 1;
    }
    Arena arena_player = arena_alloc(MegaByte(1));
    if (!arena_player.data) {
        return 1;
    }
    Arena arena_tournament = arena_alloc(MegaByte(1));
    if (!arena_tournament.data) {
        return 1;
    }
    Arena arena_player_registration = arena_alloc(MegaByte(1));
    if (!arena_player_registration.data) {
        return 1;
    }

    String pippo = tmstring_arena_write(&arena_string, str_lit("Pippo"));
    String ping_pong = tmstring_arena_write(&arena_string, str_lit("Ping Pong"));

    HashMap player_registration = (
        player_registration_init(&arena_player_registration, 16)
    );

    player_create(
        &arena_player_registration,
        player_registration,
        pippo
    );
    player_register(
        &arena_player_registration,
        player_registration,
        pippo,
        ping_pong
    );

    log_info("Application Ended");

    return 0;
}
