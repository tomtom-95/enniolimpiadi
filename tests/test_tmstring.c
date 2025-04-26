#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../log.c"
#include "../tmstring.c"

int main(void) {
    log_set_level(LOG_DEBUG);

    FILE *logfile = fopen("tests/test.log", "w");
    if (!logfile) {
        printf("Failed to open file for logging");
        return 1;
    }

    // Log all levels to file
    log_add_fp(logfile, LOG_TRACE);

    log_info("String tests started");

    String a = str_lit("ciao");
    String b = str_lit("ciao");
    if (tmstring_are_equal(a, b)) {
        log_info("test succeded!");
    }
    else {
        log_error("test failed");
    }

    a = str_lit("ciao");
    b = str_lit("hello");
    if (!tmstring_are_equal(a, b)) {
        log_info("test succeded!");
    }
    else {
        log_error("test_failed");
    }

    log_info("String tests finished");
}
