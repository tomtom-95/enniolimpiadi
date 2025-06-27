#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "utils.c"
#include "names.c"
#include "string.c"

typedef struct Player Player;
struct Player {
    Name *player_name;
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
    Name *tournament_name;
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

void player_map_init(Arena *arena, PlayerMap *player_map, u64 bucket_count);
void tournament_map_init(Arena *arena, TournamentMap *tournament_map, u64 bucket_count);

Player *player_alloc(PlayerState *player_state);
Tournament *tournament_alloc(TournamentState *tournament_state);

Player *player_find(PlayerMap *player_map, String str_player_name);
Tournament *tournament_find(TournamentMap *tournament_map, String str_tournament_name);

Player *player_create(PlayerMap *player_map, String str_player_name, PlayerState *player_state, NameState *name_state, NameChunkState *name_chunk_state);
Tournament *tournament_create(TournamentMap *tournament_map, String str_tournament_name, TournamentState *tournament_state, NameState *name_state, NameChunkState *name_chunk_state);

void player_delete(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, PlayerState *player_state, NameState *name_state, NameChunkState *name_chunk_state);
void tournament_delete(PlayerMap *player_map, TournamentMap *tournament_map, String str_tournament_name, TournamentState *tournament_state, NameState *name_state, NameChunkState *name_chunk_state);

void player_enroll(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, String str_tournament_name, PlayerState *player_state, TournamentState *tournament_state, NameState *name_state, NameChunkState *name_chunk_state);
void player_withdraw(PlayerMap *player_map, TournamentMap *tournament_map, String str_player_name, String str_tournament_name, NameState *name_state, NameChunkState *name_chunk_state);

StringList list_tournaments(Arena *arena, TournamentMap *tournament_map);

#endif // REGISTRATION_H
