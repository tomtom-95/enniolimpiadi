#ifndef REGISTRATION_V2_C
#define REGISTRATION_V2_C

#include "utils.c"
#include "names.c"
#include "string.c"

// typedef struct TournamentName TournamentName;
// struct TournamentName {
//     Name name;
//     TournamentName *next;
// };
// 
// typedef struct TournamentNameList TournamentNameList;
// struct TournamentNameList {
//     NameChunkList *head;
// };


typedef struct Player Player;
struct Player {
    Name player_name;
    NameList tournament_names;
    Player *next;
};

typedef struct PlayerState PlayerState;
struct PlayerState {
    Arena *arena;
    Player *first_free;
};

typedef struct PlayerMap PlayerMap;
struct PlayerMap {
    u64 bucket_count;
    Player **players;
};

PlayerMap *
player_map_init(Arena *arena, u64 bucket_count) {
    PlayerMap *player_map = arena_push(arena, sizeof(PlayerMap));
    player_map->players = arena_push(arena, sizeof(Player *)*bucket_count);
    player_map->bucket_count = bucket_count;
    return player_map;
}

u64
hash_string(String str) {
    u64 hash = 5381;
    for (u64 i = 0; i < str.len; ++i) {
        hash = (
            ((hash << 5) + hash) + (u64)((str.str)[i])
        );
    }
    return hash;
}

// u64
// hash_name(Arena *arena, NameChunkList name) {
//     u64 hash = 5381;
// 
//     Temp temp = temp_begin(arena);
//     String str = name_cat(arena, name);
//     for (u64 i = 0; i < str.len; ++i) {
//         hash = (
//             // hash * 33 + c
//             ((hash << 5) + hash) + (u64)((str.str)[i])
//         );
//     }
//     temp_end(temp);
//     
//     return hash;
// }

void
player_create(
    NameChunkState *name_chunk_state,
    PlayerState *player_state,
    PlayerMap *player_map,
    String str_player_name
) {
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);
    Name player_name = name_save(name_chunk_state, str_player_name);
    while (*player) {
        if (name_cmp((*player)->player_name, player_name)) {
            log_error("%s, is already present", player_name);
            name_delete(name_chunk_state, player_name);
            return;
        }
        else {
            player = &((*player)->next);
        }
    }
    *player = player_state->first_free;
    if (*player) {
        player_state->first_free = player_state->first_free->next;
    }
    else {
        *player = arena_push(player_state->arena, sizeof(Player)); 
    }
    (*player)->player_name = player_name;
    (*player)->tournament_names.head = NULL;
    (*player)->next = NULL;
}

void
player_delete(
    NameChunkState *name_chunk_state,
    PlayerState *player_state,
    PlayerMap *player_map,
    String str_player_name
) {
    Name player_name = name_save(name_chunk_state, str_player_name);
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;

    Player **player = &(player_map->players[bucket_num]);
    while (*player) {
        if (name_cmp((*player)->player_name, player_name)) {
            Player *to_remove = *player;
            *player = (*player)->next;

            to_remove->next = player_state->first_free;
            player_state->first_free = to_remove;
            name_delete(name_chunk_state, to_remove->player_name);
            // TODO: dealloc tournaments list
            break;
        }
        else {
            player = &((*player)->next);
        }
    }

    name_delete(name_chunk_state, player_name);
}

// Player *
// player_find(
//     Arena *arena,
//     PlayerMap player_map,
//     Name player_name
// ) {
//     Temp temp = temp_begin(arena);
//     String str = name_cat(arena, player_name);
//     u64 bucket_num = hash_string(str) % player_map.bucket_count;
//     temp_end(temp);
// 
//     Player *player = player_map.players[bucket_num];
//     while (player) {
//         if (name_cmp(player_name, player->name)) {
//             return player;
//         }
//         else {
//             player = player->next;
//         }
//     }
//     return NULL;
// }
// 
// void
// player_rename(
//     Names *names,
//     Players *players,
//     Player *player,
//     String newname
// ) {
//     name_delete(names, player->name);
//     NameChunkList res = name_save(names, newname);
//     player->name = res;
// }


// void
// player_enroll(
//     Names *names,
//     PlayerMap *player_map,
//     String _player_name,
//     String _tournament_name
// ) {
//     NameChunkList player_name = name_save(names, _player_name);
//     u64 bucket_num = hash_string(_player_name) % player_map->bucket_count;
// 
//     Player **player = &(player_map->players[bucket_num]);
//     while (*player) {
//         if (name_cmp((*player)->name, player_name)) {
//             TournamentNameList tournaments = (*player)->tournaments;
//             TournamentName **tournament_name = &(tournaments.head);
//             while (*tournament_name) {
//                 tournament_name = &((*tournament_name)->next);
//             }
// 
//         }
//         else {
//             player = &((*player)->next);
//         }
//     } 
// 
//     name_delete(names, player_name);
// }

#endif // REGISTRATION_V2_C
