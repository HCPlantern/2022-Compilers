#include "ir_generator.h"

#include "ir.h"
#include "stack.h"

extern bool is_in_compst;
extern bool is_in_struct;

static inline void set_int_const(Node* node, int i) {
    node->is_constant = true;
    node->constant.i = i;
}

static inline void set_float_const(Node* node, float f) {
    node->is_constant = true;
    node->constant.f = f;
}

bool prefix(const char* pre, const char* str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

size_t indexOfAssignOp(char* ir) {
    for (size_t i = 0; i < strlen(ir) - 1; i++) {
        if (':' == ir[i] && '=' == ir[i + 1]) {
            return i;
        }
    }

    return -1;
}

void assign_gen(Node* father, Node* lValue, Node* exp) {
    // TODO
    // for primitive type, check if the last ir's lValue is exp->var_in_ir
    // if true, just modify the last ir,
    // else gen a new ir.

    char buf[max_single_ir_len];
    if (exp->type.kind == BASIC) {
        if (exp->is_constant) {
            set_int_const(father, exp->constant.i);
            sprintf(buf, "%s := #%d", lValue->var_in_ir, exp->constant.i);
            add_last_ir(buf);
            return;
        } else if (prefix(exp->var_in_ir, ir_list->prev->ir)) {
            father->is_constant = false;
            strncpy(father->var_in_ir, lValue->var_in_ir, 10);
            size_t colon_index = indexOfAssignOp(ir_list->prev->ir);
            assert(colon_index > 0);
            sprintf(buf, "%s %s", lValue->var_in_ir, ir_list->prev->ir + colon_index);  // WARNING: make sure that lValue has a var_in_ir.
            strncpy(ir_list->prev->ir, buf, max_single_ir_len);
            return;
        } else {
            father->is_constant = false;
            strncpy(father->var_in_ir, lValue->var_in_ir, 10);
            sprintf(buf, "%s := %s", lValue->var_in_ir, exp->var_in_ir);
            add_last_ir(buf);
            return;
        }
    }

    // lValue is array or struct
    int lValue_size = get_type_size(&(lValue->type));
    int exp_size = get_type_size(&(exp->type));
    int min_size = lValue_size < exp_size ? lValue_size : exp_size;

    for (int i = 0; i < min_size; i += 4) {
        char* lValue_ptr_var = get_temp_var(0)->name;
        sprintf(buf, "%s := %s + #%d", lValue_ptr_var, lValue->var_in_ir, i);
        add_last_ir(buf);

        char* rValue_ptr_var = get_temp_var(0)->name;
        sprintf(buf, "%s := %s + #%d", rValue_ptr_var, exp->var_in_ir, i);
        add_last_ir(buf);

        sprintf(buf, "*%s := *%s", lValue_ptr_var, rValue_ptr_var);
        add_last_ir(buf);
    }
}

void not_gen(Node* father, Node* exp) {
    // TODO
    if (exp->is_constant) {
        if (exp->type.u.basic == T_INT) {
            assert(exp->type.kind == BASIC);
            set_int_const(father, exp->constant.i ? 0 : 1);
        }
        if (exp->type.u.basic == T_FLOAT) {
            assert(exp->type.kind == BASIC);
            set_int_const(father, exp->constant.f ? 0 : 1);
        }
        return;
    }
    /*
    else {
        if (exp->type.u.basic == T_INT) {
            strncpy(father->var_in_ir, get_temp_var(0), 10);
        }
        else {
            strncpy(father->var_in_ir, get_temp_var(1), 10);
        }
    }
    */
}

void and_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
}

void or_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
}

void relop_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
}

