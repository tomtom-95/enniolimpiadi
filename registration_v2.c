#ifndef REGISTRATION_V2_C
#define REGISTRATION_V2_C

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
    Names *names,
    Players *players,
    PlayerMap *player_map,
    String player_name
) {
    u64 bucket_num = hash_string(player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);
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
        *player = arena_push(players->arena, sizeof(Player)); 
    }
    (*player)->name = name;
    // TODO: add some tournaments
}

#endif // REGISTRATION_V2_C
