#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "log.c"
#include "utils.c"

typedef struct Arena Arena;
struct Arena {
    u8 *data;
    u64 pos;
    u64 size;
};

typedef struct Temp Temp;
struct Temp {
    Arena *arena;
    u64 pos;
};

// TODO: proper align on push and pop
Arena
arena_alloc(u64 size) {
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
arena_push(Arena *arena, u64 size) {
    // TODO: this must become a proper handling
    assert(arena->pos + size <= arena->size);
    void *result = arena->data + arena->pos;
    arena->pos += size;
    memset(result, 0, size);
    return result;
}

void
arena_pop(Arena *arena, u64 size) {
    assert(arena->pos > arena->size);
    arena->pos -= arena->size;
}

void
arena_clear(Arena *arena) {
    arena->pos = 0;
}

Temp
temp_begin(Arena *arena) {
    return (Temp){.arena = arena, .pos = arena->pos};
}

void
temp_end(Temp temp) {
    temp.arena->pos = temp.pos;
}

#endif // ARENA_C
