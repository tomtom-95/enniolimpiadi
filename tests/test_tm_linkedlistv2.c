#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../tm_utils.c"
#include "../tm_linkedlistv2.c"

void
linked_list_v2_test_0(void) {
    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    LinkedListV2 ll = linked_list_v2_init(&arena, 2000);
    ll = linked_list_v2_push_left(&arena, ll, 3000);
    ll = linked_list_v2_push_right(&arena, ll, 4000);
    ll = linked_list_v2_pop_right(ll);
    ll = linked_list_v2_push_right(&arena, ll, 5000);
}

int
main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/logs/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("tmlinkedlist tests started");
    linked_list_v2_test_0();
    log_info("tmlinkedlist tests ended");
}
