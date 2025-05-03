#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "tm_utils.c"
#include "tm_string.c"
#include "tm_linkedlist.c"

typedef struct PlayerMap PlayerMap;

typedef struct Players Players;
typedef struct PlayerNode PlayerNode;

typedef struct TournamentNode TournamentNode;

struct PlayerMap {
    u64 bucket_count;
    Players *players;
    TournamentNode *tournament_free_entry;
};

struct Players {
    PlayerNode *head;
    PlayerNode *first_free_entry;
};

struct PlayerNode {
    String player_name;
    TournamentNode *tournament_head;
    PlayerNode *next;
};

struct TournamentNode {
    String tournament_name;
    TournamentNode *next;
};


PlayerMap *
player_map_init(Arena *arena, u64 bucket_count) {
    PlayerMap *player_map = arena_push(arena, sizeof(*player_map));

    player_map->players = arena_push(
        arena, sizeof(*(player_map->players))*bucket_count
    );
    if (player_map->players) {
        player_map->bucket_count = bucket_count;
    }
    else {
        log_error("HashMap allocation failed");
    }

    for (u64 i = 0; i < bucket_count; i++) {
        ((player_map->players)[i]).head = NULL;
        ((player_map->players)[i]).first_free_entry = NULL;
    }

    player_map->tournament_free_entry = NULL;

    return player_map;
}

u64
hash_string(String string) {
    u64 hash = 5381;

    for (u64 i = 0; i < string.size; i++) {
        hash = (
            // hash * 33 + c
            ((hash << 5) + hash) + (u64)string.str[i]
        );
    }
    
    return hash;
}

u64
choose_bucket(u64 hash, u64 bucket_count) {
    return hash % bucket_count;
}

void
player_map_add(Arena *arena, PlayerMap *player_map, String player_name) {
    // add a player

    u64 bucket_num = choose_bucket(
        hash_string(player_name), player_map->bucket_count
    );

    Players *players = player_map->players + bucket_num;
    PlayerNode **player = &(players->head);

    // check that player_name is not already present between players
    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            log_error("%s, is already present", player_name.str);
        }
        else {
            player = &((*player)->next);
        }
    }

    PlayerNode *node = players->first_free_entry;
    if (node) {
        players->first_free_entry = players->first_free_entry->next;
    }
    else {
        node = arena_push(arena, sizeof(*node));
    }

    // put the new player at the start of players list
    node->player_name = player_name;
    node->tournament_head = NULL; 
    node->next = players->head;

    players->head = node;
}

void
player_map_remove(PlayerMap *player_map, String player_name) {
    // remove a player

    u64 bucket_num = choose_bucket(
        hash_string(player_name), player_map->bucket_count
    );

    Players *players = player_map->players + bucket_num;
    PlayerNode **player = &(players->head);

    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            // append player tournaments to the end of the tournaments free list
            TournamentNode **tournament_free_entry = &(player_map->tournament_free_entry);
            while (*tournament_free_entry) {
                tournament_free_entry = &((*tournament_free_entry)->next);
            }
            *tournament_free_entry = (*player)->tournament_head;

            // remove player from players list
            PlayerNode *player_free_entry = players->first_free_entry;
            PlayerNode *player_to_remove = *player;

            *player = (*player)->next;

            players->first_free_entry = player_to_remove;
            player_to_remove->next = player_free_entry;

            return;
        }
        else {
            player = &((*player)->next);
        }
    }
}

void
player_map_register(
    Arena *arena,
    PlayerMap *player_map,
    String player_name,
    String tournament_name
) {
    // register a player to a tournament

    u64 bucket_num = choose_bucket(
        hash_string(player_name), player_map->bucket_count
    );

    Players *players = player_map->players + bucket_num;
    PlayerNode **player = &(players->head);

    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            TournamentNode **tournament = &((*player)->tournament_head);

            // check if player is already registered to that tournament
            while (*tournament) {
                if (string_are_equal((*tournament)->tournament_name, tournament_name)) {
                    log_error(
                        "Player %s already registered to tournament %s",
                        player_name,
                        tournament_name
                    );
                    return;
                }
                else {
                    tournament = &((*tournament)->next);
                }
            }

            *tournament = player_map->tournament_free_entry;
            if (*tournament) {
                player_map->tournament_free_entry = player_map->tournament_free_entry->next;
            }
            else {
                *tournament = arena_push(arena, sizeof(TournamentNode));
            }

            (*tournament)->tournament_name = tournament_name;
            (*tournament)->next = NULL;

            return;
        }
        else {
            player = &((*player)->next);
        }
    }

    assert(0);
}

void
player_map_unregister(
    PlayerMap *player_map,
    String player_name,
    String tournament_name
) {
    // unregister a player from a tournament

    u64 bucket_num = choose_bucket(
        hash_string(player_name), player_map->bucket_count
    );

    Players *players = player_map->players + bucket_num;
    PlayerNode **player = &(players->head);

    while (*player) {
        if (string_are_equal((*player)->player_name, player_name)) {
            TournamentNode **tournament = &((*player)->tournament_head);
            while (*tournament) {
                if (string_are_equal((*tournament)->tournament_name, tournament_name)) {
                    // deregister: remove this element from the linked list and add it to the free list
                    TournamentNode *tournament_to_remove = *tournament;
                    TournamentNode *first_free_entry = player_map->tournament_free_entry;

                    *tournament = (*tournament)->next;

                    player_map->tournament_free_entry = tournament_to_remove;
                    tournament_to_remove->next = first_free_entry;

                    return;
                }
                else {
                    tournament = &((*tournament)->next);
                }
            }
        }
        else {
            player = &((*player)->next);
        }
    }

    assert(0);
}

PlayerNode *
player_map_find(PlayerMap *player_map, String player_name) {
    u64 bucket_num = choose_bucket(
        hash_string(player_name), player_map->bucket_count
    );

    PlayerNode *player = (player_map->players)->head + bucket_num;
    while (player) {
        if (string_are_equal(player->player_name, player_name)) {
            return player;
        }
        else {
            player = player->next;
        }
    }

    log_error("No player found with name %s", player_name.str);
    return NULL;
}

#endif // REGISTRATION_C
