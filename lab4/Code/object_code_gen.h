
typedef struct _ObjectCode ObjectCode;

struct _ObjectCode {
    char* code;
    ObjectCode* prev;
    ObjectCode* next;
};

void init_ir_arr();

void set_block_by_label(char* label);

void divide_block();

void add_last_object_code(char* code);