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
num_t reg_op;

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
    num_t c1, c2;
    num_t op1, op2;
    while (1) {
        // 1
        reg_addr = reg_pc;
        reg_pc = reg_pc + 1;
        
        // 2
        reg_c = machine_mem[reg_addr];
        reg_op = (reg_c >> 24) & 0x3f;
        op1 = reg_op >> 3;
        op2 = reg_op & 0x7;
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
            if (reg_op == 5 || reg_op == 7) {
                sign_a = 1;
                sign_b = 1;
            } else {
                sign_a = (reg_a & 0x40000000) ? -1 : 1;
                sign_b = (reg_b & 0x40000000) ? -1 : 1;
            }

            if (op2 == OP_ADD) {
                reg_c = do_add(sign_a, reg_a & 0x3fffffff, sign_b, reg_b & 0x3fffffff);
            } else if (op2 == OP_SUB) {
                reg_c = do_sub(sign_a, reg_a & 0x3fffffff, sign_b, reg_b & 0x3fffffff);
            } else if (op2 == OP_DIV) {
                reg_c = do_div(sign_a, reg_a & 0x3fffffff, sign_b, reg_b & 0x3fffffff);
            } else if (op2 == OP_MUL) {
                reg_c = do_mul(sign_a, reg_a & 0x3fffffff, sign_b, reg_b & 0x3fffffff);
            } else if (op2 == OP_AND) {
                reg_c = do_and(sign_a, reg_a & 0x3fffffff, sign_b, reg_b & 0x3fffffff);
            }

            reg_b = reg_c;

            if (op2 == 0 || op2 == 2 || reg_op == 4 || reg_op == 6) {
                machine_mem[reg_addr] = reg_c;
            }
            if (reg_op == 4 || reg_op == 6) {
                output_reg_c();
            }

        } else if (reg_op == 007 || reg_op == 027) {
            // TODO: not support input
            reg_addr = c2;
            machine_mem[reg_addr] = 0001234567;

        } else if (reg_op == 005 || reg_op == 015) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_addr = c2;
            machine_mem[reg_addr] = reg_c;
            reg_b = reg_c;

        } else if (reg_op == 045 || reg_op == 055) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_addr = c2;
            machine_mem[reg_addr] = reg_c;
            output_reg_c();

        } else if (reg_op == 024) {
            reg_pc = c1;
            reg_addr = c2;
            reg_c = reg_b;
            machine_mem[reg_addr] = reg_c;

        } else if (reg_op == 064) {
            reg_pc = c1;
            reg_addr = c2;
            reg_c = reg_b;
            machine_mem[reg_addr] = reg_c;
            output_reg_c();

        } else if (reg_op == 074) {
            reg_pc = c2;
            reg_b = reg_b & 0x3fffffff;

        } else if (reg_op == 034) {
            if (reg_b & 0x40000000) {
                reg_pc = c1;
            } else {
                reg_pc = c2;
            }

        } else if (reg_op == 004 || reg_op == 014 || reg_op == 044) {
            reg_pc = c1;
            reg_a = reg_c;
            reg_addr = c2;
            reg_c = machine_mem[reg_addr];
            reg_b = reg_c;
            break;

        } else if (reg_op == 054) {
            reg_pc = c1;
            reg_a = reg_c;
            reg_a = reg_a & 0x3fffffff;
            reg_addr = c2;
            reg_c = machine_mem[reg_addr];
            reg_b = reg_c;
            reg_b = reg_b & 0x3fffffff;
            break;

        } else if (reg_op == 037) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_addr = c2;
            reg_a = reg_c;
            reg_c = reg_b;
            break;

        } else if (reg_op == 057) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_addr = c2;
            reg_a = reg_c;
            reg_c = machine_mem[reg_addr];
            reg_b = reg_c;
            reg_a = reg_a & 0x3fffffff;
            reg_b = reg_b & 0x3fffffff;
            break;

        } else if (reg_op == 077) {
            reg_addr = c1;
            reg_c = machine_mem[reg_addr];
            reg_addr = c2;
            reg_a = reg_c;
            reg_c = reg_b;
            reg_a = reg_a & 0x3fffffff;
            reg_b = reg_b & 0x3fffffff;
            break;

        } else {
            fprintf(stderr, "unknow reg_op code\n");
            exit(EXIT_FAILURE);
        }
        print_status();
    }

    print_status();
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

void print_status() {
    printf("pc: %04o\t", reg_pc);
    printf("addr: %04o\t", reg_addr);
    printf("op: %02o\t", reg_op);
    printf("c: %c %010o", (reg_c & 0x40000000) ? '-' : '+', reg_c & 0x3fffffff);
    printf("\n");
}