#ifndef REGISTRATION_V2_C
#define REGISTRATION_V2_C

#include "utils.c"
#include "names.c"
#include "string.c"

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
            log_error("%s, is already present", str_player_name);
            name_delete(name_chunk_state, &player_name);
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
    NameState *name_state,
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
            name_delete(name_chunk_state, &(to_remove->player_name));
            namelist_delete(name_state, name_chunk_state, &(to_remove->tournament_names));
            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    name_delete(name_chunk_state, &player_name);
}

void
player_enroll(
    NameState *name_state,
    NameChunkState *name_chunk_state,
    PlayerMap *player_map,
    String str_player_name,
    String str_tournament_name
) {
    Name player_name = name_save(name_chunk_state, str_player_name);
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);
    while (*player) {
        if (name_cmp((*player)->player_name, player_name)) {
            namelist_append_right(
                name_state,
                name_chunk_state,
                &((*player)->tournament_names),
                str_tournament_name
            );
            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    name_delete(name_chunk_state, &player_name);
}

void
player_withdraw(
    NameState *name_state,
    NameChunkState *name_chunk_state,
    PlayerMap *player_map,
    String str_player_name,
    String str_tournament_name
) {
    Name player_name = name_save(name_chunk_state, str_player_name);
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);
    while (*player) {
        if (name_cmp((*player)->player_name, player_name)) {
            namelist_pop(
                name_state,
                name_chunk_state,
                &((*player)->tournament_names),
                str_tournament_name
            );
            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    name_delete(name_chunk_state, &player_name);
}

Player *
player_find(
    PlayerMap *player_map,
    NameChunkState *name_chunk_state,
    String str_player_name
) {
    Name player_name = name_save(name_chunk_state, str_player_name);
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;

    Player **player = &(player_map->players[bucket_num]);
    while (*player) {
        if (name_cmp(player_name, (*player)->player_name)) {
            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    name_delete(name_chunk_state, &player_name);
    return *player;
}

void
player_rename(
    PlayerMap *player_map,
    NameChunkState *name_chunk_state,
    String str_oldname,
    String str_newname
) {
    Player *player = player_find(player_map, name_chunk_state, str_oldname);
    assert(player);
    name_delete(name_chunk_state, &(player->player_name));
    Name newname = name_save(name_chunk_state, str_newname);
    player->player_name = newname;
}

StringList
list_tournaments(
    Arena *arena,
    PlayerMap *player_map
) {
    StringList string_list = {0};
    StringNode **node = &(string_list.head);
    for (u64 bucket = 0; bucket < player_map->bucket_count; ++bucket) {
        Player **player = player_map->players + bucket;
        while (*player) {
            Name *tournament = ((*player)->tournament_names).head;
            while (tournament) {
                String str = name_to_string(arena, *tournament);
                if (!string_list_find(&string_list, str)) {
                    // string_list_push(arena, &string_list, str);
                    *node = arena_push(arena, sizeof(StringNode));
                    (*node)->str = str;
                    ++(string_list.len);
                    node = &((*node)->next);
                }
                tournament = tournament->next;
            }
            player = &((*player)->next);
        }
    }
    return string_list;
}


#endif // REGISTRATION_V2_C
