#include <stdlib.h>
#include <stdio.h>

#include "../utils.c"
#include "../arena.c"

void *
FunctionA(Arena *arena) {
  Temp scratch = scratch_get(&arena, 1);
  void *result = arena_push(arena, 1024);
  arena_push(scratch.arena, 2048);
  scratch_release(scratch);
  return result;
}

void
FunctionB(void) {
  Temp scratch = scratch_get(0, 0);
  void *result = FunctionA(scratch.arena);
  scratch_release(scratch);
}

int main(void) {
  Ctx ctx;
  ctx_init(&ctx);
  FunctionB();
}
