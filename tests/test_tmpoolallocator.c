#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../tmstring.c"
#include "../tmutils.h"
#include "../log.c"
#include "../tmpoolallocator.c"

void
pool_allocator_test_0(void) {
    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    PoolAllocator *allocator = pool_init(&arena, sizeof(String));
    String *pool_str = pool_alloc(&arena, allocator);
    *pool_str = tmstring_alloc_and_init((uint8_t *)"Hello");
    tmstring_realloc_and_reinit(pool_str, (uint8_t *)"World!");

    /* now pool_free must free also the string that was allocated */
    pool_free(allocator, pool_str);
}

int
main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("tmpoolallocator tests started");
    pool_allocator_test_0();
    log_info("tmpoolallocator tests ended");
}
