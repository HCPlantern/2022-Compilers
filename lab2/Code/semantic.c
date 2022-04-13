#include "semantic.h"
#include "type.h"
#include "node.h"
#include "stack.h"

static const struct _Type T_BOOL = {BASIC, T_INT};
static const struct _Type T_UNDEF = {UNDEF, T_INT};  // WARNING: kind should be examined before u.basic.

// global variables that help sematic check.
// make sure that when the current checking process is done,
// the two vars below is set NULL.
// because whether they are NULL means different checking procedure.
static Type current_def_type;
static Node args_for_func_def;

/*
static inline union _constant {int i; float f;} cal(Node* father, Node* exp1, Node* exp2) {
    union _constant c = {0, 0.0};
    return c;
}
*/

static inline bool is_undef(Node* exp) {
    return exp->type.kind == UNDEF;
}

static inline bool is_int(Node* exp) {
    return exp->type.kind == BASIC && exp->type.u.basic == T_INT;
}

static inline bool is_float(Node* exp) {
    return exp->type.kind == BASIC && exp->type.u.basic == T_FLOAT;
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
    // TODO
}

void fun_dec_check(Node* funDec, Node* varList/*, bool isDef*/) {
    // TODO
}

void new_scope() {
    // TODO
}

void exit_scope() {
    // TODO
}

void return_type_check(Node* ret_exp) {

}

void condition_type_check(Node* condition_exp) {
    // illegal. currently no ignorance for errors happened in sub exp
    if (!is_int(condition_exp)) {
        // it seems that no error type matches this situation.
        printf("Error type ? at Line %d: Type conflict. condition exp is not int.\n", condition_exp->lineno);
    }
}

void dec_assign_check(Node* father, Node* varDec, Node* exp) {
    // TODO
}

// calculation

void binary_cal_check(Node* father, Node* exp1, Node* exp2) {
    // illegal.
    if (is_undef(exp1) || is_undef(exp2)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!type_equal(&(exp1->type), &(exp2->type))) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 7 at Line %d: Type conflict. binary op.\n", father->lineno);
        return;
    }

    // legal.
    if (exp1->is_constant && exp2->is_constant) { // only exps that consists of literals and ops could be constant.
        if (exp1->type.u.basic == T_INT) {
            set_val(father, exp1->type, true, 0, 0, NULL);
        }
        else if (exp1->type.u.basic == T_FLOAT) {
            set_val(father, exp1->type, true, 0, 0, NULL);
        }
        else {
            assert(0);
        }
        return;
    }

    set_val(father, exp1->type, false, 0, 0, NULL);
}

void assignment_check(Node* father, Node* lValue, Node* rValue) {
    // TODO
}

void logical_check(Node* father, Node* exp1, Node* exp2) {
    // illegal.
    if (is_undef(exp1) || is_undef(exp2)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!is_int(exp1) || !is_int(exp2)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 7 at Line %d: Type conflict. logical.\n", father->lineno);
        return;
    }

    // legal.
    set_val(father, T_BOOL, false, 0, 0, NULL);
}

void relop_check(Node* father, Node* exp1, Node* exp2) {
    // illegal.
    if (is_undef(exp1) || is_undef(exp2)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!((is_int(exp1) || is_float(exp1)) && type_equal(&(exp1->type), &(exp2->type)))) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 7 at Line %d: Type conflict. relop.\n", father->lineno);
        return;
    }

    // legal.
    set_val(father, T_BOOL, false, 0, 0, NULL);
}

void parathese_check(Node* father, Node* inner_exp) {
    // illegal.
    if (is_undef(inner_exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }

    // legal.
    set_val(father, inner_exp->type, false, 0, 0, inner_exp->corresponding_field);
}

void minus_check(Node* father, Node* exp) {
    // illegal.
    if (is_undef(exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!is_int(exp) && !is_float(exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 7 at Line %d: Type conflict. minus.\n", father->lineno);
        return;
    }

    // legal.
    set_val(father, exp->type, false, 0, 0, NULL);
}

void not_check(Node* father, Node* exp) {
    // illegal.
    if (is_undef(exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!is_int(exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 7 at Line %d: Type conflict. not.\n", father->lineno);
        return;
    }

    // legal.
    set_val(father, T_BOOL, false, 0, 0, NULL);
}

// void tilde_check(Node* father, Node* exp);   // ~

void func_call_check(Node* father, Node* func, Node* args) {
    // TODO
}

void array_check(Node* father, Node* array, Node* index) {
    // TODO
}

// the field_name here is just a ID, so it should have no type imformation.
void field_access_check(Node* father, Node* base, Node* field_name) {
    // illegal.
    if (is_undef(base)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        // return;
    }
    
    if (base->type.kind != STRUCTURE) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 13 at Line %d: Type of base is not struct.\n", father->lineno);
        // return;
    }

    FieldList fields = base->type.u.structure;
    while (fields != NULL) {
        if (!strcmp(fields->name, field_name->data.text)) {
            // successful to find the field in the struct type.
            set_val(father, *(fields->type), false, 0, 0, fields);
            return;
        }

        fields = fields->next;
    }

    // in the struct no field matchs the field_name.
    set_val(father, T_UNDEF, false, 0, 0, NULL);
    printf("Error type 14 at Line %d: Undefined field.\n", father->lineno);
    return;
}

// notice that due to the LALR syntax, only when reduce according exp -> id happens,
// id_check would be called. (i.e. lValue -> EXP DOT ID or EXP -> EXP DOT ID would not
// call this function.)
// and EXP -> ID must be a variable for that in cmm function name could not be a exp.
void id_check(Node* father, Node* id) {
    FieldList variable = find_any_in_stack(id->data.text);

    if (variable == NULL || !variable->is_var) {
        // no variable found in the whole stack.
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 1 at Line %d: Undefined variable.\n", father->lineno);
        return;
    }

    // legal.
    set_val(father, *(variable->type), false, 0, 0, variable);
    return;
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