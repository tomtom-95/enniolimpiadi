#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../string.c"

int main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("../test_tmstring.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("String tests started");

    String a = string_from_cstring((u8 *)"ciao");
    string_print(a);

    String b = string_from_cstring((u8 *)("hello"));
    printf("%s\n", b.str);

    if (string_are_equal(a, b)) {
        log_info("Strings are equal");
    }
    else {
        log_info("Strings are not equal");
    }

    Arena arena = arena_alloc(MegaByte(1));
    String *string = arena_push(&arena, sizeof(*string));

    string_copy(string, &a);
    string_concat(&a, &b);

    String ciaohello = string_from_cstring((u8 *)"ciaohello");

    log_info("String tests finished");

}
