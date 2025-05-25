#ifndef REGISTRATION_C
#define REGISTRATION_C

#include "utils.c"
#include "names.c"
#include "string.c"

typedef struct TournamentName TournamentName;
struct TournamentName {
    NameChunkList name;
    TournamentName *next;
};

typedef struct TournamentNameList TournamentNameList;
struct TournamentNameList {
    TournamentName *head;
};

typedef struct Player Player;
struct Player {
    NameChunkList name;
    TournamentNameList tournaments;
    Player *next;
};

typedef struct Players Players;
struct Players {
    Arena *arena;
    Player *first_free;
};

typedef struct PlayerMap PlayerMap;
struct PlayerMap {
    u64 bucket_count;
    Player *players;
};

PlayerMap *
player_map_init(Arena *arena, u64 bucket_count) {
    PlayerMap *player_map = arena_push(arena, sizeof(PlayerMap));
    player_map->players = arena_push(arena, sizeof(Player)*bucket_count);
    player_map->bucket_count = bucket_count;
    return player_map;
}

u64
hash_string(String str) {
    u64 hash = 5381;
    for (u64 i = 0; i < str.len; ++i) {
        hash = (
            // hash * 33 + c
            ((hash << 5) + hash) + (u64)((str.str)[i])
        );
    }
    return hash;
}

u64
hash_name(Arena *arena, NameChunkList name) {
    u64 hash = 5381;

    Temp temp = temp_begin(arena);
    String str = name_cat(arena, name);
    for (u64 i = 0; i < str.len; ++i) {
        hash = (
            // hash * 33 + c
            ((hash << 5) + hash) + (u64)((str.str)[i])
        );
    }
    temp_end(temp);
    
    return hash;
}

void
player_create(
    Arena *arena,
    Names *names,
    Players *players,
    PlayerMap *player_map,
    String player_name
) {
    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;
    Player *bucket_head = player_map->players + bucket_num;
    Player **player = &bucket_head;
    NameChunkList name = name_save(names, player_name);
    while (*player) {
        if (name_cmp((*player)->name, name)) {
            log_error("%s, is already present", player_name);
            name_delete(names, name);
            return;
        }
        else {
            player = &((*player)->next);
        }
    }
    // Add player
    *player = players->first_free;
    if (*player) {
        players->first_free = players->first_free->next;
    }
    else {
        *player = arena_push(arena, sizeof(Player)); 
    }
    (*player)->name = name;
    // TODO: add some tournaments
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

TournamentNamesArray
list_tournaments(Arena *arena, PlayerMap *player_map) {
    TournamentNamesArray name_array = {0};
    name_array.names = arena_push(arena, 100*sizeof(String)); // instead of guessing 100 I shold keep the info on how many tournament and players are registered

    for (u64 i = 0; i < player_map->bucket_count; i++) {
        PlayerNode **players = player_map->players + i;

        while (*players) {
            Name *tournament = (*players)->tournament_names_head;
            while (tournament) {
                u64 array_len = name_array.count;
                if (array_len == 0) {
                    name_array.names[0] = tournament->name;
                    name_array.count++;
                }
                else {
                    bool inside = false;
                    for (u64 j = 0; j < array_len; j++) {
                        if (string_are_equal(tournament->name, name_array.names[j])) {
                            inside = true;
                            break;
                        }
                    }
                    if (!inside) {
                        name_array.names[name_array.count] = tournament->name;
                        name_array.count++;
                    }
                }
                tournament = tournament->next;
            }
            players = &((*players)->next);
        }
    }

    return name_array;
}


#endif // REGISTRATION_C
