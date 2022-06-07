#ifndef OBJECT_CODE_GEN_H
#define OBJECT_CODE_GEN_H
#include <stdbool.h>
#include <stddef.h>

typedef struct _ObjectCode ObjectCode;

typedef struct _LiveAnalysisUnit LiveAnalysisUnit;
struct _ObjectCode {
    char* code;
    ObjectCode* prev;
    ObjectCode* next;
};

struct _LiveAnalysisUnit {
    bool is_live;
    size_t next_use;
};

void init_ir_and_var_arr();

void set_block_by_label(char* label);

void divide_block();

void add_last_object_code(char* code);

#endif