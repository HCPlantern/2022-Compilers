
#include "type.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "stack.h"

#define curr_table (stack->tables[stack->top - 1])

extern Type arg_type;
extern Node* syntax_tree_root;
extern Stack stack;
extern FieldList check_VarDec(Node* specifier, Node* node, bool in_struct);

size_t anonymous_struct_count = 0;

FieldList init_fieldlist(char* name, Type type, FieldList next, bool is_var, int size, char* ir_var) {
    FieldList res = malloc(sizeof (struct _FieldList));
    res->name = name;
    res->type = type;
    res->next = next;
    res->is_var = is_var;
    res->size = size;
    res->ir_var = ir_var;
    return res;
}

// todo: only compare type. (in structural equivalence)
bool type_equal(Type type1, Type type2) {
    if (type1->kind == BASIC && type2->kind == BASIC) {
        return type1->u.basic == type2->u.basic;
    } else if (type1->kind == ARRAY && type2->kind == ARRAY) {
        return type_equal(type1->u.array.elem, type2->u.array.elem);
    } else if (type1->kind == STRUCTURE && type2->kind == STRUCTURE) {
        FieldList field1 = type1->u.structure;
        FieldList field2 = type2->u.structure;
        while (field1 != NULL && field2 != NULL) {
            if (!type_equal(field1->type, field2->type)) {
                return false;
            }
            field1 = field1->next;
            field2 = field2->next;
        }

        // only when one of the var field are NULL can reach here
        if (field1 == NULL && field2 == NULL) {
            return true;
        }

        // nums of fields not equal.
        return false;
    } else if (type1->kind == FUNC && type2->kind == FUNC) {
        if (type1->u.function.arg_len != type2->u.function.arg_len) {
            return false;
        }
        if (!type_equal(type1->u.function.return_type, type2->u.function.return_type)) {
            return false;
        }
        FieldList arg1 = type1->u.function.args;
        FieldList arg2 = type2->u.function.args;
        while (arg1 != NULL && arg2 != NULL) {
            if (!type_equal(arg1->type, arg2->type)) {
                return false;
            }
            arg1 = arg1->next;
            arg2 = arg2->next;
        }

        // only when one of the var field are NULL can reach here
        if (arg1 == NULL && arg2 == NULL) {
            return true;
        }

        // nums of fields not equal.
        return false;
    }

    // even the kinds do not match each other or one of which is UNDEF
    return false;
}

char* anonymous_struct_name() {
    char* struct_str = "struct";
    char* res = malloc(sizeof(char) * (10 + strlen(struct_str)));
    sprintf(res, "%lu%s", anonymous_struct_count, struct_str);
    anonymous_struct_count++;
    return res;
}

Type create_int_type() {
    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = BASIC;
    new_type->u.basic = T_INT;
    new_type->size = 4;
}

Type create_float_type() {
    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = BASIC;
    new_type->u.basic = T_FLOAT;
    new_type->size = 4;
}
Type create_basic_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "TYPE"));
    if (!strcmp(specifier->child->data.text, "int")) {
        return create_int_type();
    } else if (!strcmp(specifier->child->data.text, "float")) {
        return create_float_type();
    }
    return NULL;
}

// u.array.elem is null
Type create_array_type(int size) {
    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = ARRAY;
    new_type->u.array.elem = NULL;
    new_type->u.array.size = size;
    new_type->size = -1;
}

// if this is struct definiton, create struct type and return, create struct field
// if StructSpecifier -> STRUCT Tag, check this struct def from stack top to buttom, if find it return its type else return NULL
Type create_struct_type(Node* specifier) {
    Node* struct_specifier = specifier->child;
    assert(!strcmp(struct_specifier->id, "StructSpecifier"));
    if (struct_specifier->child->sibling->sibling != NULL) {
        // add this struct itself into table
        FieldList new_struct_field = create_struct_field_for_struct(struct_specifier);
        Type res = malloc(sizeof(struct _Type));
        res->kind = STRUCTURE;
        res->u.structure = new_struct_field;
        res->size = -1;
        return res;
    } else if (!strcmp(struct_specifier->child->sibling->id, "Tag")) {
        // find struct in all tables
        char* name = struct_specifier->child->sibling->child->data.text;
        FieldList field = find_struct_def_in_stack(name);
        if (field == NULL) {
            printf("Error type 17 at Line %d :Undefined structure \"%s\".\n", struct_specifier->lineno, name);
        } else {
            Type res = malloc(sizeof(struct _Type));
            res->kind = STRUCTURE;
            res->u.structure = field;
            res->size = -1;
            return res;
        }
    }
    return NULL;
}

