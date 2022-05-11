
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "ir.h"
#include "type.h"
#include "stack.h"

#define max_ir_var_len 10
#define max_lable_len 10
#define max_temp_var_len 10

size_t ir_count = 0;
size_t temp_var_count = 0;
size_t lable_count = 0;

TempVar* temp_var_list;
IR* code_list;

char* get_ir_var_by_name(char* name) {
    FieldList filedlist = find_any_in_stack(name);
    return get_ir_var_by_field(filedlist);
}

char* get_ir_var_by_field(FieldList fieldlist) {
    if (fieldlist->ir_var != NULL) {
        return fieldlist->ir_var;
    }

    Type type = fieldlist->type;
    char* ir_var = malloc(sizeof(char) * (max_ir_var_len));
    if (type->kind == BASIC) {
        if (type->u.basic == T_INT) {
            sprintf(ir_var, "%s%lu", "iv", ir_count);
        } else if (type->u.basic == T_FLOAT) {
            sprintf(ir_var, "%s%lu", "fv", ir_count);
        }
    } else {
        sprintf(ir_var, "%s%lu", "v", ir_count);
    }
    fieldlist->ir_var = ir_var;
    ir_count++;
    return ir_var;
}

char* new_lable() {
    char* res = malloc(sizeof(char) * (max_lable_len));
    sprintf(res, "%s%lu", "label", lable_count);
    lable_count++;
    return res;
}

// init the list
void new_temp_var_list() {
    temp_var_list = malloc(sizeof(TempVar));
    temp_var_list->is_const = false;
    temp_var_list->next = NULL;
}


// code list methods begin

void new_ir_code_list() {
    code_list = malloc(sizeof(IR));
    code_list->code = NULL;
    code_list->next = code_list;
    code_list->prev = code_list;
}

void add_last_code(char* code) {
    IR* new_code = malloc(sizeof (IR));
    new_code->code = code;
    new_code->prev = code_list->prev;
    new_code->next = code_list;
    code_list->prev->next = new_code;
    code_list->prev = new_code;
}

void add_first_code(char* code) {
    IR* new_code = malloc(sizeof (IR));
    new_code->code = code;
    new_code->prev = code_list;
    new_code->next = code_list->next;
    code_list->next->prev = new_code;
    code_list->next = new_code;
}

void remove_first_code() {
    IR* temp = code_list->next;
    code_list->next = code_list->next->next;
    code_list->next->prev = code_list;
    free(temp);
}

void remove_last_code() {
    IR* temp = code_list->prev;
    code_list->prev = code_list->prev->prev;
    code_list->prev->next = code_list;
    free(temp);
}

// code list methods end;


// call this when creating non-const temp var
TempVar* get_temp_var(int type) {
    TempVar* res = malloc(sizeof(TempVar));
    char* name = malloc(sizeof(char) * (max_temp_var_len));
    if (type == 0) {
        sprintf(name, "%s%lu", "it", temp_var_count);
    } else if (type == 1) {
        sprintf(name, "%s%lu", "ft", temp_var_count);
    }
    temp_var_count++;
    res->name = name;
    res->is_const = false;
    
    res->next = temp_var_list->next;
    temp_var_list->next = res;

    return res;
}



// // call this when creating or finding const int temp var
// // first find, if not exist then create a new var
// TempVar* find_int_val(int i) {
//     TempVar* var = temp_var_list->next;
//     while (var != NULL) {
//         if (var->is_const && var->is_int && var->u.int_val == i) {
//             return var;
//         }
//         var = var->next;
//     }
//     // create new temp var
//     TempVar* new_var = new_temp_var();
//     new_var->is_const = true;
//     new_var->is_int = true;
//     new_var->u.int_val = i;
//     return new_var;
// }


// TempVar* find_float_val(float f) {
//     TempVar* var = temp_var_list->next;
//     while (var != NULL) {
//         if (var->is_const && !var->is_int && var->u.float_val == f) {
//             return var;
//         }
//         var = var->next;
//     }
//     // create new temp var
//     TempVar* new_var = new_temp_var();
//     new_var->is_const = true;
//     new_var->is_int = false;
//     new_var->u.float_val = f;
//     return new_var;
// }