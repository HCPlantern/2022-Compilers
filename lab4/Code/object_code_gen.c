#include "object_code_gen.h"

#include <stdio.h>

#include "ir.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#define max_object_code_len 50
extern size_t ir_count;
extern TempVar* temp_var_list;
extern size_t var_count;
extern bool prefix(const char* pre, const char* str);

IR** ir_arr;
TempVar** var_arr;
ObjectCode* object_code;
Register** reg_arr;
Function* func_list;
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
    ir_arr = malloc(sizeof(IR*) * ir_count);
    IR* ir = ir_list->next;
    for (size_t i = 0; i < ir_count; i++) {
        if (0 == strcmp(ir->ir, "GOTO")) {
            ir_count--;
            i--;
            ir = ir->next;
            continue;
        }
        ir_arr[i] = ir;
        ir->ir_no = i;
        ir = ir->next;
    }

    var_arr = malloc(sizeof(TempVar*) * var_count);
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
    new_code->code = malloc(sizeof(char) * max_object_code_len);
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
                } else {
                    var_arr[var_no]->live_analysis[ir_no]->is_live = false;
                    var_arr[var_no]->live_analysis[ir_no]->is_live = __UINTMAX_MAX__;
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

void init_regs() {
    reg_arr = malloc(sizeof(Register*) * 32);
    Register* curr_reg;
    for (int i = 0; i < 32; i++) {
        curr_reg = reg_arr[i];
        curr_reg = malloc(sizeof(Register));
        curr_reg->reg_no = i;
        curr_reg->name = malloc(sizeof(char) * 5);
        curr_reg->var = NULL;
        if (i == 0) {
            sprintf(curr_reg->name, "%s", "zero");
        } else if (i == 1) {
            sprintf(curr_reg->name, "%s", "at");
        } else if (i == 2 || i == 3) {
            sprintf(curr_reg->name, "%s%d", "v", i - 2);
        } else if (4 <= i && i <= 7) {
            sprintf(curr_reg->name, "%s%d", "a", i - 4);
        } else if (8 <= i && i <= 15) {
            sprintf(curr_reg->name, "%s%d", "t", i - 8);
        } else if (16 <= i && i <= 23) {
            sprintf(curr_reg->name, "%s%d", "s", i - 16);
        } else if (i == 24 || i == 25) {
            sprintf(curr_reg->name, "%s%d", "t", i - 24);
        } else if (i == 26 || i == 27) {
            sprintf(curr_reg->name, "%s%d", "k", i - 26);
        } else if (i == 28) {
            sprintf(curr_reg->name, "%s", "gp");
        } else if (i == 29) {
            sprintf(curr_reg->name, "%s", "sp");
        } else if (i == 30) {
            sprintf(curr_reg->name, "%s", "fp");
        } else if (i == 31) {
            sprintf(curr_reg->name, "%s", "ra");
        }
    }
}

TempVar* get_var(char* var_name) {
    size_t var_no = atoi(var_name + 2);
    return var_arr[var_no];
}

bool is_live(char* var_name, size_t ir_no) {
    size_t var_no = atoi(var_name + 2);
    return get_var(var_name)->live_analysis[ir_no]->is_live;
}

size_t next_use(char* var_name, size_t ir_no) {
    return get_var(var_name)->live_analysis[ir_no]->next_use;
}

size_t get_func_offset(char* name) {
    Function* curr = func_list->next;
    if (!strcmp(curr->name, name)) {
        return curr->frame_size;
    }
    return 0;
}

void add_func_list(Function* func) {
    Function* curr = func_list;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = func;
}

void set_var_offset(char* var, size_t offset) {
    size_t var_no = atoi(var + 2);
    var_arr[var_no]->fp_offset = offset;
}

void cal_framesize() {
    func_list = malloc(sizeof(Function));
    func_list->frame_size = 0;
    func_list->name = NULL;
    func_list->next = NULL;
    for (int i = 0; i < ir_count; i++) {
        char curr_ir[max_single_ir_len];
        // copy ir for strtok use
        strncpy(curr_ir, ir_arr[i]->ir, max_single_ir_len);
        char* token = strtok(curr_ir, " ");
        // function begin
        if (!strcmp("FUNCTION", token)) {
            size_t frame_size = 8;
            token = strtok(NULL, " ");

            Function* new_func = malloc(sizeof(Function));
            new_func->name = malloc(sizeof(char) * strlen(token));
            strncpy(new_func->name, token, strlen(token));

            add_func_list(new_func);
            // cal frame size.
            for (int j = i + 1; j < ir_count; j++) {
                if (prefix("FUNCTION", ir_arr[j]->ir)) {
                    break;
                }
                // copy ir for strtok use
                char temp_ir[max_single_ir_len];
                strncpy(temp_ir, ir_arr[j]->ir, max_single_ir_len);
                token = strtok(temp_ir, " ");
                // PARAM iv0
                if (!strcmp("PARAM", token)) {
                    token = strtok(NULL, " ");
                    set_var_offset(token, frame_size + 4);
                    frame_size += 4;
                } else if (!strcmp("DEC", token)) {
                    // DEC iv0 [size]
                    token = strtok(NULL, " ");
                    size_t size = atoi(token);
                    set_var_offset(token, frame_size + 4);
                    frame_size += size;
                } else {
                    // iv0 := ...
                    if (token[0] != 'i') {
                        continue;
                    }
                    TempVar* var = get_var(token);
                    // first time
                    if (var->fp_offset == 0) {
                        set_var_offset(token, frame_size + 4);
                        frame_size += 4;
                    }
                }
            }
            new_func->frame_size = frame_size;
        }
    }
}

void spill(Register* reg) {
    TempVar* var = reg->var;
    assert(var != NULL);
    char code[max_object_code_len];
    // spill reg value into memory
    sprintf(code, "sw, $%s, -%lu(fp)", reg->name, var->fp_offset);
    add_last_object_code(code);
    var->reg = NULL;
    reg->var = NULL;    
}

Register* allocate(TempVar* var, size_t ir_no) {
    Register* res;
    for (int i = 8; i <= 23; i++) {
        Register* curr = reg_arr[i];
        if (curr->var == NULL) {
            curr->var = var;
            var->reg = curr;
            return curr;
        } else {
            if (res == NULL || next_use(curr->var->name, ir_no) > next_use(res->var->name, ir_no)) {
                res = curr;
            }
        }
    }
    spill(res);
    res->var = var;
    var->reg = res;
    return res;
}

Register* ensure_var(char* var_name, size_t ir_no) {
    Register* res;
    size_t var_no = atoi(var_name + 2);
    TempVar* curr_var = var_arr[var_no];
    if (curr_var->reg != NULL) {
        res = curr_var->reg;
    } else {
        res = allocate(curr_var, ir_no);
        char code[max_object_code_len];
        // load var value into register
        sprintf(code, "lw $%s, -%lu(fp)", res->name, curr_var->fp_offset);
        add_last_object_code(code);
    }
    return res;
}

void gen_func_code(char* func_name) {
    char code[max_object_code_len] = {0};
    strcpy(code, func_name);
    add_last_object_code(code);
}

void gen_label_code(char* label_name) {
    char code[max_object_code_len] = {0};
    strcpy(code, label_name);
    add_last_object_code(code);
}

void gen_goto_code(char* label_name) {
    char code[max_object_code_len] = {0};
    sprintf(code, "j %s", label_name);
    add_last_object_code(code);
}

void gen_return_code() {
}

void gen_read_code() {
}

void gen_write_code() {
}

void gen_call_code() {
}

void gen_if_code() {
}

// generate object code
void gen_object_code() {
    IR* curr_ir;
    char* curr_ir_code;
    for (size_t ir_no = 0; ir_no < ir_count; ir_no++) {
        curr_ir = ir_arr[ir_no];
        if (curr_ir->is_block_begin) {
            // spill all registers
        }
        curr_ir_code = curr_ir->ir;
        if (prefix("FUNCTION", curr_ir_code)) {
            gen_func_code(curr_ir_code + 9);
        } else if (prefix("LABEL", curr_ir_code)) {
            gen_label_code(curr_ir_code + 6);
        } else if (prefix("GOTO", curr_ir_code)) {
            gen_goto_code(curr_ir_code + 5);
        } else if (prefix("RETURN", curr_ir_code)) {
            //
        } else if (prefix("READ", curr_ir_code)) {
            gen_read_code();
        } else if (prefix("WRITE", curr_ir_code)) {
            //
        } else if (prefix("CALL", curr_ir_code)) {
            //
        } else if (prefix("IF", curr_ir_code)) {
            //
        } else {
            // assign statement
        }
    }
}

void object_code_gen_go() {
    init_ir_and_var_arr();
    divide_block();
    init_object_code();
    live_variable_analysis();
    init_regs();
    cal_framesize();
    gen_object_code();

    // check live analysis
    // for (int i = 0; i < ir_count; i++) {
    //     printf("\n");
    //     printf("%d: %s\n", i, ir_arr[i]->ir);
    //     for (int j = 0; j < var_count; j++) {
    //         printf("\n");
    //         printf("Variable %s: \n", var_arr[j]->name);
    //         printf("Is live: %5d\n", var_arr[j]->live_analysis[i]->is_live);
    //         if (!(var_arr[j]->live_analysis[i]->next_use == __UINTMAX_MAX__)) {
    //             printf("Next use: %lu\n", var_arr[j]->live_analysis[i]->next_use);
    //         } else {
    //             printf("Next use: unused\n");
    //         }
    //     }
    // }

    // print all object codes
    // ObjectCode* curr = object_code->next;
    // printf("\n");
    // while (curr != object_code) {
    //     printf("%s\n", curr->code);
    //     curr = curr->next;
    // }

    // print all func frame size
    Function* curr = func_list->next;
    while (curr != NULL) {
        printf("%s %lu\n", curr->name, curr->frame_size);
        curr = curr->next;
    }
}
