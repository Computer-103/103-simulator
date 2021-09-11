#define DECIMAL 1
#define OCTAL   0

#define OP_ADD  0
#define OP_SUB  1
#define OP_DIV  2
#define OP_MUL  3
#define OP_AND  6

typedef unsigned int num_t;

void set_pc(int val);
void set_output_numeral(int val);

void init_mem();
void setup_mem(char * tape_filename);
void print_mem();

void mem_write();
void mem_read();

void init_machine();
void run_machine();

void init_output();
void output_reg_c();

num_t do_add(int sign_a, int num_a, int sign_b, int num_b);
num_t do_sub(int sign_a, int num_a, int sign_b, int num_b);
num_t do_div(int sign_a, long num_a, int sign_b, long num_b);
num_t do_mul(int sign_a, long num_a, int sign_b, long num_b);
num_t do_and(int sign_a, int num_a, int sign_b, int num_b);