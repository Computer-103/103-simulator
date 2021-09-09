#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <ctype.h>

#include "103.h"
#include "tape.h"

num_t * machine_mem;
num_t reg_addr;

num_t reg_a;
num_t reg_b;
num_t reg_c;
num_t reg_d;

num_t reg_pc;

int switch_output_numeral;

void set_pc(int val) {
    reg_pc = val & 0xfff;
}

void set_output_numeral(int val) {
    switch_output_numeral = val;
}

void init_mem() {
    machine_mem = malloc(sizeof(num_t) * 04000);
}

void setup_mem(char * tape_filename) {
    FILE * tape_file = fopen(tape_filename, "rb");
    if (!tape_file) {
        err(EXIT_FAILURE, "%s", tape_filename);
    }

    char ch;
    while ((ch = fgetc(tape_file)) != EOF) {
        if (ch == SPACE) {
            // nothing to do
        } else if (ch == POS) {
            reg_c <<= 3;
            reg_c |= 0x6;
        } else if (ch == NEG) {
            reg_c <<= 3;
            reg_c |= 0x7;
        } else if (ch == NUM0) {
            reg_c <<= 3;
            reg_c |= 0x0;
        } else if (ch == NUM1) {
            reg_c <<= 3;
            reg_c |= 0x1;
        } else if (ch == NUM2) {
            reg_c <<= 3;
            reg_c |= 0x2;
        } else if (ch == NUM3) {
            reg_c <<= 3;
            reg_c |= 0x3;
        } else if (ch == NUM4) {
            reg_c <<= 3;
            reg_c |= 0x4;
        } else if (ch == NUM5) {
            reg_c <<= 3;
            reg_c |= 0x5;
        } else if (ch == NUM6) {
            reg_c <<= 3;
            reg_c |= 0x6;
        } else if (ch == NUM7) {
            reg_c <<= 3;
            reg_c |= 0x7;
        } else if (ch == SET_ADD) {
            reg_addr = reg_c & 0xfff;
        } else if (ch == WRITE) {
            machine_mem[reg_addr] = reg_c;
        }
    }
}

void print_mem() {
    FILE * mem_file = fopen("mem.txt", "w");
    for (int i = 0; i < 2048; i++) {
        fprintf(mem_file, "%04o ", i);
        fprintf(mem_file, "%c ", (i & 0x40000000)? '-' : '+');
        fprintf(mem_file, "%010o\n", machine_mem[i] & 0x3fffffff);
    }
}

void init_machine() {
    init_mem();
}

void run_machine() {

}