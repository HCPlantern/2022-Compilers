
#include "type.h"

#include <stdio.h>

#include "stack.h"

#define curr_table (stack->tables[stack->top - 1])

extern Node* syntax_tree_root;
extern Stack stack;
extern FieldList check_VarDec(Node* specifier, Node* node, bool in_struct);

size_t anonymous_struct_count = 0;

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

Type create_basic_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "TYPE"));

    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = BASIC;
    if (!strcmp(specifier->child->data.text, "int")) {
        new_type->u.basic = INT;
    } else if (!strcmp(specifier->child->data.text, "float")) {
        new_type->u.basic = FLOAT;
    }
    return new_type;
}

// u.array.elem is null
Type create_array_type(int size) {
    Type new_type = malloc(sizeof(struct _Type));
    new_type->kind = ARRAY;
    new_type->u.array.elem = NULL;
    new_type->u.array.size = size;
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
        return res;
    } else if (!strcmp(struct_specifier->child->sibling->id, "Tag")) {
        // find struct in all tables
        char* name = struct_specifier->child->sibling->child->id;
        FieldList field = NULL;
        size_t stack_index = stack->top - 1;
        Table table;
        while (stack_index >= 0) {
            table = stack->tables[stack_index];
            field = find_field(table, name);
            if (field != NULL) return field->type;
            stack_index--;
        }
        return NULL;
    }
    return NULL;
}

// todo
Type create_func_type(Node* specifier, Node* fundec) {
    Type res = malloc(sizeof(struct _Type));
    res->kind = FUNC;
    // get return type
    Type return_type;
    if (!strcmp(specifier->child->id, "TYPE")) {
        return_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        return_type = create_struct_type(specifier);
    }
    res->u.function.return_type = return_type;

    // handle args
    if (fundec->child->sibling->sibling->sibling == NULL) {
        res->u.function.arg_len = 0;
        res->u.function.args = NULL;
    } else {
        Node* varlist = fundec->child->sibling->sibling;
        Node* paramdec = NULL;
        while (true) {
            paramdec = varlist->child;

            if (paramdec->sibling == NULL) break;
            else paramdec = paramdec->sibling->sibling->child;
        }
    }
}


FieldList create_basic_and_struct_field_for_var(char* name, Node* specifier) {
    Type new_type;
    if (!strcmp(specifier->child->id, "TYPE")) {
        new_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        new_type = create_struct_type(specifier);
    }
    FieldList field = malloc(sizeof(struct _FieldList));
    field->name = name;
    field->type = new_type;
    field->next = NULL;
    field->is_var = true;
    return field;
}

// 传入一个数组的 VarDec 结点（最上层的）
// 除了最上层的 VarDec 结点 其他都创建 array field
FieldList create_array_field(Node* node, Node* specifier) {
    assert(!strcmp(node->id, "VarDec"));
    if (!strcmp(node->child->id, "ID")) {  // 最底层 VarDec
        // printf("最底层\n");
        FieldList field = malloc(sizeof(struct _FieldList));
        field->name = node->child->data.text;
        Type new_type = create_array_type(node->sibling->sibling->data.i);
        field->type = new_type;
        field->is_var = true;
        return field;
    } else if (!strcmp(node->child->id, "VarDec")) {
        FieldList field;
        Type new_type;
        if (node->sibling == NULL || strcmp(node->sibling->id, "LB") != 0) {
            // 最上层的 VarDec 增加一个最深层的 type
            // 两种情况 basic 或 struct
            // printf("最上层\n");
            field = create_array_field(node->child, specifier);
            if (!strcmp(specifier->child->id, "TYPE")) {
                new_type = create_basic_type(specifier);
            } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
                new_type = create_struct_type(specifier);
            }
        } else {
            // 中间层 VarDec
            // printf("中间层\n");
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

    FieldList res = malloc(sizeof(struct _FieldList));
    res->is_var = false;
    res->name = id;
    res->next = NULL;
    res->type = malloc(sizeof(struct _Type));
    res->type->kind = STRUCTURE;
    FieldList next_structure_field = res->type->u.structure;

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
                printf("Error type 15 at line %d: Initialized field \"%s\"", dec->lineno, dec->child->child->data.text);
            }
            Node* vardec = dec->child;
            FieldList field = check_VarDec(specifier, vardec, true);

            // check whether field name repeats
            if (find_field(table, field->name) != NULL) {
                printf("Error type 15 at line %d: Redefined field \"%s\"\n", vardec->lineno, vardec->child->data.text);
            } else {
                add_table_node(table, field);
            }
            // concat StructureField
            next_structure_field = field;
            if (next_structure_field->next != NULL)
                next_structure_field = next_structure_field->next;

            if (declist->child->sibling == NULL)
                break;
            else
                declist = declist->child->sibling->sibling;
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

