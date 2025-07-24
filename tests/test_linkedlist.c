#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../linkedlist.c"

void
test_linkedlist(void)
{
    Arena *arena = arena_alloc(KiloByte(1));

    Ctx ctx = {0};
    ctx_init(&ctx);

    NodeState node_state = { .arena = arena, .first_free = NULL };

    SLLStack ll;
    sll_init(&ll);

    sll_push_front(&ll, &node_state, 1);
    sll_pop_front(&ll, &node_state);
}

int main(void)
{
    test_linkedlist();
}
