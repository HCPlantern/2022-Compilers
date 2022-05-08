#ifndef IR_H
#define IR_H
#include "stdbool.h"
typedef struct _TempVar TempVar;

struct _TempVar {
    char* name;
    bool is_const;
    int const_val;
    TempVar* next;
};

char* new_ir();
void new_temp_var_list();
TempVar* new_temp_var();
TempVar* find_const_var(int i);

#endif