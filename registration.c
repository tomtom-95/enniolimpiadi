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

// TODO: do I want the trick of head being a dummy value?
void
player_map_add(
    Arena *arena,
    PlayerMap *player_map,
    u8 *player_name,
    PlayerFreeList *player_free_list
) {
    String string_player_name = string_from_cstring(player_name);
    assert(string_player_name.size <= STRING_MAX_LEN);

    u64 bucket_num = hash_string(string_player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    // check that player_name is not already present between players
    while (*players) {
        if (string_are_equal((*players)->player_name, string_player_name)) {
            log_error("%s, is already present", *player_name);
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

    node->player_name = string_player_name;
    node->tournament_names_head = NULL; 
    *players = node;
}

void
player_map_remove(
    PlayerMap *player_map,
    u8 *player_name,
    PlayerFreeList *player_free_list,
    NameFreeList *name_free_list
) {
    String string_player_name = string_from_cstring(player_name);
    assert(string_player_name.size <= STRING_MAX_LEN);

    u64 bucket_num = hash_string(string_player_name) % player_map->bucket_count;

    PlayerNode **players = player_map->players + bucket_num;

    while (*players) {
        if (string_are_equal((*players)->player_name, string_player_name)) {
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
}

// void
// player_map_register(
//     Arena *arena,
//     PlayerMap *player_map,
//     SmallString *player_name,
//     SmallString *tournament_name
// ) {
//     u64 bucket_num = hash_string(player_name) % player_map->bucket_count;
// 
//     Players *players = player_map->players + bucket_num;
//     PlayerNode **player = &(players->head);
// 
//     while (*player) {
//         if (small_string_are_equal((*player)->player_name, player_name)) {
//             TournamentNode **tournament = &((*player)->tournament_head);
// 
//             // check if player is already registered to that tournament
//             while (*tournament) {
//                 if (small_string_are_equal((*tournament)->tournament_name, tournament_name)) {
//                     log_error(
//                         "Player %s already registered to tournament %s",
//                         player_name,
//                         tournament_name
//                     );
//                     return;
//                 }
//                 else {
//                     tournament = &((*tournament)->next);
//                 }
//             }
// 
//             *tournament = player_map->tournament_free_entry;
//             if (*tournament) {
//                 player_map->tournament_free_entry = player_map->tournament_free_entry->next;
//             }
//             else {
//                 *tournament = arena_push(arena, sizeof(TournamentNode));
//             }
// 
//             (*tournament)->tournament_name = tournament_name;
//             (*tournament)->next = NULL;
// 
//             return;
//         }
//         else {
//             player = &((*player)->next);
//         }
//     }
// 
//     assert(0);
// }
// 
// void
// player_map_unregister(
//     PlayerMap *player_map,
//     SmallString *player_name,
//     SmallString *tournament_name
// ) {
//     u64 bucket_num = hash_string(player_name) % player_map->bucket_count;
// 
//     Players *players = player_map->players + bucket_num;
//     PlayerNode **player = &(players->head);
// 
//     while (*player) {
//         if (small_string_are_equal((*player)->player_name, player_name)) {
//             TournamentNode **tournament = &((*player)->tournament_head);
//             while (*tournament) {
//                 if (small_string_are_equal((*tournament)->tournament_name, tournament_name)) {
//                     // deregister: remove this element from the linked list and add it to the free list
//                     TournamentNode *tournament_to_remove = *tournament;
//                     TournamentNode *first_free_entry = player_map->tournament_free_entry;
// 
//                     *tournament = (*tournament)->next;
// 
//                     player_map->tournament_free_entry = tournament_to_remove;
//                     tournament_to_remove->next = first_free_entry;
// 
//                     return;
//                 }
//                 else {
//                     tournament = &((*tournament)->next);
//                 }
//             }
//         }
//         else {
//             player = &((*player)->next);
//         }
//     }
// 
//     assert(0);
// }
// 
// PlayerNode *
// player_map_find(PlayerMap *player_map, SmallString *player_name) {
//     u64 bucket_num = hash_string(player_name) % player_map->bucket_count;
// 
//     PlayerNode *player = (player_map->players + bucket_num)->head;
//     while (player) {
//         if (small_string_are_equal(player->player_name, player_name)) {
//             return player;
//         }
//         else {
//             player = player->next;
//         }
//     }
// 
//     log_error("No player found with name %s", player_name->str);
//     return NULL;
// }

#endif // REGISTRATION_C
