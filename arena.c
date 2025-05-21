#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>

#include "log.c"
#include "utils.c"

typedef struct
{
    u8 *data;
    u64 pos;
    u64 size;
} Arena;

// TODO: proper align on push and pop
Arena
arena_alloc(u64 size) {
    log_info("Allocating %" PRIu64 " bytes for the arena ...", size);

    Arena arena = {0};

    u8 *data = malloc(size);
    assert(data);

    arena.pos  = 0;
    arena.data = data;
    arena.size = size;

    return arena;
}

void
arena_dealloc(Arena *arena) {
    free(arena->data);
    arena->data = NULL;
}


void *
arena_push(Arena *arena, u64 size)
{
    // TODO: this must become a proper handling
    assert(arena->pos + size <= arena->size);

    void *result = arena->data + arena->pos;
    arena->pos += size;

    memset(result, 0, size);

    return result;
}


void
arena_pop(Arena *arena, u64 size)
{
    assert(arena->pos > arena->size);

    arena->pos -= arena->size;
}


void
arena_clear(Arena *arena)
{
    arena->pos = 0;
}

#endif // ARENA_C
