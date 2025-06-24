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

typedef struct Tournament Tournament;
struct Tournament {
    Name tournament_name;
    NameList player_names;
    Tournament *next;
};

typedef struct TournamentState TournamentState;
struct TournamentState {
    Arena *arena;
    Tournament *first_free;
};

typedef struct TournamentMap TournamentMap;
struct TournamentMap {
    u64 bucket_count;
    Tournament **tournaments;
};

PlayerMap *
player_map_init(Arena *arena, u64 bucket_count);

TournamentMap *
tournament_map_init(Arena *arena, u64 bucket_count);

u64
hash_string(String str);

Player *
player_create(
    NameChunkState *name_chunk_state, PlayerState *player_state,
    PlayerMap *player_map, String str_player_name
);

Tournament *
tournament_create(
    NameChunkState *name_chunk_state, TournamentState *tournament_state,
    TournamentMap *tournament_map, String str_tournament_name
);

void
player_delete(
    Arena *arena, NameState *name_state, NameChunkState *name_chunk_state, PlayerState *player_state,
    PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name
);

void
tournament_delete(
    Arena *arena, NameState *name_state, NameChunkState *name_chunk_state, TournamentState *tournament_state,
    TournamentMap *tournament_map, PlayerMap *player_map, String str_tournament_name
);

void
player_enroll(
    Arena *arena,
    NameState *name_state, NameChunkState *name_chunk_state,
    PlayerMap *player_map, TournamentMap *tournament_map, TournamentState *tournament_state,
    String str_player_name, String str_tournament_name
);

void
player_withdraw(
    Arena *arena, NameState *name_state, NameChunkState *name_chunk_state,
    PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, String str_tournament_name
);

Player *
player_find(Arena *arena, PlayerMap *player_map, NameChunkState *name_chunk_state, String str_player_name);


Tournament *
tournament_find(Arena *arena, TournamentMap *tournament_map, String str_tournament_name);

void
player_rename(
    Arena *arena,
    PlayerMap *player_map, TournamentMap *tournament_map, NameChunkState *name_chunk_state,
    String str_oldname, String str_newname
);

StringList
list_tournaments(Arena *arena, TournamentMap *tournament_map);

///////////////////////////////////////////////////////////////
// Implementation

PlayerMap *
player_map_init(Arena *arena, u64 bucket_count) {
    PlayerMap *player_map = arena_push(arena, sizeof(PlayerMap));
    player_map->players = arena_push(arena, sizeof(Player *) * bucket_count);
    player_map->bucket_count = bucket_count;
    return player_map;
}

TournamentMap *
tournament_map_init(Arena *arena, u64 bucket_count) {
    TournamentMap *tournament_map = arena_push(arena, sizeof(TournamentMap));
    tournament_map->tournaments = arena_push(arena, sizeof(Tournament *) * bucket_count);
    tournament_map->bucket_count = bucket_count;
    return tournament_map;
}

u64
hash_string(String str) {
    u64 hash = 5381;
    for (u64 i = 0; i < str.len; ++i) {
        hash = ((hash << 5) + hash) + (u64)((str.str)[i]);
    }
    return hash;
}

Player *
player_create(
    NameChunkState *name_chunk_state, PlayerState *player_state,
    PlayerMap *player_map, String str_player_name
) {
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    Player **player = &(player_map->players[bucket_num]);

    while (*player) {
        player = &((*player)->next);
    }

    *player = player_state->first_free;
    if (*player) {
        player_state->first_free = player_state->first_free->next;
    }
    else {
        *player = arena_push(player_state->arena, sizeof(Player)); 
    }

    Name player_name = push_name_from_string(name_chunk_state, str_player_name);
    (*player)->player_name = player_name;
    (*player)->tournament_names.head = NULL;
    (*player)->next = NULL;

    return *player;
}

Tournament *
tournament_create(
    NameChunkState *name_chunk_state, TournamentState *tournament_state,
    TournamentMap *tournament_map, String str_tournament_name
) {
    u64 bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;
    Tournament **tournament = &(tournament_map->tournaments[bucket_num]);

    while (*tournament) {
        tournament = &((*tournament)->next);
    }

    *tournament = tournament_state->first_free;
    if (*tournament) {
        tournament_state->first_free = tournament_state->first_free->next;
    }
    else {
        *tournament = arena_push(tournament_state->arena, sizeof(Tournament)); 
    }

    Name tournament_name = push_name_from_string(name_chunk_state, str_tournament_name);
    (*tournament)->tournament_name = tournament_name;
    (*tournament)->player_names.head = NULL;
    (*tournament)->next = NULL;

    return *tournament;
}

