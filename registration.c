#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "log.c"

#include "tm_arena.c"
#include "tm_hashmap.c"


typedef struct Player Player;
typedef struct Tournament Tournament;
typedef struct PlayerRegistration PlayerRegistration;

struct Player {
    String player_name;
    Player *next;
};

struct Tournament {
    String tournament_name;
    Tournament *next;
};

struct PlayerRegistration {
    String player_name;
    Tournament *tournament;
    PlayerRegistration *next;
};


HashMap
player_registration_init(
    Arena *arena,
    uint64_t bucket_count
) {
    log_info("Initializing hashmap used for player registration ...");

    HashMap hash_map_player_registration = hashmap_init(arena, bucket_count);

    log_info("Hashmap used for player registration successfully created");

    return hash_map_player_registration;
}

void
player_create(
    Arena *arena,
    HashMap player_registration,
    String player_name
) {
    log_info("Creating a new player ...");
    /* Create a player and add it to the player registration hashmap */
    /* TODO(tommaso): error handling */
    hashmap_add(arena, player_registration, player_name);

    log_info("New player successfully created");
}

void
player_register_to_tournament(
    Arena *arena_player_registration,
    Arena *arena_tournament,
    HashMap player_registration,
    String player_name,
    String tournament_name
) {
    log_info("Registering player to a tournament ...");

    BucketNode *player = hashmap_find(player_name, player_registration);

    Tournament **tournament = (Tournament **)(&(player->data));
    while (*tournament) {
        if (
            tmstring_are_equal(
                (*tournament)->tournament_name, tournament_name
            )
        ) {
            /* TODO(tommaso): error, this should not happen */
        }
        tournament = &((*tournament)->next);
    }

    if (player.)

    log_info("To be completed ...");
}

#endif /* REGISTRATION_C */
