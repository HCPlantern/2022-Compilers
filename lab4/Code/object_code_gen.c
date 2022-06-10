#include "object_code_gen.h"

#include <stdio.h>

#include "assert.h"
#include "ir.h"
#include "stdlib.h"
#include "string.h"
#define max_object_code_len 50
extern size_t ir_count;
extern TempVar* temp_var_list;
extern size_t var_count;
extern IR* ir_list;
extern bool prefix(const char* pre, const char* str);

IR** ir_arr;
TempVar** var_arr;
ObjectCode* object_code;
Register** reg_arr;
Function* func_list;
size_t object_code_len;
int current_func_frameSize;

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

bool contain_CALL(char* ir) {
    if (prefix("CALL", ir)) return true;
    if (prefix("CALL", ir + get_blank_index(ir, 2))) return true;
}

void divide_block() {
    IR* ir;
    // first ir is basic block begin
    ir_arr[0]->is_block_begin = true;
    for (int i = 0; i < ir_count; i++) {
        ir = ir_arr[i];
        if (ir->is_block_begin) {
            continue;
            // with args funciton call
        } else if (prefix("ARG", ir->ir)) {
            ir->is_block_begin = true;
            while (prefix("ARG", ir_arr[i]->ir)) {
                i++;
            }
            if (contain_CALL(ir_arr[i]->ir) && i + 1 < ir_count) {
                ir_arr[i + 1]->is_block_begin = true;
            }
        } else if (prefix("GOTO", ir->ir)) {
            set_block_by_label(ir->ir + get_blank_index(ir->ir, 1));
            if (i + 1 < ir_count) {
                ir_arr[i + 1]->is_block_begin = true;
            }
        } else if (prefix("IF", ir->ir)) {
            set_block_by_label(ir->ir + get_blank_index(ir->ir, 5));
            if (i + 1 < ir_count) {
                ir_arr[i + 1]->is_block_begin = true;
            }
            // no arg function call
        } else if (contain_CALL(ir->ir)) {
            ir_arr[i]->is_block_begin = true;
        }
    }
    for (int i = 0; i < ir_count; i++) {
        if (i != 0 && ir_arr[i]->is_block_begin) {
            ir_arr[i - 1]->is_block_end = true;
        }
        if (i == ir_count - 1) {
            ir_arr[i]->is_block_end = true;
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
        reg_arr[i] = malloc(sizeof(Register));
        reg_arr[i]->reg_no = i;
        reg_arr[i]->name = malloc(sizeof(char) * 5);
        reg_arr[i]->var = NULL;
        reg_arr[i]->is_free = true;
        curr_reg = reg_arr[i];
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
            sprintf(curr_reg->name, "%s%d", "t", i - 16);
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

bool no_next_use(char* var_name, size_t ir_no) {
    if (ir_no + 1 >= ir_count) return false;
    return get_var(var_name)->live_analysis[ir_no + 1]->next_use == __UINTMAX_MAX__;
}
// return frame size of a function
size_t get_func_framesize(char* name) {
    Function* curr = func_list->next;
    while (curr != func_list) {
        if (!strcmp(curr->name, name)) {
            return curr->frame_size;
        }
        curr = curr->next;
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

// calculate function framesize and save varaible offset
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
            size_t frame_size = 4 * (1 + 1 + 8);
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
                    set_var_offset(token, frame_size);
                    frame_size += 4;
                } else if (!strcmp("DEC", token)) {
                    // DEC iv0 [size]
                    token = strtok(NULL, " ");
                    token = strtok(NULL, " ");
                    size_t size = atoi(token);
                    set_var_offset(token, frame_size);
                    frame_size += size;
                } else if (!strcmp("READ", token)) {
                    token = strtok(NULL, " ");
                    TempVar* var = get_var(token);
                    // first time
                    if (var->fp_offset == 0) {
                        set_var_offset(token, frame_size);
                        frame_size += 4;
                    }
                } else {
                    // iv0 := ...
                    if (token[0] == 'i') {
                        TempVar* var = get_var(token);
                        // first time
                        if (var->fp_offset == 0) {
                            set_var_offset(token, frame_size);
                            frame_size += 4;
                        }
                    }
                }
            }
            new_func->frame_size = frame_size;
        }
    }
}