void
player_delete(
    Arena *arena,
    NameState *name_state, NameChunkState *name_chunk_state, PlayerState *player_state,
    PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name
) {
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;

    Player **player = &(player_map->players[bucket_num]);
    Temp temp = temp_begin(arena);
    while (*player) {
        String str = push_string_from_name(temp.arena, (*player)->player_name);
        if (string_cmp(str, str_player_name)) {
            Player *player_to_remove = *player;

            // Remove the player from all the tournaments it was enrolled
            Name *tournament_name = player_to_remove->tournament_names.head;
            while (tournament_name) {
                String str_tournament_name = push_string_from_name(temp.arena, *tournament_name);
                Tournament *tournament = tournament_find(arena, tournament_map, str_tournament_name);
                namelist_pop(name_state, name_chunk_state, &tournament->player_names, str_player_name);
                tournament_name = tournament_name->next;
            }

            namelist_delete(name_state, name_chunk_state, &player_to_remove->tournament_names);

            *player = (*player)->next;
            player_to_remove->next = player_state->first_free;
            player_state->first_free = player_to_remove;

            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    temp_end(temp);
}

void
tournament_delete(Arena *arena, NameState *name_state, NameChunkState *name_chunk_state, TournamentState *tournament_state,
    TournamentMap *tournament_map, PlayerMap *player_map, String str_tournament_name)
{
    u64 bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;

    Tournament **tournament = &(tournament_map->tournaments[bucket_num]);
    Temp temp = temp_begin(arena);
    while (*tournament) {
        String str = push_string_from_name(temp.arena, (*tournament)->tournament_name);
        if (string_cmp(str, str_tournament_name)) {
            Tournament *tournament_to_remove = *tournament;

            // Remove the tournament from all the player that enrolled to it
            Name *player_name = tournament_to_remove->player_names.head;
            while (player_name) {
                String str_player_name = push_string_from_name(temp.arena, *player_name);
                Player *player = player_find(temp.arena, player_map, name_chunk_state, str_player_name);
                namelist_pop(name_state, name_chunk_state, &player->tournament_names, str_tournament_name);
                player_name = player_name->next;
            }

            namelist_delete(name_state, name_chunk_state, &tournament_to_remove->player_names);

            *tournament = (*tournament)->next;
            tournament_to_remove->next = tournament_state->first_free;
            tournament_state->first_free = tournament_to_remove;

            break;
        }
        else {
            tournament = &((*tournament)->next);
        }
    }
    temp_end(temp);
}

void
player_enroll(
    Arena *arena,
    NameState *name_state, NameChunkState *name_chunk_state,
    PlayerMap *player_map, TournamentMap *tournament_map, TournamentState *tournament_state,
    String str_player_name, String str_tournament_name
) {
    u64 player_bucket_num = hash_string(str_player_name) % player_map->bucket_count;
    u64 tournament_bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;

    Player *player = player_find(arena, player_map, name_chunk_state, str_player_name);
    assert(player);
    namelist_append_right(name_state, name_chunk_state, &player->tournament_names, str_tournament_name);

    Tournament *tournament = tournament_find(arena, tournament_map, str_tournament_name);
    if (!tournament) {
        tournament = tournament_create(name_chunk_state, tournament_state, tournament_map, str_tournament_name);
    }
    namelist_append_right(name_state, name_chunk_state, &tournament->player_names, str_player_name);
}

void
player_withdraw(
    Arena *arena, NameState *name_state, NameChunkState *name_chunk_state,
    PlayerMap *player_map, TournamentMap *tournament_map,
    String str_player_name, String str_tournament_name
) {
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;

    Temp temp = temp_begin(arena);
    Player *player = player_find(arena, player_map, name_chunk_state, str_player_name);
    assert(string_cmp(push_string_from_name(temp.arena, player->player_name), str_player_name));
    namelist_pop(name_state, name_chunk_state, &player->tournament_names, str_tournament_name);

    Tournament *tournament = tournament_find(arena, tournament_map, str_tournament_name);
    assert(string_cmp(push_string_from_name(temp.arena, tournament->tournament_name), str_tournament_name));
    namelist_pop(name_state, name_chunk_state, &tournament->player_names, str_player_name);
    temp_end(temp);
}

Player *
player_find(Arena *arena, PlayerMap *player_map, NameChunkState *name_chunk_state, String str_player_name) {
    u64 bucket_num = hash_string(str_player_name) % player_map->bucket_count;

    Player **player = &(player_map->players[bucket_num]);
    Temp temp = temp_begin(arena);
    while (*player) {
        if (string_cmp(str_player_name, push_string_from_name(temp.arena, (*player)->player_name))) {
            break;
        }
        else {
            player = &((*player)->next);
        }
    }
    temp_end(temp);
    return *player;
}

Tournament *
tournament_find(Arena *arena, TournamentMap *tournament_map, String str_tournament_name) {
    u64 bucket_num = hash_string(str_tournament_name) % tournament_map->bucket_count;

    Tournament *tournament = tournament_map->tournaments[bucket_num];
    Temp temp = temp_begin(arena);
    while (tournament) {
        if (string_cmp(str_tournament_name, push_string_from_name(temp.arena, tournament->tournament_name))) {
            break;
        }
        else {
            tournament = tournament->next;
        }
    }
    temp_end(temp);

    return tournament;
}

void
player_rename(
    Arena *arena, PlayerMap *player_map, TournamentMap *tournament_map,
    NameChunkState *name_chunk_state, String str_oldname, String str_newname
) {
    Name newname = push_name_from_string(name_chunk_state, str_newname);

    Player *player = player_find(arena, player_map, name_chunk_state, str_oldname);
    assert(player);

    name_delete(name_chunk_state, &(player->player_name));
    player->player_name = newname;

    Temp temp = temp_begin(arena);
    Name *tournament_name = player->tournament_names.head;
    while (tournament_name) {
        String str_tournament_name = push_string_from_name(temp.arena, *tournament_name);
        Tournament *tournament = tournament_find(arena, tournament_map, str_tournament_name);

        Name *player_name = tournament->player_names.head;
        while (!string_cmp(str_oldname, push_string_from_name(temp.arena, *player_name))) {
            player_name = player_name->next;
        }
        *player_name = newname;
        tournament_name = tournament_name->next;
    }
    temp_end(temp);
}

StringList
list_tournaments(Arena *arena, TournamentMap *tournament_map) {
    StringList string_list = {0};
    StringNode **node = &string_list.head;
    for (u64 idx = 0; idx < tournament_map->bucket_count; ++idx) {
        Tournament *tournament  = tournament_map->tournaments[idx]; 
        while (tournament) {
            String str_tournament_name = push_string_from_name(arena, tournament->tournament_name);
            string_list_push(arena, &string_list, str_tournament_name);
            tournament = tournament->next;
        }
    }
    return string_list;
}

#endif // REGISTRATION_V2_C
