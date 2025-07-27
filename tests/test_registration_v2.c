#include "../utils.c"
#include "../arena.c"
#include "../names.c"
#include "../namelist.c"

#include "../registration_v2.c"

int main(void)
{
    Arena *arena = arena_alloc(MegaByte(10));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameState name_state = name_state_init(arena);

    TournamentMap tournament_map = tournament_map_init(arena, 16);

    Name pingpong    = name_init(string_from_lit_comp("Ping Pong"), &name_state.name_chunk_state);
    Name machiavelli = name_init(string_from_lit_comp("Machiavelli"), &name_state.name_chunk_state);
    Name riccardo    = name_init(string_from_lit_comp("Riccardo"), &name_state.name_chunk_state);

    tournament_add(&tournament_map, pingpong, &name_state);
    tournament_add(&tournament_map, machiavelli, &name_state);
    tournament_player_enroll(&tournament_map, pingpong, riccardo, &name_state);

    Temp temp = scratch_get(0, 0);

    NameList *tournament_list = (
        list_all_tournaments(&tournament_map, &name_state, temp.arena)
    );
    NameList *players_list = (
        list_all_players(&tournament_map, &name_state, temp.arena)
    );

    scratch_release(temp);
}
