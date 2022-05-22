#include "ir_generator.h"

#include "ir.h"
#include "stack.h"

extern bool is_in_compst;
extern bool is_in_struct;
extern Type arg_type;

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
    // for primitive type, check if the last ir's lValue is exp->var_in_ir
    // if true, just modify the last ir,
    // else gen a new ir.
    // same optimization could be applied to READ

    father->is_bool = false;
    char buf[max_single_ir_len];
    if (exp->type.kind == BASIC) {
        if (exp->is_constant) {
            set_int_const(father, exp->constant.i);
            sprintf(buf, "%s := #%d", lValue->var_in_ir, exp->constant.i);
            add_last_ir(buf);
            return;
        } else if (prefix(exp->var_in_ir, ir_list->prev->ir) && lValue->var_in_ir[0] != '*') {
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
        sprintf(buf, "%s := %s + #%ld", lValue_ptr_var, lValue->var_in_ir, lValue->addr_offset + i);
        add_last_ir(buf);

        char* rValue_ptr_var = get_temp_var(0)->name;
        sprintf(buf, "%s := %s + #%ld", rValue_ptr_var, exp->var_in_ir, exp->addr_offset + i);
        add_last_ir(buf);

        sprintf(buf, "*%s := *%s", lValue_ptr_var, rValue_ptr_var);
        add_last_ir(buf);
    }
}

// char* relop_negative(char* relop) {
//     char* res = malloc(sizeof(char) * 3);
//     if (!strcmp(relop, "==")) {
//         strcpy(res, "!=");
//     } else if (!strcmp(relop, "!=")) {
//         strcpy(res, "==");
//     } else if (!strcmp(relop, ">")) {
//         strcpy(res, "<=");
//     } else if (!strcmp(relop, "<")) {
//         strcpy(res, ">=");
//     } else if (!strcmp(relop, ">=")) {
//         strcpy(res, "<");
//     } else if (!strcmp(relop, "<=")) {
//         strcpy(res, ">");
//     }
//     return res;
// }

void not_gen(Node* father, Node* exp) {
    // assert(is_in_cond);
    father->is_bool = true;
    father->true_list = exp->false_list;
    father->false_list = exp->true_list;
}

void and_gen(Node* father, Node* exp1, Node* M, Node* exp2) {
    // assert(is_in_cond);
    father->is_bool = true;
    backPatch(exp1->true_list, M, false);
    father->true_list = exp2->true_list;
    father->false_list = merge(exp1->false_list, exp2->false_list);
}

void or_gen(Node* father, Node* exp1, Node* M, Node* exp2) {
    // assert(is_in_cond);
    father->is_bool = true;
    backPatch(exp1->false_list, M, false);
    father->true_list = merge(exp1->true_list, exp2->true_list);
    father->false_list = exp2->false_list;
}

void relop_gen(Node* father, Node* exp1, Node* relop, Node* exp2) {
    // assert(is_in_cond);
    father->is_bool = true;
    char ir[max_single_ir_len];
    // char* relop_neg = relop_negative(relop->data.text);
    char* relop_neg = relop->data.text;
    if (!exp1->is_constant && !exp2->is_constant) {
        sprintf(ir, "IF %s %s %s GOTO", exp1->var_in_ir, relop_neg, exp2->var_in_ir);
    } else if (exp1->is_constant && !exp2->is_constant) {
        sprintf(ir, "IF #%d %s %s GOTO", exp1->constant.i, relop_neg, exp2->var_in_ir);
    } else if (!exp1->is_constant && exp2->is_constant) {
        sprintf(ir, "IF %s %s #%d GOTO", exp1->var_in_ir, relop_neg, exp2->constant.i);
    } else {
        sprintf(ir, "IF #%d %s #%d GOTO", exp1->constant.i, relop_neg, exp2->constant.i);
    }
    add_last_ir(ir);
    father->true_list = makeList(ir_list->prev);
    add_last_ir("GOTO");
    father->false_list = makeList(ir_list->prev);
}

