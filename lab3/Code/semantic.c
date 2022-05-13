#include "semantic.h"

#include "node.h"
#include "semantic_check.h"
#include "stack.h"
#include "type.h"

extern Stack stack;
extern void add_last_ir(char* code);
FieldList check_VarDec(Node* specifier, Node* node, bool need_check_table);
static const struct _Type T_BOOL = {BASIC, T_INT};
static const struct _Type T_UNDEF = {UNDEF, T_INT};  // WARNING: kind should be examined before u.basic.

// global variables that help sematic check.
// make sure that when the current checking process is done,
// the two vars below is set NULL.
// because whether they are NULL means different checking procedure.
static Type current_def_type;

// semantic_check.c will use it too.
Type return_type;

// same functionality as current_def_type,
// just make it easier to call funcs in semantic_check.c
Node* current_specifier_node;

bool is_in_compst = false;

bool is_in_struct = false;
size_t struct_count = 0;
extern Type arg_type;
// add to table after the new scope is created when meeting left "{".
Node args_for_func_def;

Node* temp_ExtDef;

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

static inline Node* next_arg(Node* arg) {
    assert(!strcmp(arg->id, "Args"));
    if (arg->child->sibling == NULL) {
        // end of the args.
        return NULL;
    }

    return arg->child->sibling->sibling;
}

void var_dec_check(Node* varDec) {
    // printf("%d at : %d\n", is_in_struct, yylineno);
    assert(current_specifier_node != NULL);
    if (is_in_compst && !is_in_struct) {
        // printf("var_dec_check\n");
        check_VarDec(current_specifier_node, varDec, true);
    }
}

// check whether in struct
void if_in_struct(bool enter) {
    if (enter) {
        is_in_struct = true;
        struct_count++;
    } else {
        if (struct_count == 1) {
            is_in_struct = false;
        }
        struct_count--;
    }
}

void fun_dec_check(Node* funDec, Node* varList /*, bool isDef*/) {
    // TODO
}

void new_scope() {
    Table table = new_table();
    push(table);
}

void add_args_into_table() {
    if (arg_type != NULL) {
        FieldList args = arg_type->u.function.args;
        for (int i = 0; i < arg_type->u.function.arg_len; i++) {
            if (find_field(stack->tables[stack->top - 1], args->name) != NULL || find_struct_def_in_stack(args->name) != NULL) {
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", yylineno, args->name);
            }
            add_table_node(stack->tables[stack->top - 1], args);
            args = args->next;
        }
    }
}

void exit_scope() {
    pop();
}

// assume that when processing a func def compst, the func that is currently processing
// is stored at static (or should be global to cooperate with Deng's part?) variable
// return_type.
void return_type_check(Node* ret_exp) {
    assert(return_type != NULL);

    if (is_undef(ret_exp)) {
        return;
    }

    if (!type_equal(return_type, &(ret_exp->type))) {
        // printf("%d\n", ret_exp->type.kind);
        // printf("%d\n", return_type->kind);
        printf("Error type 8 at Line %d: Return type conflict.\n", ret_exp->lineno);
    }
}

void condition_type_check(Node* condition_exp) {
    // illegal. currently no ignorance for errors happened in sub exp
    if (is_undef(condition_exp)) {
        return;
    }

    if (!is_int(condition_exp)) {
        // it seems that no error type matches this situation.
        printf("Error type ? at Line %d: Type conflict. condition exp is not int.\n", condition_exp->lineno);
    }
}

void dec_assign_check(Node* father, Node* varDec, Node* exp) {
    assert(!strcmp(varDec->child->id, "ID"));

    FieldList var = find_field(stack->tables[stack->top - 1], varDec->child->data.text);
    if (var == NULL) {
        return;
    }
    if (is_undef(exp)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }

    if (!type_equal(var->type, &(exp->type))) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 5 at Line %d: Type conflict. Assignment (initialization).\n", father->lineno);
    }

    // do nothing when legal.
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
        printf("Error type 7 at Line %d: Type conflict. Binary op.\n", father->lineno);
        return;
    }

    // legal.
    /*
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
    */

    set_val(father, exp1->type, false, 0, 0, NULL);
}

// be ready to transfer to no token LValue version.
void assignment_check(Node* father, Node* lValue, Node* rValue) {
    // assert(!strcmp(lValue->id, "LValue"));

    // illegal.
    if (is_undef(lValue) || is_undef(rValue)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }
    if (!type_equal(&(lValue->type), &(rValue->type))) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 5 at Line %d: Type conflict. Assignment.\n", father->lineno);
        return;
    }

    // legal.
    /*
    if (rValue->is_constant) { // only exps that consists of literals and ops could be constant.
        if (rValue->type.u.basic == T_INT) {
            set_val(father, rValue->type, true, 0, 0, NULL);
        }
        else if (rValue->type.u.basic == T_FLOAT) {
            set_val(father, rValue->type, true, 0, 0, NULL);
        }
        else {
            assert(0);
        }
        return;
    }
    */

    set_val(father, rValue->type, false, 0, 0, NULL);
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

void parentheses_check(Node* father, Node* inner_exp) {
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
    assert(!strcmp(func->id, "ID"));
    FieldList f = find_any_in_stack(func->data.text);

    if (f == NULL) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 2 at Line %d: Function call before declaration.\n", father->lineno);
        return;
    }

    if (f->type->kind != FUNC) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 11 at Line %d: Call to non-function.\n", father->lineno);
        return;
    }

    FieldList parameters = f->type->u.function.args;
    int no = 1;
    bool legal = true;
    while (parameters != NULL && args != NULL) {
        if (!type_equal(parameters->type, &(args->child->type))) {
            legal = false;
            printf("Error type 9 at Line %d: Type unmatched at arg %d.\n", father->lineno, no);
        }

        parameters = parameters->next;
        args = next_arg(args);
        no++;
    }

    if (parameters != NULL || args != NULL) {
        legal = false;
        printf("Error type 9 at Line %d: Amount of args and params unmatched.\n", father->lineno);
    }

    if (!legal) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }

    // legal.
    set_val(father, *(f->type->u.function.return_type), false, 0, 0, NULL);
}

void array_check(Node* father, Node* array, Node* index) {
    if (is_undef(array) || is_undef(index)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }

    if (array->type.kind != ARRAY) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 10 at Line %d: Access non-array via index.\n", father->lineno);
        return;
    }

    if (!is_int(index)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 12 at Line %d: Index is not a integer.\n", father->lineno);
        return;
    }

    set_val(father, *(array->type.u.array.elem), false, 0, 0, NULL);
}

// the field_name here is just a ID, so it should have no type imformation.
void field_access_check(Node* father, Node* base, Node* field_name) {
    // illegal.
    if (is_undef(base)) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        return;
    }

    if (base->type.kind != STRUCTURE) {
        set_val(father, T_UNDEF, false, 0, 0, NULL);
        printf("Error type 13 at Line %d: Type of base is not struct.\n", father->lineno);
        return;
    }

    FieldList fields = base->type.u.structure->type->u.structure;
    while (fields != NULL) {
        if (!strcmp(fields->name, field_name->data.text)) {
            // successful to find the field in the struct type.
            set_val(father, *(fields->type), false, 0, 0, NULL);  // co_field cannot be described by a FieldList
            return;
        }

        fields = fields->next;
    }

    // in the struct no field matchs the field_name.
    set_val(father, T_UNDEF, false, 0, 0, NULL);
    printf("Error type 14 at Line %d: Undefined field \"%s\".\n", father->lineno, field_name->data.text);
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
        printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", father->lineno, id->data.text);
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