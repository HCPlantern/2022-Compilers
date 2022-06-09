#ifndef OBJECT_CODE_GEN_H
#define OBJECT_CODE_GEN_H
#include <stdbool.h>
#include <stddef.h>

typedef struct _ObjectCode ObjectCode;

typedef struct _LiveAnalysisUnit LiveAnalysisUnit;

typedef struct _Register Register;

typedef struct _TempVar TempVar;

typedef struct _Function Function;

struct _ObjectCode {
    char* code;
    ObjectCode* prev;
    ObjectCode* next;
};

struct _LiveAnalysisUnit {
    bool is_live;
    size_t next_use;
};

struct _Register {
    size_t reg_no;
    char* name;
    TempVar* var;
    bool is_free;
};

struct _Function {
    char* name;
    size_t frame_size;
    Function* next;
}; 

void init_ir_and_var_arr();

void set_block_by_label(char* label);

void divide_block();

void add_last_object_code(char* code);

#endif