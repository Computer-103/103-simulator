#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <ctype.h>

#include "103.h"
#include "tape.h"

FILE * typewriter;

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
    init_output();
}

void run_machine() {
    num_t op, c1, c2;
    num_t op1, op2;
    while (1) {
        // 1
        reg_addr = reg_pc;
        
        // 2
        reg_c = machine_mem[reg_addr];
        op = (reg_c >> 24) & 0x3f;
        op1 = op >> 3;
        op2 = op & 0x7;
        c1 = (reg_c >> 12) & 0xfff;
        c2 = (reg_c) & 0xfff;
        
        if (op2 == OP_ADD || op2 == OP_SUB || op2 == OP_DIV || op2 == OP_MUL || op2 == OP_AND) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_a = reg_c;

            reg_addr = c2;
            if (op1 == 0 || op1 == 1 || op1 == 4 || op1 == 5) {
                reg_c = machine_mem[reg_addr];
                reg_b = reg_c;
            }

            int sign_a, sign_b;
            if (op == 5 || op == 7) {
                sign_a = 1;
                sign_b = 1;
            } else {
                sign_a = (reg_a & 0x40000000) ? -1 : 1;
                sign_b = (reg_b & 0x40000000) ? -1 : 1;
            }

            if (op2 == OP_ADD) {
                
            } else if (op2 == OP_SUB) {

            } else if (op2 == OP_DIV) {

            } else if (op2 == OP_MUL) {

            } else if (op2 == OP_AND) {

            }

            reg_b = reg_c;

            if (op2 == 0 || op2 == 2 || op == 4 || op == 6) {
                machine_mem[reg_addr] = reg_c;
            }
            if (op == 4 || op == 6) {
                output_reg_c();
            }

        // } else if (op == 007 || op == 027) {

        // } else if (op == 005 || op == 015) {

        // } else if (op == 045 || op == 055) {

        // } else if (op == 024) {

        // } else if (op == 064) {

        // } else if (op == 074) {

        // } else if (op == 034) {

        // } else if (op == 004 || op == 014 || op == 044) {

        // } else if (op == 054) {
            
        // } else if (op == 037) {

        // } else if (op == 057) {

        // } else if (op == 077) {

        } else {
            exit(EXIT_FAILURE);
        }
    }
}

void init_output() {
    typewriter = fopen("103-output.txt", "w");
}

void output_reg_c() {
    fprintf(typewriter, "%c ", (reg_c & 0x40000000) ? '-' : '+');
    if (switch_output_numeral == DECIMAL) {
        for (int i = 26; i >= 0; i -= 4) {
            fprintf(typewriter, "%d", (reg_c >> i) & 0xF);
        }
    } else if (switch_output_numeral == OCTAL) {
        for (int i = 27; i >= 0; i -= 3) {
            fprintf(typewriter, "%d", (reg_c >> i) & 0x7);
        }
    }
    fprintf(typewriter, "\n");
}

num_t do_add(int sign_a, int num_a, int sign_b, int num_b) {
    int res = (sign_b * num_b) + (sign_a * num_a);
    num_t ans;
    if (res > 0x3fffffff || res < -0x3fffffff) {
        fprintf(stderr, "overflow\n");
        exit(EXIT_FAILURE);
    }
    if (res > 0) {
        ans = res & 0x3fffffff;
    } else if (res < 0) {
        ans = ((-res) & 0x3fffffff) | 0x40000000;
    } else {
        ans = (sign_b < 0) << 30;
    }
    return ans;
}

num_t do_sub(int sign_a, int num_a, int sign_b, int num_b) {
    int res = (sign_b * num_b) - (sign_a * num_a);
    num_t ans;
    if (res > 0x3fffffff || res < -0x3fffffff) {
        fprintf(stderr, "overflow\n");
        exit(EXIT_FAILURE);
    }
    if (res > 0) {
        ans = res & 0x3fffffff;
    } else if (res < 0) {
        ans = ((-res) & 0x3fffffff) | 0x40000000;
    } else {
        ans = (sign_b < 0) << 30;
    }
    return ans;
}

num_t do_div(int sign_a, long num_a, int sign_b, long num_b) {
    long res = ((num_b << 30) / num_a);
    num_t ans;
    if (res > 0x3fffffff) {
        fprintf(stderr, "overflow\n");
        exit(EXIT_FAILURE);
    }
    if (sign_a * sign_b > 0) {
        ans = res & 0x3fffffff;
    } else if (sign_a * sign_b < 0) {
        ans = (res & 0x3fffffff) | 0x40000000;
    } else {
        ans = (sign_b < 0) << 30;
    }
    return ans;
}

num_t do_mul(int sign_a, long num_a, int sign_b, long num_b) {
    long res = (num_b * num_a) >> 30;
    num_t ans;
    if (res > 0x3fffffff) {
        fprintf(stderr, "overflow\n");
        exit(EXIT_FAILURE);
    }
    if (sign_a * sign_b > 0) {
        ans = res & 0x3fffffff;
    } else if (sign_a * sign_b < 0) {
        ans = (res & 0x3fffffff) | 0x40000000;
    } else {
        ans = (sign_b < 0) << 30;
    }
    return ans;
}

num_t do_and(int sign_a, int num_a, int sign_b, int num_b) {
    int res = num_b & num_a;
    num_t ans = res | ((sign_b < 0) << 30);
    return ans;
}