Type create_func_type(Node* specifier, Node* fundec) {
    Type res = malloc(sizeof(struct _Type));
    res->kind = FUNC;
    res->size = -1;
    size_t arg_len = 0;
    // get return type
    Type return_type;
    FieldList next_field;
    res->u.function.args == NULL;
    if (!strcmp(specifier->child->id, "TYPE")) {
        return_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        return_type = create_struct_type(specifier);
    }
    res->u.function.return_type = return_type;
    // handle args
    if (fundec->child->sibling->sibling->sibling == NULL) {
        res->u.function.arg_len = 0;
    } else {
        Node* varlist = fundec->child->sibling->sibling;
        Node* paramdec = varlist->child;
        while (true) {
            FieldList arg = check_VarDec(paramdec->child, paramdec->child->sibling, false);

            // concat args field
            if (res->u.function.args == NULL) {
                res->u.function.args = arg;
                next_field = arg;
            } else {
                next_field->next = arg;
                next_field = arg;
            }
            arg_len++;
            if (paramdec->sibling == NULL)
                break;
            else {
                paramdec = paramdec->sibling->sibling->child;
            }
        }
    }
    res->u.function.arg_len = arg_len;
    // fundec->type = *res;
    arg_type = res;
    return res;
}

// // todo
// Type get_exp_type(Node* exp) {
//     Node* child = exp->child;
//     char* child_id = child->id;
//     if (!strcmp(child_id, "ID") && child->sibling == NULL) {
//         return find_any_in_stack(stack, child_id)->type;
//     } else if (!strcmp(child_id, "ID") && child->sibling != NULL) {
//         // todo exp is function call
//     } else if (!strcmp(child_id, "INT")) {
//         return create_int_type();
//     } else if (!strcmp(child_id, "FLOAT")) {
//         return create_float_type();
//     } else if (!strcmp(child_id, "LP")) {
//         return get_exp_type(child->sibling);
//     } else if (!strcmp(child_id, "MINUS")) {
//         Type type = get_exp_type(child->sibling);
//         if (type->kind != BASIC)
//             printf("Error type 7 at Line %d: Type mismatched for operands.", child->lineno);
//     } else if (!strcmp(child_id, "NOT")) {
//         Type type = get_exp_type(child->sibling);
//         if (type->kind != BASIC || type->u.basic != T_INT) {
//             printf("Error type 7 at Line %d: Type mismatched for operands.", child->lineno);
//         }
//     } else if (!strcmp(child_id, "Exp")) {

//     }
// }

FieldList create_basic_and_struct_field_for_var(char* name, Node* specifier) {
    Type new_type;
    if (!strcmp(specifier->child->id, "TYPE")) {
        new_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        new_type = create_struct_type(specifier);
    }
    FieldList field = init_fieldlist(name, new_type, NULL, true, -1, NULL);
    return field;
}

