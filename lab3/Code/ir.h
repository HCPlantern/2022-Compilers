#ifndef IR_H
#define IR_H

#include <stdbool.h>

#define max_ir_var_len 10   // '*' and '&' are taken into account.
#define max_label_len 10
#define max_temp_var_len 10
#define max_single_ir_len 100

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
    char* ir;
    IR* prev;
    IR* next;
    char* label_next;
    bool label_printed;
};

IR* ir_list;

char* get_ir_var_by_name(char* name);
char* get_ir_var_by_field(FieldList fieldlist);

char* new_label();
void new_temp_var_list();
void new_ir_code_list();
void add_last_ir(char* code);
void add_next_ir(IR* ir_node, char* code);
void print_ir();

TempVar* get_temp_var(int type);
// TempVar* find_int_val(int i);
// TempVar* find_float_val(float f);

#endif