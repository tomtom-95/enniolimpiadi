#ifndef REGISTRATION_H
#define REGISTRATION_H

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

u64 hash_string(String str);

PlayerMap *player_map_init(Arena *arena, u64 bucket_count);
TournamentMap *tournament_map_init(Arena *arena, u64 bucket_count);

Player *player_create(NameChunkState *name_chunk_state, PlayerState *player_state, PlayerMap *player_map, String str_player_name);
Tournament *tournament_create(NameChunkState *name_chunk_state, TournamentState *tournament_state, TournamentMap *tournament_map, String str_tournament_name);

void player_delete(NameState *name_state, NameChunkState *name_chunk_state, PlayerState *player_state, PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name);
void tournament_delete(NameState *name_state, NameChunkState *name_chunk_state, TournamentState *tournament_state, TournamentMap *tournament_map, PlayerMap *player_map, String str_tournament_name);

Player *player_find(PlayerMap *player_map, String str_player_name);
Tournament *tournament_find(TournamentMap *tournament_map, String str_tournament_name);

void player_enroll(NameState *name_state, NameChunkState *name_chunk_state, PlayerMap *player_map, TournamentMap *tournament_map, TournamentState *tournament_state, String str_player_name, String str_tournament_name);
void player_withdraw(NameState *name_state, NameChunkState *name_chunk_state, PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, String str_tournament_name);

void player_rename(PlayerMap *player_map, TournamentMap *tournament_map, NameChunkState *name_chunk_state, String str_oldname, String str_newname);

StringList list_tournaments(TournamentMap *tournament_map);

#endif // REGISTRATION_H
