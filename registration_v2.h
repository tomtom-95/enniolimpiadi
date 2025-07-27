#ifndef REGISTRATION_V2_H
#define REGISTRATION_V2_H

#include "namelist.h"

typedef struct Tournament Tournament;
struct Tournament {
    Name name;
    NameList players_enrolled;
    u64 next;
};

typedef struct TournamentMap TournamentMap;
struct TournamentMap {
    u64 *index_array;
    Tournament *tournaments;
    u64 offset;
    u64 first_free;
    u64 bucket_count;
};

// TODO: hash_name() ?
u64           hash_string(String str);
TournamentMap tournament_map_init(Arena *arena, u64 bucket_count);
u64           tournament_find(TournamentMap *map, Name tournament_name);
void          tournament_add(TournamentMap *map, Name tournament_name, NameState *name_state);
void          tournament_del(TournamentMap *map, Name tournament_name, NameState *name_state);
void          tournament_player_enroll(TournamentMap *map, Name tournament_name, Name player_name, NameState *name_state);
void          tournament_player_withdraw(TournamentMap *map, Name tournament_name, Name player_name, NameState *name_state);

#endif // REGISTRATION_V2_H
