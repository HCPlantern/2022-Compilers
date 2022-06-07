#include "object_code_gen.h"

#include <stdio.h>

#include "ir.h"
#include "stdlib.h"
#include "string.h"
#define max_object_code_len 50
extern size_t ir_count;
extern TempVar* temp_var_list;
extern size_t var_count;
extern bool prefix(const char* pre, const char* str);

IR** ir_arr;
TempVar** var_arr;
ObjectCode* object_code;
size_t object_code_len;

size_t count_blanks(char* str) {
    size_t res = 0;
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (*str == ' ') {
            res++;
        }
        str++;
    }
    return res;
}
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

void live_variable_analysis() {
    TempVar* curr;
    // init
    for (int i = 0; i < var_count; i++) {
        curr = var_arr[i];
        curr->live_analysis = malloc(ir_count * sizeof(LiveAnalysisUnit*));
        for (int j = 0; j < ir_count; j++) {
            curr->live_analysis[j] = malloc(sizeof(LiveAnalysisUnit));
            curr->live_analysis[j]->is_live = false;
            curr->live_analysis[j]->next_use = __UINTMAX_MAX__;
        }
    }

    // 从后往前遍历
    for (int ir_no = ir_count - 1; ir_no >= 0; ir_no--) {
        IR* curr_ir = ir_arr[ir_no];
        if (ir_no == ir_count - 1 || ir_arr[ir_no + 1]->is_block_begin) {
            // this ir is basic block end
            for (int var_no = 0; var_no < var_count; var_no++) {
                if (var_arr[var_no]->name[1] == 'v') {
                    var_arr[var_no]->live_analysis[ir_no]->is_live = true;
                }
            }
        } else {
            // pass live variable infomation
            for (int var_no = 0; ir_no != ir_count - 1 && var_no < var_count; var_no++) {
                TempVar* curr_var = var_arr[var_no];
                LiveAnalysisUnit* curr_unit = curr_var->live_analysis[ir_no];
                LiveAnalysisUnit* prev_unit = curr_var->live_analysis[ir_no + 1];
                curr_unit->is_live = prev_unit->is_live;
                curr_unit->next_use = prev_unit->next_use;
            }
        }

        if (
            prefix("FUNCTION", curr_ir->ir) ||
            prefix("PARAM", curr_ir->ir) ||
            prefix("ARG", curr_ir->ir) ||
            prefix("GOTO", curr_ir->ir) ||
            prefix("LABEL", curr_ir->ir) ||
            prefix("READ", curr_ir->ir) ||
            prefix("WRITE", curr_ir->ir) ||
            prefix("RETURN", curr_ir->ir)) {
            continue;
        } else if (prefix("IF", curr_ir->ir)) {
            size_t blank1 = get_blank_index(curr_ir->ir, 1);
            size_t blank2 = get_blank_index(curr_ir->ir, 2);
            size_t blank3 = get_blank_index(curr_ir->ir, 3);
            size_t blank4 = get_blank_index(curr_ir->ir, 4);
            char var1[max_temp_var_len] = {0};
            char var2[max_temp_var_len] = {0};
            strncpy(var1, curr_ir->ir + blank1, blank2 - blank1 - 1);
            strncpy(var2, curr_ir->ir + blank3, blank4 - blank3 - 1);
            if (var1[0] != '#') {
                size_t var_no = atoi(var1 + 2);
                TempVar* temp_var = var_arr[var_no];
                temp_var->live_analysis[ir_no]->is_live = true;
                temp_var->live_analysis[ir_no]->next_use = ir_no;
            }
            if (var2[0] != '#') {
                size_t var_no = atoi(var2 + 2);
                TempVar* temp_var = var_arr[var_no];
                temp_var->live_analysis[ir_no]->is_live = true;
                temp_var->live_analysis[ir_no]->next_use = ir_no;
            }
        } else {
            size_t blanks = count_blanks(curr_ir->ir);
            size_t blank1 = get_blank_index(curr_ir->ir, 1);
            size_t blank2 = get_blank_index(curr_ir->ir, 2);
            char var1[max_temp_var_len] = {0};
            char var2[max_temp_var_len] = {0};
            char var3[max_temp_var_len] = {0};
            if (blanks == 2) {
                // it1 := it2
                strncpy(var1, curr_ir->ir, blank1 - 1);
                strncpy(var2, curr_ir->ir + blank2, max_temp_var_len);
                // var1
                size_t var_no = atoi(var1 + 2);
                TempVar* temp_var = var_arr[var_no];
                temp_var->live_analysis[ir_no]->is_live = false;
                temp_var->live_analysis[ir_no]->next_use = __UINTMAX_MAX__;
                // var2
                if (var2[0] != '#') {
                    size_t var_no = atoi(var2 + 2);
                    TempVar* temp_var = var_arr[var_no];
                    temp_var->live_analysis[ir_no]->is_live = true;
                    temp_var->live_analysis[ir_no]->next_use = ir_no;
                }
            } else if (blanks == 3) {
                // it1 := CALL func
                strncpy(var1, curr_ir->ir, blank1 - 1);
                size_t var_no = atoi(var1 + 2);
                TempVar* temp_var = var_arr[var_no];
                temp_var->live_analysis[ir_no]->is_live = false;
                temp_var->live_analysis[ir_no]->next_use = __UINTMAX_MAX__;

            } else if (blanks == 4) {
                // it1 := it2 * it3
                size_t blank3 = get_blank_index(curr_ir->ir, 3);
                size_t blank4 = get_blank_index(curr_ir->ir, 4);
                strncpy(var1, curr_ir->ir, blank1 - 1);
                strncpy(var2, curr_ir->ir + blank2, blank3 - blank2);
                strncpy(var3, curr_ir->ir + blank4, strlen(curr_ir->ir) - blank4);

                // var1
                strncpy(var1, curr_ir->ir, blank1 - 1);
                size_t var_no = atoi(var1 + 2);
                TempVar* temp_var = var_arr[var_no];
                temp_var->live_analysis[ir_no]->is_live = false;
                temp_var->live_analysis[ir_no]->next_use = __UINTMAX_MAX__;
                // var2
                if (var2[0] != '#') {
                    size_t var_no = atoi(var2 + 2);
                    TempVar* temp_var = var_arr[var_no];
                    temp_var->live_analysis[ir_no]->is_live = true;
                    temp_var->live_analysis[ir_no]->next_use = ir_no;
                }
                // var 3
                if (var3[0] != '#') {
                    size_t var_no = atoi(var3 + 2);
                    TempVar* temp_var = var_arr[var_no];
                    temp_var->live_analysis[ir_no]->is_live = true;
                    temp_var->live_analysis[ir_no]->next_use = ir_no;
                }
            }
        }
    }
}

void object_code_gen_go() {
    init_ir_and_var_arr();
    divide_block();
    init_object_code();
    live_variable_analysis();

    // check live analysis
    for (int i = 0; i < ir_count; i++) {
        printf("\n");
        printf("%d: %s\n", i, ir_arr[i]->ir);
        for (int j = 0; j < var_count; j++) {
            printf("\n");
            printf("Variable %s: \n", var_arr[j]->name);
            printf("Is live: %5d\n", var_arr[j]->live_analysis[i]->is_live);
            if (!(var_arr[j]->live_analysis[i]->next_use == __UINTMAX_MAX__)) {
                printf("Next use: %lu\n", var_arr[j]->live_analysis[i]->next_use);
            } else {
                printf("Next use: unused\n");
            }
        }
    }
}