void plus_gen(Node* father, Node* exp1, Node* exp2) {
    father->is_bool = false;
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
    father->is_bool = false;
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
    father->is_bool = false;
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

static int py_div(int a, int b) {
    if (a < 0)
        if (b < 0)
            return -a / -b;
        else
            return -(-a / b) - (-a % b != 0 ? 1 : 0);
    else if (b < 0)
        return -(a / -b) - (a % -b != 0 ? 1 : 0);
    else
        return a / b;
}

void div_gen(Node* father, Node* exp1, Node* exp2) {
    father->is_bool = false;
    if (exp1->is_constant && exp2->is_constant) {
        father->is_constant = true;
        if (exp1->type.u.basic == T_INT) {
            father->constant.i = py_div(exp1->constant.i, exp2->constant.i);
        } else {
            father->constant.f = py_div(exp1->constant.f, exp2->constant.f);
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
    father->is_bool = false;
    if (exp->is_constant) {
        set_int_const(father, -(exp->constant.i));
    } else {
        father->is_constant = false;
        char* temp_var = get_temp_var(0)->name;
        sprintf(father->var_in_ir, "%s", temp_var);
        char buf[max_single_ir_len];
        sprintf(buf, "%s := #0 - %s", father->var_in_ir, exp->var_in_ir);
        add_last_ir(buf);
    }
}

void parentheses_reduce(Node* father, Node* exp) {
    father->is_bool = exp->is_bool;
    if (exp->is_constant) {
        father->is_constant = true;
        father->constant.i = exp->constant.i;
    } else {
        father->is_constant = false;
        father->addr_offset = 0;
        strncpy(father->var_in_ir, exp->var_in_ir, 10);
    }

    if (exp->is_bool) {
        father->true_list = exp->true_list;
        father->false_list = exp->false_list;
    }
}

// void tilde_gen(Node* exp); // vm does not support tilde.

// special process for read() and write(x)
void func_call_gen(Node* father, Node* id, Node* args) {
    father->is_bool = false;
    FieldList fieldlist = find_any_in_stack(id->data.text);
    Type type = fieldlist->type;
    // handle function "write"
    if (!strcmp(id->data.text, "write")) {
        assert(type->u.function.arg_len == 1);
        Node* exp = args->child;
        char* ir_var = exp->var_in_ir;
        char ir[max_single_ir_len];
        if (exp->is_constant) {
            if (exp->type.u.basic == T_INT) {
                sprintf(ir, "WRITE #%d", exp->constant.i);
            } else if (exp->type.u.basic == T_FLOAT) {
                sprintf(ir, "WRITE #%f", exp->constant.f);
            }
            add_last_ir(ir);
        } else {
            sprintf(ir, "WRITE %s", exp->var_in_ir);
            add_last_ir(ir);
        }
        return;
    }

    if (args != NULL) {
        int arg_len = type->u.function.arg_len;
        Node* exps[arg_len];
        Node* exp = args->child;
        for (int i = 0; i < arg_len - 1; i++) {
            exps[i] = exp;
            exp = exp->sibling->sibling->child;
        }
        exps[arg_len - 1] = exp;
        for (int i = arg_len - 1; i >= 0; i--) {
            char ir[max_single_ir_len];
            if (!exps[i]->is_constant) {
                // if (exps[i]->type.kind == BASIC) {
                sprintf(ir, "ARG %s", exps[i]->var_in_ir);
                // } else {
                //     assert(exps[i]->type.kind == STRUCTURE || exps[i]->type.kind == ARRAY);
                //     assert(exps[i]->var_in_ir[0] == '&');
                //     sprintf(ir, "ARG %s", exps[i]->var_in_ir + 1);
                // }
            } else {
                sprintf(ir, "ARG #%d", exps[i]->constant.i);
            }
            add_last_ir(ir);
        }
    }
    father->is_constant = false;
    strncpy(father->var_in_ir, get_temp_var(0)->name, 10);
    char ir[max_single_ir_len];
    if (!strcmp(id->data.text, "read")) {
        sprintf(ir, "READ %s", father->var_in_ir);
    } else {
        sprintf(ir, "%s := CALL %s", father->var_in_ir, fieldlist->name);
    }
    add_last_ir(ir);
}

void array_store_gen(Node* father, Node* array, Node* index) {
    // TODO
}

void array_access_gen(Node* father, Node* array, Node* index) {
    father->is_bool = false;
    size_t element_size = get_type_size(&(father->type));

    char buf[max_single_ir_len];
    if (index->is_constant) {
        father->addr_offset = array->addr_offset + element_size * index->constant.i;
        char* addr_var = get_temp_var(0)->name;
        if (father->type.kind == BASIC) {
            /*
            if (father->addr_offset == 0) {
                sprintf(father->var_in_ir, "*%s", addr_var);
            } else {
                sprintf(buf, "%s := %s + #%ld", addr_var, array->var_in_ir, father->addr_offset);
                sprintf(father->var_in_ir, "*%s", addr_var);
            }
            */
            if (father->addr_offset == 0) {
                sprintf(buf, "%s := %s", addr_var, array->var_in_ir);
                add_last_ir(buf);
                sprintf(father->var_in_ir, "*%s", addr_var);
            } else {
                sprintf(buf, "%s := %s + #%ld", addr_var, array->var_in_ir, father->addr_offset);
                add_last_ir(buf);
                sprintf(father->var_in_ir, "*%s", addr_var);
            }
        } else {
            strncpy(father->var_in_ir, addr_var, max_ir_var_len);
        }
    } else {
        father->addr_offset = 0;

        char* current_offset = get_temp_var(0)->name;
        sprintf(buf, "%s := %s * #%ld", current_offset, index->var_in_ir, element_size);
        add_last_ir(buf);

        char* total_offset = get_temp_var(0)->name;
        sprintf(buf, "%s := #%ld + %s", total_offset, array->addr_offset, current_offset);
        add_last_ir(buf);

        char* addr_var = get_temp_var(0)->name;
        sprintf(buf, "%s := %s + %s", addr_var, array->var_in_ir, total_offset);
        add_last_ir(buf);

        if (father->type.kind == BASIC) {
            sprintf(father->var_in_ir, "*%s", addr_var);
        } else {
            sprintf(father->var_in_ir, "%s", addr_var);
        }
    }
}

void field_access_gen(Node* father, Node* base, Node* field) {
    // below is the implementation without optimization.
    /*
    char* field_addr_var = get_temp_var(0)->name;
    int offset = get_struct_field_offset(&(base->type), field->data.text);

    char buf[100];
    sprintf(buf, "%s := %s + #%d", field_addr_var, base->var_in_ir, offset);
    add_last_ir(buf);

    if (father->type.kind == BASIC) {
        char star_and_addr_var[max_ir_var_len];
        sprintf(star_and_addr_var, "*%s", field_addr_var);
        strncpy(father->var_in_ir, star_and_addr_var, max_ir_var_len);
    } else {
        strncpy(father->var_in_ir, field_addr_var, max_ir_var_len);
    }
    */

    // below is the implementation with optimization.
    ///*
    father->is_bool = false;
    int offset = get_struct_field_offset(&(base->type), field->data.text);

    // printf("%d\n", offset); // offset is always -1.

    father->addr_offset = base->addr_offset + offset;
    char buf[100];
    if (father->type.kind != BASIC) {
        strncpy(father->var_in_ir, base->var_in_ir, max_ir_var_len);
    } else {
        char* field_addr_var = get_temp_var(0)->name;
        if (father->addr_offset == 0) {
            sprintf(buf, "%s := %s", field_addr_var, base->var_in_ir);
            add_last_ir(buf);

            sprintf(father->var_in_ir, "*%s", field_addr_var);
        } else {
            sprintf(buf, "%s := %s + #%ld", field_addr_var, base->var_in_ir, father->addr_offset);
            add_last_ir(buf);

            sprintf(father->var_in_ir, "*%s", field_addr_var);
        }
    }
    //*/
}

void field_load_gen(Node* father, Node* base, Node* field) {
    // TODO
}

void id_gen(Node* father, Node* id) {
    father->is_bool = false;
    char* ir_var = get_ir_var_by_name(id->data.text);
    father->is_constant = false;
    father->addr_offset = 0;
    strncpy(father->var_in_ir, ir_var, 10);
    // if (is_in_cond) {
    //     char ir[max_single_ir_len];
    //     sprintf(ir, "IF %s == #0 GOTO", ir_var);
    //     add_last_ir(ir);
    //     father->false_list = makeList(ir_list->prev);
    //     father->true_list = NULL;
    // }
}

void int_gen(Node* father, Node* int_literal) {
    father->is_bool = false;
    set_int_const(father, int_literal->data.i);
    // if (is_in_cond) {
    //     char ir[max_single_ir_len];
    //     sprintf(ir, "IF #%d == #0 GOTO", int_literal->data.i);
    //     add_last_ir(ir);
    //     father->false_list = makeList(ir_list->prev);
    //     father->true_list = NULL;
    // }
}

void float_gen(Node* father, Node* float_literal) {
    father->is_bool = false;
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
        // vardec->addr_offset = -1;
        return;
    }
    // kind is structure and array
    // vardec->addr_offset = 0;
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

void param_dec_gen(Type arg_type) {
    if (arg_type == NULL) return;
    char* prev_ir = ir_list->prev->ir;
    bool has_printed = !prefix("FUNCTION", prev_ir) && !prefix("PARAM", prev_ir);
    if (has_printed) return;
    FieldList args = arg_type->u.function.args;
    for (int i = 0; i < arg_type->u.function.arg_len; i++) {
        char* ir_var = get_ir_var_by_field(args);
        char ir[100];
        if (ir_var[0] == '&') {
            assert(ir_var[0] == '&');
            sprintf(ir, "PARAM %s", ir_var + 1);
            // remove '&' for all params
            char* ir_var2 = malloc(sizeof(char) * 10);
            sprintf(ir_var2, "%s", ir_var + 1);
            args->ir_var = ir_var2;
            free(ir_var);
        } else {
            sprintf(ir, "PARAM %s", ir_var);
        }
        add_last_ir(ir);
        args = args->next;
    }
    arg_type = NULL;
}

void M_gen(Node* node) {
    /*
    node->label = new_label();
    node->prev_ir = ir_list->prev;
    node->backPatched = false;
    */
    node->prev_ir = ir_list->prev;
    if (node->prev_ir->label_next == NULL) {
        node->prev_ir->label_next = new_label();
    }
    node->label = node->prev_ir->label_next;
}

void N_gen(Node* node) {
    add_last_ir("GOTO");
    node->next_list = makeList(ir_list->prev);
}

void if_gen(Node* father, Node* cond_exp, Node* M, Node* stmt) {
    backPatch(cond_exp->true_list, M, false);
    father->next_list = merge(cond_exp->false_list, stmt->next_list);
}

void if_else_gen(Node* father, Node* cond_exp, Node* M1, Node* true_stmt, Node* N, Node* M2, Node* false_stmt) {
    backPatch(cond_exp->true_list, M1, false);
    backPatch(cond_exp->false_list, M2, false);
    IRLinkedList* temp = merge(true_stmt->next_list, N->next_list);
    father->next_list = merge(temp, false_stmt->next_list);
}

void while_gen(Node* father, Node* M1, Node* cond_exp, Node* M2, Node* stmt) {
    // printf("%s\n", M1->prev_ir->ir);
    // printf("%s\n", M1->label);
    //

    // printf("%s\n", M1->prev_ir->ir);
    // printf("%d\n", M1->prev_ir->label_printed);
    // printf("%s\n", M1->label);
    backPatch(stmt->next_list, M1, true);
    // printf("%d\n", M1->prev_ir->label_printed);

    // backPatch(stmt->next_list, M1);
    backPatch(cond_exp->true_list, M2, false);
    father->next_list = cond_exp->false_list;
    char buf[max_single_ir_len];
    sprintf(buf, "GOTO %s", M1->label);
    add_last_ir(buf);
}

void exp_for_if_gen(Node* exp) {
    // TODO
    if (exp->child->sibling != NULL && 0 != strcmp(">", exp->child->sibling->id) && 0 != strcmp("<", exp->child->sibling->id) && 0 != strcmp("!=", exp->child->sibling->id) && 0 != strcmp("<=", exp->child->sibling->id) && 0 != strcmp(">=", exp->child->sibling->id) && 0 != strcmp("==", exp->child->sibling->id) && 0 == strcmp("ID", exp->child->id)) {
        char ir[max_single_ir_len];
        sprintf(ir, "IF %s != #0 GOTO", exp->var_in_ir);
        add_last_ir(ir);
        exp->true_list = makeList(ir_list->prev);
        add_last_ir("GOTO");
        exp->false_list = makeList(ir_list->prev);
    }
}

void trans_bool_to_value_gen(Node* exp) {
    if (!exp->is_bool) {
        return;
    }
    exp->is_bool = false;
    char* temp_var = get_temp_var(0)->name;
    strncpy(exp->var_in_ir, temp_var, 10);

    Node* M_true = new_node("Epsilon");
    M_gen(M_true);
    backPatch(exp->true_list, M_true, false);
    char ir_true[max_single_ir_len];
    sprintf(ir_true, "%s := #1", temp_var);

    Node* M_false = new_node("Epsilon");
    M_gen(M_false);
    backPatch(exp->false_list, M_false, false);
    char ir_false[max_single_ir_len];
    sprintf(ir_false, "%s := #0", temp_var);
}

void trans_value_to_bool_gen(Node* exp) {
    if (exp->is_bool) {
        return;
    }
    exp->is_bool = true;
    char ir[max_single_ir_len];
    if (exp->is_constant) {
        sprintf(ir, "IF #%d != #0 GOTO", exp->constant.i);
    } else if (!exp->is_constant) {
        sprintf(ir, "IF %s != #0 GOTO", exp->data.text);
    }
    add_last_ir(ir);
    exp->true_list = makeList(ir_list->prev);
    add_last_ir("GOTO");
    exp->false_list = makeList(ir_list->prev);
}