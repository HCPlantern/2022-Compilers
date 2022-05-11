#include "ir_generator.h"
#include "ir.h"

static inline void set_int_const(Node* node, int i) {
    node->is_constant = true;
    node->constant.i = i;
}

static inline void set_float_const(Node* node, float f) {
    node->is_constant = true;
    node->constant.f = f;
}

void assign_gen(Node* father, Node* lValue, Node* exp) {
    // TODO
    // check if the last ir's lValue is exp->var_in_ir
    // if true, just modify the last ir,
    // else gen a new ir.
}

void not_gen(Node* father, Node* exp) {
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
        }
        else {
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
    }
    else if (exp2->is_constant){
        sprintf(ir, "%s := %s + #%d",
            father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    }
    else {
        sprintf(ir, "%s := %s + %s",
            father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_code(ir);
}

void minus_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i - exp2->constant.i;
        }
        else {
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
    }
    else if (exp2->is_constant){
        sprintf(ir, "%s := %s - #%d",
            father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    }
    else {
        sprintf(ir, "%s := %s - %s",
            father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_code(ir);
}

void star_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = exp1->constant.i * exp2->constant.i;
        }
        else {
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
    }
    else if (exp2->is_constant){
        sprintf(ir, "%s := %s * #%d",
            father->var_in_ir, exp1->var_in_ir, exp2->constant.i);
    }
    else {
        sprintf(ir, "%s := %s * %s",
            father->var_in_ir, exp1->var_in_ir, exp2->var_in_ir);
    }
    add_last_code(ir);
}

void div_gen(Node* father, Node* exp1, Node* exp2) {
    // TODO
}

void negative_gen(Node* father, Node* exp) {
    // TODO
}

void parathesis_reduce(Node* father, Node* exp) {
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
