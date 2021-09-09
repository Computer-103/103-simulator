#define DECIMAL 1
#define OCTAL   0

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