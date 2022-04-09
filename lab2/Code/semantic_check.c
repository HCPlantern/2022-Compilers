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
        // todo: 仅结构体的定义而没有声明结构体变量

    } else if (!strcmp(specifier_node->sibling->id, "FunDec")) {
        // todo: 函数体
    }
}

// 处理连续定义的外部变量
void check_ExtDecList(Node* specifier, Node* node) {
    while (true) {
        assert(!strcmp(node->id, "ExtDecList"));
        check_VarDec(specifier, node->child, false);
        if (node->child->sibling == NULL)
            break;
        else
            node = node->child->sibling->sibling;
    }
}

// 处理单个变量的定义或数组的定义
FieldList check_VarDec(Node* specifier, Node* node, bool in_struct) {
    assert(!strcmp(node->id, "VarDec"));
    FieldList new_field;
    if (!strcmp(node->child->id, "ID")) {
        new_field = create_basic_and_struct_field_for_var(node->child->data.text, specifier);
        if (!in_struct) {
            if (find_field(curr_table, new_field->name)) {
                printf("Error type 4 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
            } else {
                add_table_node(curr_table, new_field);
            }
        }

    } else if (!strcmp(node->child->id, "VarDec")) {
        new_field = create_array_field(node, specifier);
        if (!in_struct) {
            if (find_field(curr_table, new_field->name)) {
                printf("Error type 4 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
            } else {
                add_table_node(curr_table, new_field);
            }
        }
    }
    return new_field;
}