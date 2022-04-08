#include "semantic_check.h"

// Hash Table funcs begin ----------------------------------------------------------------------
// 初始化 哈希表
void init_table() {
    for (int i = 0; i < TABLE_LEN; i++) {
        table[i] = malloc(sizeof(struct TableEntry));
        table[i]->len = 0;
        table[i]->next = NULL;
    }
}

// 计算给定 field 的 hash table 键值
int cal_key(FieldList field) {
    char* name = field->name;
    unsigned int hash = 0, i;
    for (; *name; ++name) {
        hash = (hash << 2) + *name;
        if (i = hash & ~TABLE_LEN) hash = (hash ^ (i >> 12)) & TABLE_LEN;
    }
    return hash % TABLE_LEN;
}

// 在链表的开头增加新节点 输入为一个 field
void add_table_node(FieldList field) {
    assert(!find_field(field));
    // create a new table_node
    TableNode node = malloc(sizeof(struct TableNode));
    node->field = field;
    node->next = NULL;

    int key = cal_key(field);
    TableNode temp = table[key]->next;
    table[key]->next = node;
    node->next = temp;

    table[key]->len++;
    printf("Add table node \"%s\"\n", field->name);
}
// Hash Table funcs end ---------------------------------------------------------------------

// table 中找到等价 field 则返回 true
// todo: 根据不同的等价规则 若等价则返回 true
bool find_field(FieldList field) {
    int key = cal_key(field);
    TableNode temp = table[key]->next;
    while (temp != NULL) {
        if (!field_equal(temp->field, field)) {
            temp = temp->next;
        } else {
            printf("Find same field %s\n", temp->field->name);
            return true;
        }
    }
    return false;
}

// todo: 根据不同的等价规则 若等价则返回 true
bool field_equal(FieldList field1, FieldList field2) {
    return !strcmp(field1->name, field2->name);
}

// Create Types begin ---------------------------------------------------------------------

Type create_basic_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "TYPE"));

    Type new_type = malloc(sizeof(struct Type));
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
    Type new_type = malloc(sizeof(struct Type));
    new_type->kind = ARRAY;
    new_type->u.array.elem = NULL;
    new_type->u.array.size = size;
}

Type create_struct_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "StructSpecifier"));
    // todo
}

// todo
Type create_func_type() {
}

// Create Types end ---------------------------------------------------------------------

// Create fields begin ---------------------------------------------------------------------
// 创建并返回一个 basic field
// todo : 将单个 struct 结构也加入到哈希表中
FieldList create_basic_and_struct_field(char* name, Node* specifier) {
    Type new_type;
    if (!strcmp(specifier->child->id, "TYPE")) {
        new_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        new_type = create_struct_type(specifier);
    }
    FieldList field = malloc(sizeof(struct FieldList));
    field->name = name;
    field->type = new_type;
    field->next = NULL;
    return field;
}

// 传入一个数组的 VarDec 结点（最上层的）
// 除了最上层的 VarDec 结点 其他都创建 array field
FieldList create_array_field(Node* node, Node* specifier) {
    // printf("Create Array Field\n");
    assert(!strcmp(node->id, "VarDec"));
    if (!strcmp(node->child->id, "ID")) {  // 最底层 VarDec
        // printf("最底层\n");
        FieldList field = malloc(sizeof(struct FieldList));
        field->name = node->child->data.text;
        Type new_type = create_array_type(node->sibling->sibling->data.i);
        field->type = new_type;
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


// Create fields end ---------------------------------------------------------------------

// 遍历所有的 ExtDef 结点
void semantic_check(Node* node) {
    if (node == NULL || !strcmp(node->id, "Epsilon")) return;
    if (!strcmp(node->id, "ExtDef")) {
        check_ExtDef(node);
        semantic_check(node->sibling->child);
    }
    semantic_check(node->child);
}

void check_ExtDef(Node* node) {
    // printf("Build a ExtDef node.\n");
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
    // printf("Build a ExtDef node end.\n");
}

// 处理连续定义的外部变量
void check_ExtDecList(Node* specifier, Node* node) {
    while (true) {
        assert(!strcmp(node->id, "ExtDecList"));
        check_VarDec(specifier, node->child);
        if (node->child->sibling == NULL)
            break;
        else
            node = node->child->sibling->sibling;
    }
}

// 处理单个变量的定义或数组的定义
void check_VarDec(Node* specifier, Node* node) {
    assert(!strcmp(node->id, "VarDec"));
    if (!strcmp(node->child->id, "ID")) {
        // printf("check VarDec basic\n");
        FieldList new_field = create_basic_and_struct_field(node->child->data.text, specifier);
        if (find_field(new_field)) {
            printf("Error type 4 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
        } else {
            add_table_node(new_field);
        }
    } else if (!strcmp(node->child->id, "VarDec")) {
        // printf("check VarDec array\n");
        FieldList new_field = create_array_field(node, specifier);
        if (find_field(new_field)) {
            printf("Error type 4 at Line %d: Redefined variable \"%s\".\n", node->lineno, new_field->name);
        } else {
            add_table_node(new_field);
        }
    }
}