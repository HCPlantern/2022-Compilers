#ifndef IR_H
#define IR_H

#include "stdbool.h"

typedef struct _TempVar TempVar;
typedef struct _IR IR;

struct _TempVar {
    char* name;
    bool is_const;
    // bool is_int;
    // union {
    //     int int_val;
    //     float float_val;
    // }u;
    TempVar* next;
};

struct _IR {
    char* code;
    IR* prev;
    IR* next;
};

char* new_ir_var();
void new_temp_var_list();
void new_ir_code_list();

TempVar* new_temp_var();
TempVar* find_int_val(int i);
TempVar* find_float_val(float f);

#endif