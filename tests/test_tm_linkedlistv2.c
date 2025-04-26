#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"

#include "../tm_utils.c"
#include "../tm_linkedlistv2.c"

void
linked_list_v2_test_0(void) {
    log_info("testing");

    Arena arena = arena_alloc(MegaByte(1));
    if (!arena.data) {
        log_error("test failed");
    }

    u32 data0 = 2000;
    u32 data1 = 3000;
    u32 data2 = 4000;
    u32 data3 = 5000;

    LinkedListV2 ll = linked_list_v2_init(&arena, &(data0));
    ll = linked_list_v2_push_left(&arena, ll, &(data1));
    ll = linked_list_v2_push_right(&arena, ll, &(data2));
    ll = linked_list_v2_pop_right(ll);
    ll = linked_list_v2_push_right(&arena, ll, &(data3));
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
