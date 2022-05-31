#include "object_code_gen.h"

#include "ir.h"
#include "stdlib.h"
#include "string.h"

extern size_t ir_count;
extern bool prefix(const char* pre, const char* str);

IR** ir_arr;

// Put all IRs into an array
void init_ir_arr() {
    ir_arr = malloc(sizeof(IR) * ir_count);
    IR* ir = ir_list->next;
    for (size_t i = 0; i < ir_count; ir++) {
        ir_arr[i] = ir;
        ir->ir_no = i;
        ir = ir->next;
    }
}

void set_block_by_label(char* label) {
    IR* ir;
    for (int i = 0; i < ir_count; i++) {
        ir = ir_arr[i];
        if (prefix("LABEL", ir->ir)) {
            size_t label_index = get_blank_index(ir->ir, 1);
            if (strcmp(label, ir->ir + label_index) == 0 && (i + 1) < ir_count) {
                // set next ir as begin of a basic block
                ir_arr[i + 1]->is_block_begin = true;
            }
        }
    }
}

void divide_block() {
    IR* ir;
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
        }
    }
}