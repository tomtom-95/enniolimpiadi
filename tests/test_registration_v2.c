#include "../registration_v2.c"

int main(void)
{
    Arena *arena = arena_alloc(MegaByte(10));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NameChunkState name_chunk_state;
    name_chunk_state_init(arena, &name_chunk_state);

    NameState name_state;
    name_state_init(arena, &name_state);

    TournamentMap tournament_map = tournament_map_init(arena, 16);

    Name pingpong = name_from_string(string_from_lit_comp("Ping Pong"), &name_chunk_state);
    Name machiavelli = name_from_string(string_from_lit_comp("Machiavelli"), &name_chunk_state);
    Name riccardo = name_from_string(string_from_lit_comp("Riccardo"), &name_chunk_state);

    tournament_add(&tournament_map, pingpong, &name_chunk_state);
    tournament_add(&tournament_map, machiavelli, &name_chunk_state);
    tournament_player_enroll(&tournament_map, pingpong, riccardo, &name_state, &name_chunk_state);
    tournament_player_withdraw(&tournament_map, pingpong, riccardo, &name_state, &name_chunk_state);

    tournament_del(&tournament_map, pingpong, &name_state, &name_chunk_state);
}