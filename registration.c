#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "utils.c"
#include "names.c"
#include "string.c"

typedef struct PlayerMap PlayerMap;
typedef struct PlayerFreeList PlayerFreeList;
typedef struct PlayerNode PlayerNode;

typedef struct TournamentMap TournamentMap;
typedef struct TournamentFreeList TournamentFreeList;
typedef struct TournamentNode TournamentNode;

typedef struct NameFreeList NameFreeList;

struct PlayerFreeList {
    PlayerNode *first_free_entry;
};

struct TournamentFreeList {
    TournamentNode *first_free_entry;
};

struct NameFreeList {
    Name *first_free_entry;
};

struct PlayerMap {
    u64 bucket_count;
    PlayerNode **players;
};

struct PlayerNode {
    String player_name;
    Name *tournament_names_head;
    PlayerNode *next;
};

struct TournamentNode {
    String *tournament_name;
    TournamentNode *next;
};


PlayerMap *
player_map_init(Arena *arena, u64 bucket_count) {
    PlayerMap *player_map = arena_push(arena, sizeof(*player_map));
    u64 players_array_size = sizeof(*(player_map->players))*bucket_count;

    player_map->players = arena_push(arena, players_array_size);
    memset(player_map->players, 0, players_array_size);

    player_map->bucket_count = bucket_count;

    return player_map;
}

u64
hash_string(String string) {
    u64 hash = 5381;

    for (u8 i = 0; i < string.size; i++) {
        hash = (
            // hash * 33 + c
            ((hash << 5) + hash) + (u64)((string.str)[i])
        );
    }
    
    return hash;
}

void
player_create(
    Arena *arena,
    PlayerMap *player_map,
    String player_name,
    PlayerFreeList *player_free_list
) {
    assert(player_name.size <= STRING_MAX_LEN);

    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    // check that player_name is not already present between players
    while (*players) {
        if (string_are_equal((*players)->player_name, player_name)) {
            log_error("%s, is already present", player_name);
            return;
        }
        else {
            players = &((*players)->next);
        }
    }

    PlayerNode *node = player_free_list->first_free_entry;
    if (node) {
        player_free_list->first_free_entry = (
            player_free_list->first_free_entry->next
        );
    }
    else {
        node = arena_push(arena, sizeof(*node));
    }

    node->player_name = player_name;
    node->next = NULL;
    node->tournament_names_head = NULL; 

    *players = node;
}

void
player_destroy(
    PlayerMap *player_map,
    String player_name,
    PlayerFreeList *player_free_list,
    NameFreeList *name_free_list
) {
    assert(player_name.size <= STRING_MAX_LEN);

    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    while (*players) {
        if (string_are_equal((*players)->player_name, player_name)) {
            Name **tournament_name_node = &((*players)->tournament_names_head);
            while (*tournament_name_node) {
                tournament_name_node = &((*tournament_name_node)->next);
            }
            *tournament_name_node = name_free_list->first_free_entry;
            name_free_list->first_free_entry = (*players)->tournament_names_head;

            PlayerNode *player_to_remove = *players;

            *players = (*players)->next;

            player_to_remove->next = player_free_list->first_free_entry;
            player_free_list->first_free_entry = player_to_remove;

            return;
        }
        else {
            players = &((*players)->next);
        }
    }
    assert(0);
}

void
player_enroll(
    Arena *arena,
    PlayerMap *player_map,
    String player_name,
    String tournament_name,
    NameFreeList *name_free_list
) {
    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    while (*players) {
        if (string_are_equal((*players)->player_name, player_name)) {
            Name **tournament = &((*players)->tournament_names_head);

            // check if player is already registered to that tournament
            while (*tournament) {
                if (string_are_equal((*tournament)->name, tournament_name)) {
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

            *tournament = name_free_list->first_free_entry;
            if (*tournament) {
                name_free_list->first_free_entry = name_free_list->first_free_entry->next;
            }
            else {
                *tournament = arena_push(arena, sizeof(**tournament));
            }
            (*tournament)->name = tournament_name;
            (*tournament)->next = NULL;

            return;
        }
        else {
            players = &((*players)->next);
        }
    }
    // TODO: part_2 -> add a player name to the tournament map
}

void
player_withdraw(
    PlayerMap *player_map,
    String player_name,
    String tournament_name,
    NameFreeList *name_free_list
) {
    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    while (*players) {
        if (string_are_equal((*players)->player_name, player_name)) {
            Name **tournament = &((*players)->tournament_names_head);

            while (*tournament) {
                if (string_are_equal((*tournament)->name, tournament_name)) {
                    Name *tournament_to_remove = *tournament;

                    *tournament = (*tournament)->next;
                    tournament_to_remove->next = name_free_list->first_free_entry;
                    name_free_list->first_free_entry = tournament_to_remove;

                    // TODO: part_2 -> remove player from the tournament map

                    return;
                }
                else {
                    tournament = &((*tournament)->next);
                }
            }
        }
        else {
            players = &((*players)->next);
        }
    }
    assert(0);
}

PlayerNode *
player_find(PlayerMap *player_map, String player_name) {
    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    while (*players) {
        if (string_are_equal((*players)->player_name, player_name)) {
            return *players;
        }
        else {
            players = &((*players)->next);
        }
    }
    assert(0);
}


#endif // REGISTRATION_C
