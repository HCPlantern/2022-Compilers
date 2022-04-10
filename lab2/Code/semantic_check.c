#include "semantic_check.h"

#define curr_table (stack->tables[stack->top - 1])

extern Node* syntax_tree_root;
Stack stack;

// 遍历所有的 ExtDef 结点
void semantic_check(Node* node) {
    stack = new_stack();
    Table table = new_table();
    push(stack, table);
    node = node->child;
    assert(!strcmp(node->id, "ExtDefList"));
    while (strcmp(node->id, "Epsilon") != 0) {
        check_ExtDef(node->child);
        node = node->child->sibling;
    }
}

void check_ExtDef(Node* node) {
    Node* specifier_node = node->child;
    assert(!strcmp(specifier_node->id, "Specifier"));

    if (!strcmp(specifier_node->sibling->id, "ExtDecList")) {
        // 全局变量
        check_ExtDecList(specifier_node, specifier_node->sibling);
    } else if (!strcmp(specifier_node->sibling->id, "SEMI")) {
        // 忽略 int; 的情况 仅考虑 struct temp {...};
        if (!strcmp(specifier_node->child->id, "StructSpecifier")) {
            create_struct_field_for_struct(specifier_node->child);
        }
    } else if (!strcmp(specifier_node->sibling->id, "FunDec")) {
        // functions
        check_func(specifier_node);
    }
}

// 处理连续定义的外部变量
void check_ExtDecList(Node* specifier, Node* node) {
    while (true) {
        assert(!strcmp(node->id, "ExtDecList"));
        check_VarDec(specifier, node->child, true);
        if (node->child->sibling == NULL)
            break;
        else
            node = node->child->sibling->sibling;
    }
}

// 处理单个变量的定义或数组的定义
FieldList check_VarDec(Node* specifier, Node* node, bool need_check_table) {
    assert(!strcmp(node->id, "VarDec"));
    FieldList new_field;
    if (!strcmp(node->child->id, "ID")) {
        new_field = create_basic_and_struct_field_for_var(node->child->data.text, specifier);
        if (need_check_table) {
            if (find_field(curr_table, new_field->name)) {
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
            } else {
                add_table_node(curr_table, new_field);
            }
        }

    } else if (!strcmp(node->child->id, "VarDec")) {
        new_field = create_array_field(node, specifier);
        if (need_check_table) {
            if (find_field(curr_table, new_field->name)) {
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
            } else {
                add_table_node(curr_table, new_field);
            }
        }
    }
    return new_field;
}

void check_func(Node* specifier) {
    Node* fundec = specifier->sibling;
    Type func_type = create_func_type(specifier, fundec);
    char* name = fundec->child->data.text;
    FieldList field_found = find_func_field(curr_table, name);
    if (!strcmp(specifier->sibling->sibling->id, "SEMI")) {
        // func declare only
        if (field_found != NULL && !type_equal(func_type, field_found->type)) {
            printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", fundec->lineno, name);
            // todo: delete func_type
        } else if (field_found == NULL) {
            FieldList new_func_field = malloc(sizeof(struct _FieldList));
            new_func_field->name = name;
            new_func_field->type = func_type;
            new_func_field->is_var = false;
            func_type->u.function.is_defined = false;
            add_table_node(curr_table, new_func_field);
        }
    } else {
        // compst exists
        if (field_found != NULL) {
            if (field_found->type->u.function.is_defined) {
                printf("Error type 4 at Line %d: Redefined function \"%s\".\n", fundec->lineno, name);
            } else {
                if (type_equal(func_type, field_found->type)) {
                    field_found->type->u.function.is_defined = true;
                    // todo : delete func_type
                } else {
                    printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", fundec->lineno, name);
                }
            }
        } else {
            FieldList new_func_field = malloc(sizeof(struct _FieldList));
            new_func_field->name = name;
            new_func_field->type = func_type;
            new_func_field->is_var = false;
            func_type->u.function.is_defined = true;
            add_table_node(curr_table, new_func_field);
        }

        // todo : check compst
    }
}