#include "object_code_gen.h"

#include <stdio.h>

#include "ir.h"
#include "stdlib.h"
#include "string.h"
#include "ir.h"
#define max_object_code_len 50
extern size_t ir_count;
extern TempVar* temp_var_list;
extern size_t var_count;
extern bool prefix(const char* pre, const char* str);

IR** ir_arr;
TempVar** var_arr;
ObjectCode* object_code;
size_t object_code_len;

// Put all IRs into an array
void init_ir_and_var_arr() {
    ir_arr = malloc(sizeof(IR) * ir_count);
    IR* ir = ir_list->next;
    for (size_t i = 0; i < ir_count; i++) {
        ir_arr[i] = ir;
        ir->ir_no = i;
        ir = ir->next;
    }

    var_arr = malloc(sizeof(TempVar) * var_count);
    TempVar* var = temp_var_list->next;
    for (size_t i = 0; i < var_count; i++) {
        var_arr[i] = var;
        var = var->next;
    }
}

void set_block_by_label(char* label) {
    IR* ir;
    for (int i = 0; i < ir_count; i++) {
        ir = ir_arr[i];
        if (prefix("LABEL", ir->ir)) {
            size_t label_index = get_blank_index(ir->ir, 1);
            if (prefix(label, ir->ir + label_index)) {
                // set next ir as begin of a basic block
                ir_arr[i]->is_block_begin = true;
            }
        }
    }
}

void divide_block() {
    IR* ir;
    // first ir is basic block begin
    ir_arr[0]->is_block_begin = true;
    for (int i = 0; i < ir_count; i++) {
        ir = ir_arr[i];
        if (ir->is_block_begin) {
            continue;
        }
        if (prefix("GOTO", ir->ir)) {
            set_block_by_label(ir->ir + get_blank_index(ir->ir, 1));
            if (i + 1 < ir_count) {
                ir_arr[i + 1]->is_block_begin = true;
            }
        } else if (prefix("IF", ir->ir)) {
            set_block_by_label(ir->ir + get_blank_index(ir->ir, 5));
            if (i + 1 < ir_count) {
                ir_arr[i + 1]->is_block_begin = true;
            }
        } else if (prefix("CALL", ir->ir + get_blank_index(ir->ir, 2))) {
            ir_arr[i]->is_block_begin = true;
        }
    }
}

void init_object_code() {
    object_code = malloc(sizeof(ObjectCode));
    object_code->code = NULL;
    object_code->next = object_code;
    object_code->prev = object_code;

};

void add_last_object_code(char* code) {
    ObjectCode* new_code = malloc(sizeof(ObjectCode));
    strncpy(new_code->code, code, max_object_code_len);

    new_code->prev = object_code->prev;
    new_code->next = object_code;
    object_code->prev->next = new_code;
    object_code->prev = new_code;
    object_code_len++;
}

void object_code_gen_go() {
    init_ir_and_var_arr();
    divide_block();
    init_object_code();

}