void plus_gen(Node* father, Node* exp1, Node* exp2) {
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i + exp2->constant.i;
        } else {
            father->constant.f = exp1->constant.f + exp2->constant.f;
        }
        return;
    }

    father->is_constant = false;
    char ir[100];
    strncpy(father->var_in_ir, get_temp_var(0)->name, 10);
    if (exp1->is_constant) {
        sprintf(ir, "%s := #%d + %s",
                father->var_in_ir, exp1->constant.i, exp2->var_in_ir);
    } else if (exp2->is_constant) {
        sprintf(ir, "%s := %s + #%d",
                father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    } else {
        sprintf(ir, "%s := %s + %s",
                father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_ir(ir);
}

void minus_gen(Node* father, Node* exp1, Node* exp2) {
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i - exp2->constant.i;
        } else {
            father->constant.f = exp1->constant.f - exp2->constant.f;
        }
        return;
    }

    father->is_constant = false;
    char ir[100];
    strncpy(father->var_in_ir, get_temp_var(0)->name, 10);
    if (exp1->is_constant) {
        sprintf(ir, "%s := #%d - %s",
                father->var_in_ir, exp1->constant.i, exp2->var_in_ir);
    } else if (exp2->is_constant) {
        sprintf(ir, "%s := %s - #%d",
                father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    } else {
        sprintf(ir, "%s := %s - %s",
                father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_ir(ir);
}

void star_gen(Node* father, Node* exp1, Node* exp2) {
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i * exp2->constant.i;
        } else {
            father->constant.f = exp1->constant.f * exp2->constant.f;
        }
        return;
    }

    father->is_constant = false;
    char ir[100];
    strncpy(father->var_in_ir, get_temp_var(0)->name, 10);
    if (exp1->is_constant) {
        sprintf(ir, "%s := #%d * %s",
                father->var_in_ir, exp1->constant.i, exp2->var_in_ir);
    } else if (exp2->is_constant) {
        sprintf(ir, "%s := %s * #%d",
                father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    } else {
        sprintf(ir, "%s := %s * %s",
                father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_ir(ir);
}

void div_gen(Node* father, Node* exp1, Node* exp2) {
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i / exp2->constant.i;
        } else {
            father->constant.f = exp1->constant.f / exp2->constant.f;
        }
        return;
    }

    father->is_constant = false;
    char ir[100];
    strncpy(father->var_in_ir, get_temp_var(0)->name, 10);
    if (exp1->is_constant) {
        sprintf(ir, "%s := #%d / %s",
                father->var_in_ir, exp1->constant.i, exp2->var_in_ir);
    } else if (exp2->is_constant) {
        sprintf(ir, "%s := %s / #%d",
                father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    } else {
        sprintf(ir, "%s := %s / %s",
                father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_ir(ir);
}

void negative_gen(Node* father, Node* exp) {
    // TODO
}

void parentheses_reduce(Node* father, Node* exp) {
    if (exp->is_constant) {
        father->is_constant = true;
        father->constant.i = exp->constant.i;
        return;
    }

    father->is_constant = false;
    father->addr_offset = 0;
    strncpy(father->var_in_ir, exp->var_in_ir, 10);
}

// void tilde_gen(Node* exp); // vm does not support tilde.

// special process for read() and write(x)
void func_call_gen(Node* father, Node* id, Node* args) {
    // TODO
}

void array_store_gen(Node* father, Node* array, Node* index) {
    // TODO
}

void array_load_gen(Node* father, Node* array, Node* index) {
    // TODO
}

void field_store_gen(Node* father, Node* base, Node* field) {
    // TODO
}

void field_load_gen(Node* father, Node* base, Node* field) {
    // TODO
}

void id_gen(Node* father, Node* id) {
    char* ir_var = get_ir_var_by_name(id->data.text);
    father->is_constant = false;
    father->addr_offset = 0;
    strncpy(father->var_in_ir, ir_var, 10);
}

void int_gen(Node* father, Node* int_literal) {
    set_int_const(father, int_literal->data.i);
}

void float_gen(Node* father, Node* float_literal) {
    set_float_const(father, float_literal->data.f);
}

void var_dec_gen(Node* vardec) {
    if (is_in_struct) return;
    assert(is_in_compst);
    // in compst
    Node* temp = vardec;
    while (strcmp(temp->id, "ID") != 0) {
        temp = temp->child;
    }
    char* id = temp->data.text;
    FieldList fieldlist = find_any_in_stack(id);
    char* ir_var = get_ir_var_by_field(fieldlist);
    strcpy(vardec->var_in_ir, ir_var);
    // in compst func dec is impossible
    assert(fieldlist->type->kind != FUNC);
    if (fieldlist->type->kind == BASIC) {
        return;
    }
    // kind is structure and array
    size_t size = get_field_size(fieldlist);
    char ir[100];
    // start from index 1 of ir_var (ir_var[0] is '&')
    sprintf(ir, "DEC %s %ld", ir_var + 1, size);
    add_last_ir(ir);
}

void return_gen(Node* exp) {
    char buf[max_single_ir_len];
    if (exp->is_constant) {
        sprintf(buf, "RETURN #%d", exp->constant.i);
        add_last_ir(buf);
    } else {
        sprintf(buf, "RETURN %s", exp->var_in_ir);
        add_last_ir(buf);
    }
}

void func_dec_gen(Node* id) {
    char ir[max_single_ir_len];
    sprintf(ir, "FUNCTION %s :", id->data.text);
    add_last_ir(ir);
}

// void param_dec_gen(Type arg_type) {
//     FieldList args = arg_type->u.function.args;
//     for (int i = 0; i < arg_type->u.function.arg_len; i++) {
//         char* ir_var = get_ir_var_by_field(args);
//         char ir[max_single_ir_len];
//         sprintf(ir, "PARAM %s", ir_var);
//         add_last_ir(ir);
//         args = args->next;
//     }
// }