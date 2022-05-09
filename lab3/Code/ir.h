#ifndef IR_H
#define IR_H

#include "stdbool.h"

typedef struct _TempVar TempVar;
typedef struct _InterCode InterCode;

struct _TempVar {
    char* name;
    bool is_const;
    bool is_int;
    union {
        int int_val;
        float float_val;
    }u;
    TempVar* next;
};

struct _InterCode {
    char* code;
    InterCode* prev;
    InterCode* next;
};

char* new_ir();
void new_temp_var_list();
void new_inter_code_list();

TempVar* new_temp_var();
TempVar* find_int_val(int i);
TempVar* find_float_val(float f);

#endif