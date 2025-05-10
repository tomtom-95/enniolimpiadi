#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../names.c"
#include "../string.c"

int main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("../test_names.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("String tests started");

    Arena arena = arena_alloc(MegaByte(1));
    NameList *name_list = name_list_init(&arena);

    String string0 = string_from_cstring((u8 *)"string0");
    String string1 = string_from_cstring((u8 *)"string1");
    String string2 = string_from_cstring((u8 *)"string2");

    name_list_push_right(&arena, name_list, string0);
    name_list_push_right(&arena, name_list, string1);
    name_list_push_right(&arena, name_list, string2);

    name_list_pop(name_list, string1);

    log_info("String tests finished");
}
