
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "ir.h"
#include "type.h"
#include "stack.h"

extern Stack stack;

#define max_ir_var_len 10
#define max_lable_len 10
#define max_temp_var_len 10
#define curr_table (stack->tables[stack->top - 1])

size_t ir_count = 0;
size_t temp_var_count = 0;
size_t lable_count = 0;

TempVar* temp_var_list;
IR* code_list;

void init_read_write_func() {
    Type read_type = malloc(sizeof(struct _Type));
    Type int_type = create_int_type();
    read_type->kind = FUNC;
    read_type->u.function.arg_len = 0;
    read_type->u.function.args = NULL;
    read_type->u.function.is_defined = true;
    read_type->u.function.return_type = int_type;
    FieldList read_field = malloc(sizeof(struct _FieldList));
    read_field->name = "read";
    read_field->type = read_type;
    read_field->next = NULL;
    read_field->is_var = false;
    read_field->size = -1;
    read_field->ir_var = NULL;
    add_table_node(curr_table, read_field);

    // init arg for write
    FieldList arg = malloc(sizeof(struct _FieldList));
    arg->name = "input";
    arg->type = int_type;
    arg->next = NULL;
    arg->is_var = true;
    arg->size = 4;
    arg->ir_var = NULL;

    Type write_type = malloc(sizeof(struct _Type));
    write_type->kind = FUNC;
    write_type->u.function.arg_len = 1;
    write_type->u.function.args = arg;
    write_type->u.function.is_defined = true;
    write_type->u.function.return_type = int_type;

    FieldList write_field = malloc(sizeof(struct _FieldList));
    write_field->name = "write";
    write_field->type = write_type;
    write_field->next = NULL;
    write_field->is_var = false;
    write_field->size = -1;
    write_field->ir_var = NULL;
    add_table_node(curr_table, write_field);
}

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
        sprintf(ir_var, "%s%lu", "&v", ir_count);
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