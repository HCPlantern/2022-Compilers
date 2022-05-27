
#include "ir.h"

#include "stack.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "type.h"
#include "stddef.h"

extern Stack stack;
extern bool has_syntax_error;
extern bool prefix(const char* pre, const char* str);

#define curr_table (stack->tables[stack->top - 1])

size_t ir_count = 0;
size_t temp_var_count = 0;
size_t label_count = 0;

TempVar* temp_var_list;

void init_read_write_func() {
    Type read_type = malloc(sizeof(struct _Type));
    Type int_type = create_int_type();
    read_type->kind = FUNC;
    read_type->u.function.arg_len = 0;
    read_type->u.function.args = NULL;
    read_type->u.function.is_defined = true;
    read_type->u.function.return_type = int_type;
    FieldList read_field = init_fieldlist("read", read_type, NULL, false, -1, NULL);
    add_table_node(curr_table, read_field);

    // init arg for write
    FieldList arg = init_fieldlist("input", int_type, NULL, true, 4, NULL);
    Type write_type = malloc(sizeof(struct _Type));
    write_type->kind = FUNC;
    write_type->u.function.arg_len = 1;
    write_type->u.function.args = arg;
    write_type->u.function.is_defined = true;
    write_type->u.function.return_type = int_type;

    FieldList write_field = init_fieldlist("write", write_type, NULL, false, -1, NULL);
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

char* new_label() {
    char* res = malloc(sizeof(char) * (max_label_len));
    sprintf(res, "%s%lu", "label", label_count);
    label_count++;
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
    ir_list = malloc(sizeof(IR));
    ir_list->ir = NULL;
    ir_list->next = ir_list;
    ir_list->prev = ir_list;
    ir_list->label_next = NULL;
    ir_list->label_printed = false;
}

void add_last_ir(char* code) {
    IR* new_ir = malloc(sizeof(IR));
    new_ir->label_next = NULL;
    new_ir->label_printed = false;
    new_ir->ir = malloc(sizeof(char) * max_single_ir_len);
    strcpy(new_ir->ir, code);
    new_ir->prev = ir_list->prev;
    new_ir->next = ir_list;
    ir_list->prev->next = new_ir;
    ir_list->prev = new_ir;
}

void add_first_ir(char* code) {
    IR* new_ir = malloc(sizeof(IR));
    new_ir->label_next = NULL;
    new_ir->label_printed = false;
    new_ir->ir = malloc(sizeof(char) * max_single_ir_len);
    strcpy(new_ir->ir, code);
    new_ir->prev = ir_list;
    new_ir->next = ir_list->next;
    ir_list->next->prev = new_ir;
    ir_list->next = new_ir;
}

void add_next_ir(IR* ir_node, char* code) {
    IR* new_ir = malloc(sizeof(IR));
    new_ir->label_next = NULL;
    new_ir->label_printed = false;
    new_ir->ir = malloc(sizeof(char) * max_single_ir_len);
    strcpy(new_ir->ir, code);
    new_ir->prev = ir_node;
    new_ir->next = ir_node->next;
    ir_node->next->prev = new_ir;
    ir_node->next = new_ir;
}

void remove_first_ir() {
    IR* temp = ir_list->next;
    ir_list->next = ir_list->next->next;
    ir_list->next->prev = ir_list;
    free(temp);
}

void remove_last_ir() {
    IR* temp = ir_list->prev;
    ir_list->prev = ir_list->prev->prev;
    ir_list->prev->next = ir_list;
    free(temp);
}

void remove_next_ir(IR* ir_node) {
    IR* next_ir = ir_node->next;
    ir_node->next = next_ir->next;
    next_ir->next->prev = ir_node;
    free(next_ir);
}

void print_ir() {
    if (has_syntax_error) return;
    IR* code = ir_list->next;
    while (code != ir_list) {
        if (0 != strcmp(code->ir, "GOTO")) {
            printf("%s\n", code->ir);
        }
        code = code->next;
    }
}

// return blank index + 1 of a str
// blank_num starts from 1
size_t get_blank_index(char* str, size_t blank_num) {
    size_t blank_count = 0;
    size_t index = 0;
    while (blank_count < blank_num) {
        if (*str == ' ') {
            blank_count++;
        }
        str++;
        index++;
    }
    return index;
}

char* relop_negative(char* relop) {
    char* res = malloc(sizeof(char) * 3);
    if (!strcmp(relop, "==")) {
        strcpy(res, "!=");
    } else if (!strcmp(relop, "!=")) {
        strcpy(res, "==");
    } else if (!strcmp(relop, ">")) {
        strcpy(res, "<=");
    } else if (!strcmp(relop, "<")) {
        strcpy(res, ">=");
    } else if (!strcmp(relop, ">=")) {
        strcpy(res, "<");
    } else if (!strcmp(relop, "<=")) {
        strcpy(res, ">");
    }
    return res;
}

void ir_optimization() {
    IR* curr_ir = ir_list->next;
    IR* next_ir;
    while (curr_ir != ir_list) {
        if (prefix("IF", curr_ir->ir)) { // optimize goto
            // change label name
            size_t blank1 = get_blank_index(curr_ir->ir, 1);
            size_t blank2 = get_blank_index(curr_ir->ir, 2);
            size_t blank3 = get_blank_index(curr_ir->ir, 3);
            size_t blank4 = get_blank_index(curr_ir->ir, 4);
            size_t blank5 = get_blank_index(curr_ir->ir, 5);

            char var1[max_temp_var_len];
            char var2[max_temp_var_len];
            memset(var1, 0, max_temp_var_len);
            memset(var2, 0, max_temp_var_len);
            strncpy(var1, curr_ir->ir + blank1, blank2 - blank1 - 1);
            strncpy(var2, curr_ir->ir + blank3, blank4 - blank3 - 1);

            // get label name
            next_ir = curr_ir->next;
            size_t next_ir_label = get_blank_index(next_ir->ir, 1);
            char label[max_label_len] = {0};
            strncpy(label, next_ir->ir + next_ir_label, max_label_len);

            // change relop
            size_t relop_len = blank3 - blank2 - 1;
            // get relop
            char* relop = malloc(sizeof(3));
            memset(relop, 0, 3);
            strncpy(relop, curr_ir->ir + blank2, relop_len);
            char* relop_neg = relop_negative(relop);
            free(relop);
            
            char* ir = malloc(sizeof(max_single_ir_len));
            sprintf(ir, "IF %s %s %s GOTO %s", var1, relop_neg, var2, label);
            free(curr_ir->ir);
            curr_ir->ir = ir;

            remove_next_ir(curr_ir);
            remove_next_ir(curr_ir);
        }
        curr_ir = curr_ir->next;
    }
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