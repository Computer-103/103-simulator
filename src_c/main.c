#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <err.h>
#include <ctype.h>

#include "tape.h"
#include "103.h"

int flag_decimal = 0;

static char short_options[] = "s:d";
static struct option long_options[] = {
    {"start",   required_argument,  0,              's'},
    {"decimal", no_argument,        &flag_decimal,  'd'},
    {0,         0,                  0,               0 },
};

int main(int argc, char ** argv) {
    init_machine();

    while (1) {
        int c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (c == -1) {
            break;
        }

        if (c == 0) {
            // nothing to do
        } else if (c == 's') {
            int start_addr;
            if (sscanf(optarg, "%o", &start_addr) != 1) {
                err(EXIT_FAILURE, "%s", optarg);
            }
            set_pc(start_addr);

        } else {
            fprintf(stderr, "Usage: %s [-s <start_addr>] [-d] <tape_file>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-s <start_addr>] [-d] <tape_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    setup_mem(argv[optind]);
    if (flag_decimal) {
        set_output_numeral(DECIMAL);
    } else {
        set_output_numeral(OCTAL);
    }

    run_machine();

    print_mem();

    exit(EXIT_SUCCESS);
}