void spill(Register* reg) {
    TempVar* var = reg->var;
    if (var == NULL) return;
    char code[max_object_code_len];
    // spill reg value into memory
    sprintf(code, "sw, $%s, -%lu($fp)", reg->name, var->fp_offset + 4);
    add_last_object_code(code);
    var->reg = NULL;
    reg->var = NULL;
    reg->is_free = true;
}

void free_reg(Register* reg) {
    reg->is_free = true;
}

Register* allocate(TempVar* var, size_t ir_no) {
    Register* res = NULL;
    // if (var->reg != NULL) {
    //     spill(var->reg);
    // }
    for (int i = 8; i <= 23; i++) {
        Register* curr = reg_arr[i];
        if (curr->is_free) {
            spill(curr);
            curr->is_free = false;
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
    res->is_free = false;
    res->var = var;
    var->reg = res;
    return res;
}

Register* allocate_by_name(char* var, size_t ir_no) {
    return allocate(get_var(var), ir_no);
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
        sprintf(code, "lw $%s, -%lu($fp)", res->name, curr_var->fp_offset + 4);
        add_last_object_code(code);
    }
    return res;
}

Register* ensure_var_without_lw(char* var_name, size_t ir_no) {
    Register* res;
    size_t var_no = atoi(var_name + 2);
    TempVar* curr_var = var_arr[var_no];
    if (curr_var->reg != NULL) {
        res = curr_var->reg;
    } else {
        res = allocate(curr_var, ir_no);
    }
    return res;
}

void spill_all_regs() {
    for (int i = 8; i <= 23; i++) {
        if (reg_arr[i]->var != NULL) {
            spill(reg_arr[i]);
        }
    }
}

bool is_param_code(const int ir_no) {
    IR* ir = ir_arr[ir_no];
    char* ir_code = ir->ir;
    if (prefix("PARAM", ir_code))
        return true;
    else
        return false;
}

int count_params(const int ir_no) {
    int param_num = 0;
    int curr_ir_no = ir_no;
    while (is_param_code(curr_ir_no)) {
        param_num++;
        curr_ir_no++;
    }

    return param_num;
}

void gen_func_code(char* func_name, int ir_no) {
    // clear all regs
    for (int i = 8; i <= 23; i++) {
        TempVar* var = reg_arr[i]->var;
        reg_arr[i]->var = NULL;
        reg_arr[i]->is_free = true;
        if (var != NULL) {
            var->reg = NULL;
        }
    }

    char ret[2] = "";
    add_last_object_code(ret);
    char code[max_object_code_len] = {0};
    snprintf(code, max_object_code_len, "%s:", func_name);
    add_last_object_code(code);

    int frameSize = get_func_framesize(func_name);
    current_func_frameSize = frameSize;
    sprintf(code, "subu $sp, $sp, %d", frameSize);
    add_last_object_code(code);
    sprintf(code, "sw $ra, %d($sp)", frameSize - 4);
    add_last_object_code(code);
    sprintf(code, "sw $fp, %d($sp)", frameSize - 8);
    add_last_object_code(code);
    sprintf(code, "addi $fp, $sp, %d", frameSize);
    add_last_object_code(code);

    // TODO: save s0~s7

    int param_num = count_params(ir_no + 1);
    for (int i = 0; i < param_num && i < 4; i++) {
        sprintf(code, "sw $a%d, -%d($fp)", i, 44 + i * 4);
        add_last_object_code(code);
    }
    for (int i = 5; i <= param_num; i++) {
        sprintf(code, "lw $v0, %d($sp)", frameSize + 4 * (i - 5));
        add_last_object_code(code);
        sprintf(code, "sw $v0, -%d($fp)", 44 + (i - 1) * 4);
        add_last_object_code(code);
    }
}

void gen_label_code(char* label_name) {
    char ret[2] = "";
    add_last_object_code(ret);
    char code[max_object_code_len] = {0};
    snprintf(code, max_object_code_len, "%s:", label_name);
    add_last_object_code(code);
}

void gen_goto_code(char* label_name) {
    char code[max_object_code_len] = {0};
    sprintf(code, "j %s", label_name);
    add_last_object_code(code);
}

void gen_return_code(char* var, size_t ir_no) {
    char code[max_object_code_len] = {0};
    char code2[max_object_code_len] = {0};
    if (*var == '#') {
        sprintf(code, "li $v0, %s", var + 1);
    } else {
        Register* reg = ensure_var(var, ir_no);
        sprintf(code, "move $v0, $%s", reg->name);
    }
    add_last_object_code(code);

    // TODO: restore s0~s7

    assert(current_func_frameSize >= 0);
    sprintf(code, "lw $ra, %d($sp)", current_func_frameSize - 4);
    add_last_object_code(code);
    sprintf(code, "lw $fp, %d($sp)", current_func_frameSize - 8);
    add_last_object_code(code);
    sprintf(code, "addi $sp, $sp, %d", current_func_frameSize);
    add_last_object_code(code);

    sprintf(code2, "jr $ra");
    add_last_object_code(code2);
}

void gen_read_code(char* var, size_t ir_no) {
    char code[max_object_code_len] = {0};
    char code2[max_object_code_len] = {0};
    char code3[max_object_code_len] = {0};
    char code4[max_object_code_len] = {0};
    char code5[max_object_code_len] = {0};
    char code6[max_object_code_len] = {0};
    sprintf(code, "addi $sp, $sp, -4");
    sprintf(code2, "sw $ra, 0($sp)");
    sprintf(code3, "jal read");
    sprintf(code4, "lw $ra, 0($sp)");
    sprintf(code5, "addi $sp, $sp, 4");
    Register* reg = ensure_var(var, ir_no);
    sprintf(code6, "move $%s, $v0", reg->name);
    add_last_object_code(code);
    add_last_object_code(code2);
    add_last_object_code(code3);
    add_last_object_code(code4);
    add_last_object_code(code5);
    add_last_object_code(code6);
}

void gen_write_code(char* var, size_t ir_no) {
    char code[max_object_code_len] = {0};
    char code2[max_object_code_len] = {0};
    char code3[max_object_code_len] = {0};
    char code4[max_object_code_len] = {0};
    char code5[max_object_code_len] = {0};
    char code6[max_object_code_len] = {0};
    if (*var == '#') {
        sprintf(code, "li $a0, %s", var + 1);
    } else {
        Register* reg = ensure_var(var, ir_no);
        sprintf(code, "move $a0, $%s", reg->name);
    }
    sprintf(code2, "addi $sp, $sp, -4");
    sprintf(code3, "sw $ra, 0($sp)");
    sprintf(code4, "jal write");
    sprintf(code5, "lw $ra, 0($sp)");
    sprintf(code6, "addi $sp, $sp, 4");
    add_last_object_code(code);
    add_last_object_code(code2);
    add_last_object_code(code3);
    add_last_object_code(code4);
    add_last_object_code(code5);
    add_last_object_code(code6);
}

void gen_if_code(size_t ir_no) {
    Register* reg1;
    Register* reg2;
    char* token;
    char* op;
    char ir[max_single_ir_len] = {0};
    char code[max_object_code_len] = {0};
    strncpy(ir, ir_arr[ir_no]->ir, max_object_code_len);
    token = strtok(ir, " ");
    token = strtok(NULL, " ");
    // var1
    if (token[0] == '#') {
        reg1 = reg_arr[24]; // t8
        sprintf(code, "li $%s, %s", reg1->name, token + 1);
        add_last_object_code(code); 
    } else {
        reg1 = ensure_var(token, ir_no);
    }

    token = strtok(NULL, " ");
    op = token;
    token = strtok(NULL, " ");
    // var2
    if (token[0] == '#') {
        reg2 = reg_arr[25]; // t9
        sprintf(code, "li $%s, %s", reg2->name, token + 1);
        add_last_object_code(code); 
    } else {
        reg2 = ensure_var(token, ir_no);
    }

    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (!strcmp(op, "==")) {
        sprintf(code, "beq $%s, $%s, %s", reg1->name, reg2->name, token);
    } else if (!strcmp(op, "!=")) {
        sprintf(code, "bne $%s, $%s, %s", reg1->name, reg2->name, token);
    } else if (!strcmp(op, ">")) {
        sprintf(code, "bgt $%s, $%s, %s", reg1->name, reg2->name, token);
    } else if (!strcmp(op, "<")) {
        sprintf(code, "blt $%s, $%s, %s", reg1->name, reg2->name, token);
    } else if (!strcmp(op, ">=")) {
        sprintf(code, "bge $%s, $%s, %s", reg1->name, reg2->name, token);
    } else if (!strcmp(op, "<=")) {
        sprintf(code, "ble $%s, $%s, %s", reg1->name, reg2->name, token);
    }
    add_last_object_code(code);
}

void two_blanks_assign_code(char* var1, char* var2, size_t ir_no) {
    Register* reg1;
    Register* reg2;
    // x := y
    // x := #1
    // x := *y
    // *x = y
    if (var1[0] == '*') {
        char code[max_object_code_len];
        if (*var2 == '#') {
            reg2 = reg_arr[24];  // t8
            char code2[max_object_code_len];
            sprintf(code2, "li $%s, %s", reg2->name, var2 + 1);
            add_last_object_code(code2);
        } else {
            reg2 = ensure_var(var2, ir_no);
        }
        reg1 = ensure_var_without_lw(var1 + 1, ir_no);
        sprintf(code, "sw $%s, 0($%s)", reg2->name, reg1->name);
        add_last_object_code(code);
    } else {
        if (*var2 == '#') {
            // x := #1
            reg1 = ensure_var_without_lw(var1, ir_no);
            char code[max_object_code_len];
            sprintf(code, "li $%s, %s", reg1->name, var2 + 1);
            add_last_object_code(code);
        } else if (*var2 == '*') {
            // x := *y
            reg2 = ensure_var(var2 + 1, ir_no);
            reg1 = ensure_var_without_lw(var1, ir_no);
            // generate code
            char code[max_object_code_len];
            sprintf(code, "lw $%s, 0($%s)", reg1->name, reg2->name);
            add_last_object_code(code);
        } else if (*var2 == '&') {
            // x := &y
            reg1 = ensure_var_without_lw(var1, ir_no);
            TempVar* temp_var = get_var(var2 + 1);
            size_t offset = temp_var->fp_offset;
            char code[max_object_code_len];
            sprintf(code, "add $%s, $fp, -%lu", reg1->name, offset);
            add_last_object_code(code);
        } else {
            // x := y
            reg2 = ensure_var(var2, ir_no);
            if (no_next_use(var2, ir_no)) {
                free_reg(reg2);
            }
            reg1 = ensure_var_without_lw(var1, ir_no);
            char code[max_object_code_len];
            sprintf(code, "move $%s, $%s", reg1->name, reg2->name);
            add_last_object_code(code);
        }
    }
}

void three_blanks_assign_code() {
}

void four_blanks_assign_code(char* var1, char* var2, char* var3, char* op, size_t ir_no) {
    // TODO
    Register* reg1;
    Register* reg2;
    Register* reg3;
    // var1

    // var2
    if (*var2 == '#') {
        reg2 = reg_arr[24];  // t8
        char code[max_object_code_len];
        sprintf(code, "li $%s, %s", reg2->name, var2 + 1);
        add_last_object_code(code);
    } else if (*var2 == '*') {
        // not exist
    } else if (*var2 == '&') {
        reg2 = reg_arr[24];  // t8
        TempVar* temp_var = get_var(var2 + 1);
        size_t offset = temp_var->fp_offset;
        char code[max_object_code_len];
        sprintf(code, "add $%s, $fp, -%lu", reg2->name, offset);
        add_last_object_code(code);
    } else {
        reg2 = ensure_var(var2, ir_no);
    }

    // var3
    if (*var3 == '#') {
        reg3 = reg_arr[25];  // t9
        char code[max_object_code_len];
        sprintf(code, "li $%s, %s", reg3->name, var3 + 1);
        add_last_object_code(code);
    } else if (*var3 == '*') {
        // not exist
    } else if (*var3 == '&') {
        reg3 = reg_arr[25];  // t9
        TempVar* temp_var = get_var(var3 + 1);
        size_t offset = temp_var->fp_offset;
        char code[max_object_code_len];
        sprintf(code, "add $%s, $fp, -%lu", reg3->name, offset);
        add_last_object_code(code);
    } else {
        reg3 = ensure_var(var3, ir_no);
        if (no_next_use(var3, ir_no)) {
            free_reg(reg3);
        }
    }

    reg1 = ensure_var_without_lw(var1, ir_no);
    // all regs have been found or allocated
    char code[max_object_code_len];
    char code2[max_object_code_len];
    if (*op == '+') {
        sprintf(code, "add $%s, $%s, $%s", reg1->name, reg2->name, reg3->name);
        add_last_object_code(code);
    } else if (*op == '-') {
        sprintf(code, "sub $%s, $%s, $%s", reg1->name, reg2->name, reg3->name);
        add_last_object_code(code);
    } else if (*op == '*') {
        sprintf(code, "mul $%s, $%s, $%s", reg1->name, reg2->name, reg3->name);
        add_last_object_code(code);
    } else if (*op == '/') {
        sprintf(code, "div $%s, $%s", reg2->name, reg3->name);
        add_last_object_code(code);
        sprintf(code2, "mflo $%s", reg1->name);
        add_last_object_code(code2);
    }
}

void gen_assign_code(size_t ir_no) {
    char ir[max_single_ir_len];
    char* var1 = malloc(sizeof(char) * max_ir_var_len);
    char* var2 = malloc(sizeof(char) * max_ir_var_len);
    char* var3 = malloc(sizeof(char) * max_ir_var_len);
    char* op = malloc(sizeof(char) * 5);

    // get var names
    strncpy(ir, ir_arr[ir_no]->ir, max_single_ir_len);
    size_t blank_num = count_blanks(ir);
    char* token = strtok(ir, " ");
    strncpy(var1, token, max_ir_var_len);
    if (blank_num == 2) {
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        strncpy(var2, token, max_ir_var_len);
    } else if (blank_num == 4) {
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        strncpy(var2, token, max_ir_var_len);
        token = strtok(NULL, " ");
        strncpy(op, token, 5);
        token = strtok(NULL, " ");
        strncpy(var3, token, max_ir_var_len);
    }

    if (blank_num == 2) {
        two_blanks_assign_code(var1, var2, ir_no);
    } else if (blank_num == 3) {
        gen_call_code(ir_no);
    } else if (blank_num == 4) {
        four_blanks_assign_code(var1, var2, var3, op, ir_no);
    }
}

// generate object code
void gen_object_code() {
    IR* curr_ir;
    char* curr_ir_code;
    for (size_t ir_no = 0; ir_no < ir_count; ir_no++) {
        curr_ir = ir_arr[ir_no];
        curr_ir_code = curr_ir->ir;
        if (ir_arr[ir_no]->is_block_end && (prefix("GOTO", ir_arr[ir_no]->ir) || prefix("IF", ir_arr[ir_no]->ir))) {
            spill_all_regs();
        }
        char temp_ir[max_single_ir_len];
        strncpy(temp_ir, curr_ir_code, max_single_ir_len);
        char* token = strtok(temp_ir, " ");
        if (!strcmp("FUNCTION", token)) {
            token = strtok(NULL, " ");
            gen_func_code(token, ir_no);
        } else if (!strcmp("LABEL", token)) {
            token = strtok(NULL, " ");
            gen_label_code(token);
        } else if (!strcmp("GOTO", token)) {
            token = strtok(NULL, " ");
            gen_goto_code(token);
        } else if (prefix("RETURN", curr_ir_code)) {
            token = strtok(NULL, " ");
            gen_return_code(token, ir_no);
        } else if (prefix("READ", curr_ir_code)) {
            token = strtok(NULL, " ");
            gen_read_code(token, ir_no);
        } else if (prefix("WRITE", curr_ir_code)) {
            token = strtok(NULL, " ");
            gen_write_code(token, ir_no);
        } else if (prefix("IF", curr_ir_code)) {
            gen_if_code(ir_no);
        } else if (prefix("DEC", curr_ir_code)) {
            //
        } else if (prefix("ARG", curr_ir_code)) {
            int call_ir_no = gen_call_with_arg_code(ir_no);
            ir_no = call_ir_no;
        } else {
            // assign statement
            gen_assign_code(ir_no);
        }
        if (ir_arr[ir_no]->is_block_end && !prefix("GOTO", ir_arr[ir_no]->ir) && !prefix("RETURN", ir_arr[ir_no]->ir) && !prefix("IF", ir_arr[ir_no]->ir)) {
            spill_all_regs();
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
    current_func_frameSize = -1;
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
    ObjectCode* curr = object_code->next;
    printf("\n");
    while (curr != object_code) {
        printf("%s\n", curr->code);
        curr = curr->next;
    }

    // print all func frame size
    // Function* curr = func_list->next;
    // while (curr != NULL) {
    //     printf("%s %lu\n", curr->name, curr->frame_size);
    //     curr = curr->next;
    // }

    // print all basic block begin and end
    // for (int i = 0; i < ir_count; i++) {
    //     if (ir_arr[i]->is_block_begin) {
    //         printf("begin: %s\n", ir_arr[i]->ir);
    //     }
    //     if (ir_arr[i]->is_block_end) {
    //         printf("end: %s\n", ir_arr[i]->ir);
    //     }
    // }
}

enum IrType getIrType(char* ir) {
    if (prefix("LABEL", ir))
        return LABEL;
    else if (prefix("FUNCTION", ir))
        return FUNCTION;
    else if (prefix("GOTO", ir))
        return GOTO;
    else if (prefix("IF", ir))
        return IF;
    else if (prefix("RETURN", ir))
        return RETURN;
    else if (prefix("DEC", ir))
        return DEC;
    else if (prefix("ARG", ir))
        return ARG;
    else if (prefix("PARAM", ir))
        return PARAM;
    else if (prefix("READ", ir))
        return READ;
    else if (prefix("WRITE", ir))
        return WRITE;
    else
        return ASSIGN;
}

void gen_call_code(int call_no) {
    char obj_code[max_object_code_len];
    char temp_ir[max_single_ir_len];
    IR* curr_ir = ir_arr[call_no];
    char* curr_ir_code = curr_ir->ir;
    strncpy(temp_ir, curr_ir_code, max_single_ir_len);
    char* token = strtok(temp_ir, " ");
    char* return_var = token;
    // reg_arr[2]
    TempVar* temp_var = get_var(return_var);
    temp_var->reg = reg_arr[2];

    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, " ");
    }

    sprintf(obj_code, "jal %s", token);
    add_last_object_code(obj_code);
}

bool is_arg_code(const int ir_no) {
    IR* ir = ir_arr[ir_no];
    char* ir_code = ir->ir;
    if (prefix("ARG", ir_code))
        return true;
    else
        return false;
}

int count_args(const int ir_no) {
    int arg_num = 0;
    int curr_ir_no = ir_no;
    while (is_arg_code(curr_ir_no)) {
        arg_num++;
        curr_ir_no++;
    }

    return arg_num;
}

// return value points to the corresponding CALL stmt
// so that the next turn of the loop in line 722 whould
// be the stmt next to the CALL stmt.
// the CALL stmt is processed in this function
int gen_call_with_arg_code(const int const ir_no) {
    int arg_num = count_args(ir_no);
    char obj_code[max_object_code_len];

    // sub sp to create space for args_i (i > 4)
    if (arg_num > 4) {
        sprintf(obj_code, "subu $sp, $sp, %d", 4 * (arg_num - 4));
        add_last_object_code(obj_code);
    }

    for (int arg_no = 1; arg_no <= arg_num; arg_no++) {
        char temp_ir[max_single_ir_len];
        int curr_ir_no = ir_no + arg_num - arg_no;
        IR* curr_ir = ir_arr[curr_ir_no];
        char* curr_ir_code = curr_ir->ir;
        strncpy(temp_ir, curr_ir_code, max_single_ir_len);
        char* arg = strtok(temp_ir, " ");
        arg = strtok(NULL, " ");

        Register* reg = ensure_var(arg, curr_ir_no);
        char* arg_reg = reg->name;
        if (arg_no <= 4) {
            sprintf(obj_code, "move $a%d, $%s", arg_no - 1, arg_reg);
            add_last_object_code(obj_code);
        } else {
            sprintf(obj_code, "sw %s, %d($sp)", arg_reg, 4 * (arg_no - 5));
            add_last_object_code(obj_code);
        }
    }

    int call_no = ir_no + arg_num;
    gen_call_code(call_no);

    // recover from line 856
    if (arg_num > 4) {
        sprintf(obj_code, "addi $sp, $sp, %d", 4 * (arg_num - 4));
        add_last_object_code(obj_code);
    }

    return ir_no + arg_num;
}