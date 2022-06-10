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

enum IrType {LABEL, FUNCTION, GOTO, IF, RETURN, DEC, ARG, PARAM, READ, WRITE, ASSIGN};

void init_ir_and_var_arr();

void set_block_by_label(char* label);

void divide_block();

void add_last_object_code(char* code);

enum IrType getIrType(char* ir);

void gen_call_code(int call_no);

int gen_call_with_arg_code(const int const ir_no);
#endif