#ifndef IR_H
#define IR_H

#include "stdbool.h"

typedef struct _TempVar TempVar;
typedef struct _IR IR;
typedef struct _FieldList* FieldList;

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

char* get_ir_var_by_name(char* name);
char* get_ir_var_by_field(FieldList fieldlist);

char* new_lable();
void new_temp_var_list();
void new_ir_code_list();

TempVar* get_temp_var(int type);
// TempVar* find_int_val(int i);
// TempVar* find_float_val(float f);

#endif