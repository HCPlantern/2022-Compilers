#include "semantic.h"
#include "type.h"
#include "node.h"

static const struct _Type T_BOOL = {BASIC, T_INT};
static const struct _Type T_UNDEF = {UNDEF, T_INT};

// global variables that help sematic check.
// make sure that when the current checking process is done,
// the two vars below is set NULL.
// because whether they are NULL means different checking procedure.
static Type current_def_type;
static Node args_for_func_def;

static inline bool is_undef(Node* exp) {
    return exp->type.kind == UNDEF;
}

static inline void set_val(Node* father, struct _Type t, bool is_constant, uint32_t i, float f, FieldList field) {
    father->type = t;
    father->corresponding_field = field;
    father->is_constant = is_constant;
    if (is_constant) {
        if (t.kind == T_INT) {
            father->constant.i = i;
        }
        if (t.kind == T_FLOAT) {
            father->constant.f = f;
        }
    }
}

void var_dec_check(Node* varDec) {

}

void fun_dec_check(Node* funDec, Node* varList/*, bool isDef*/) {

}

void new_scope() {

}

void exit_scope() {

}

void return_type_check(Node* ret_exp) {

}

void condition_type_check(Node* condition_exp) {

}

void dec_assign_check(Node* father, Node* varDec, Node* exp) {

}

// calculation

void binary_cal_check(Node* father, Node* exp1, Node* exp2) {
    if (is_undef(exp1) || is_undef(exp2)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!type_equal(&(exp1->type), &(exp2->type))) {
        
    }
}

void assignment_check(Node* father, Node* lValue, Node* rValue) {

}

void logical_check(Node* father, Node* exp1, Node* exp2) {

}

void relop_check(Node* father, Node* exp1, Node* exp2) {

}

void parathese_check(Node* father, Node* inner_exp) {

}

void minus_check(Node* father, Node* exp) {

}

void not_check(Node* father, Node* exp) {

}

// void tilde_check(Node* father, Node* exp);   // ~

void func_call_check(Node* father, Node* func, Node* args) {

}

void array_check(Node* father, Node* array, Node* index) {

}

void field_access_check(Node* father, Node* base, Node* field) {

}

void id_check(Node* father) {

}

void literal_check(Node* father) {
    Node* literal = father->child;
    bool is_int = !strcmp(literal->id, "INT");
    bool is_float = !strcmp(literal->id, "FLOAT");
    assert(is_int || is_float);
    
    // process for child itself and father exp. they are the same.
    literal->is_constant = true;
    father->is_constant = true;

    literal->type.kind = BASIC;
    father->type.kind = BASIC;

    if (is_int) {
        literal->type.u.basic = T_INT;
        father->type.u.basic = T_INT;

        literal->constant.i = literal->data.i;
        father->constant.i = literal->data.i;
    } else {
        literal->type.u.basic = T_FLOAT;
        father->type.u.basic = T_FLOAT;

        literal->constant.f = literal->data.f;
        father->constant.f = literal->data.f;
    }
}