// ????????????????????? VarDec ????????????????????????
// ?????????????????? VarDec ?????? ??????????????? array field
FieldList create_array_field(Node* node, Node* specifier) {
    assert(!strcmp(node->id, "VarDec"));
    if (!strcmp(node->child->id, "ID")) {  // ????????? VarDec
        // printf("?????????\n");
        Type new_type = create_array_type(node->sibling->sibling->data.i);
        FieldList field = init_fieldlist(node->child->data.text, new_type, NULL, true, -1, NULL);
        return field;
    } else if (!strcmp(node->child->id, "VarDec")) {
        FieldList field;
        Type new_type;
        if (node->sibling == NULL || strcmp(node->sibling->id, "LB") != 0) {
            // ???????????? VarDec ???????????????????????? type
            // ???????????? basic ??? struct
            // printf("?????????\n");
            field = create_array_field(node->child, specifier);
            if (!strcmp(specifier->child->id, "TYPE")) {
                new_type = create_basic_type(specifier);
            } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
                new_type = create_struct_type(specifier);
            }
        } else {
            // ????????? VarDec
            // printf("?????????\n");
            field = create_array_field(node->child, specifier);
            new_type = create_array_type(node->sibling->sibling->data.i);
        }
        Type prev_type = field->type;
        // find the deepest array.elem which is null
        while (prev_type->u.array.elem != NULL) {
            prev_type = prev_type->u.array.elem;
        }
        prev_type->u.array.elem = new_type;
        return field;
    }
    // impossible to execute
    return NULL;
}

// check name in table and check fields in struct
// add struct itself into table
FieldList create_struct_field_for_struct(Node* struct_specifier) {
    Node* opt_tag = struct_specifier->child->sibling;
    char* id;
    if (!strcmp(opt_tag->id, "Epsilon")) {
        id = anonymous_struct_name();
    } else {
        id = opt_tag->child->data.text;
    }
    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = STRUCTURE;
    new_type->u.structure == NULL;
    new_type->size = -1;
    FieldList res = init_fieldlist(id, new_type, NULL, false, -1, NULL);
    FieldList next_field;

    // handle deflist
    // in order to check field name
    Table table = new_table();
    Node* deflist = struct_specifier->child->sibling->sibling->sibling;
    Node* def = NULL;
    while (strcmp(deflist->id, "Epsilon") != 0) {
        def = deflist->child;
        Node* specifier = def->child;
        Node* declist = specifier->sibling;
        Node* dec = NULL;
        while (true) {
            dec = declist->child;
            // handle single dec
            if (dec->child->sibling != NULL) {
                printf("Error type 15 at line %d: Initialized field \"%s\"\n", dec->lineno, dec->child->child->data.text);
            }
            Node* vardec = dec->child;
            FieldList field = check_VarDec(specifier, vardec, false);

            // check whether field name repeats
            if (find_field(table, field->name) != NULL) {
                printf("Error type 15 at line %d: Redefined field \"%s\"\n", vardec->lineno, vardec->child->data.text);
            } else {
                add_table_node(table, field);
            }
            // concat StructureField
            if (res->type->u.structure == NULL) {
                res->type->u.structure = field;
                next_field = field;
            } else {
                next_field->next = field;
                next_field = field;
            }
            assert(declist != NULL);
            assert(declist->child != NULL);
            if (declist->child->sibling == NULL)
                break;
            else {
                assert(declist->child->sibling != NULL);
                declist = declist->child->sibling->sibling;
            }
        }
        deflist = def->sibling;
    }
    // handle deflist end

    // begin check name
    if (find_field(curr_table, id) != NULL) {
        printf("Error type 16 at line %d: Duplicated name \"%s\"\n", opt_tag->lineno, id);
    } else {
        add_table_node(curr_table, res);
    }
    return res;
}

int get_field_size(FieldList field) {
    if (field->size == -1) {
        field->size = get_type_size(field->type);
    }
    return field->size;
}

int get_type_size(Type type) {
    if (type->size != -1) return type->size;
    // init type size
    if (type->kind == BASIC) {
        type->size = 4;
    } else if (type->kind == ARRAY) {
        type->size = type->u.array.size * get_type_size(type->u.array.elem);
    } else if (type->kind == STRUCTURE) {
        FieldList first = type->u.structure;
        int total_size = 0;
        while (first != NULL) {
            total_size += get_field_size(first);
            first = first->next;
        }
        type->size = total_size;
    }
    return type->size;
}

int get_struct_field_offset(Type temp, const char* field) {
    int offset = 0;
    assert(temp->u.structure->type->kind == STRUCTURE);
    FieldList first = temp->u.structure->type->u.structure;
    while (first != NULL) {
        if (!strcmp(first->name, field)) { return offset; }
        offset += get_field_size(first);
        first = first->next;
    }
    return -1;
}