
#include "hash_table.h"

extern Node* syntax_tree_root;
// Hash Table funcs begin ----------------------------------------------------------------------
// 初始化 哈希表
Table new_table() {
    Table table = malloc(sizeof(TableEntry) * TABLE_LEN);
    for (size_t i = 0; i < TABLE_LEN; i++) {
        table[i] = malloc(sizeof(struct _TableEntry));
        table[i]->len = 0;
        table[i]->next = NULL;
    }
    return table;
}

void del_table(Table table) {
    for (size_t i = 0; i < TABLE_LEN; i++) {
        del_next(table[i]->next);
        free(table[i]);
    }
    free(table);
}

void del_next(TableNode next) {
    if (next->next == NULL)
        free(next);
    else {
        del_next(next->next);
        free(next);
    }
}

// 计算给定 field 的 hash table 键值
size_t cal_key_by_field(FieldList field) {
    return cal_key(field->name);
}

size_t cal_key(char* name) {
    unsigned long val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~(TABLE_LEN - 1)) val = (val ^ (i >> 12)) & (TABLE_LEN - 1);
    }
    return val % TABLE_LEN;
}

// 在链表的开头增加新节点 输入为一个 field
void add_table_node(Table table, FieldList field) {
    // create a new table_node
    TableNode node = malloc(sizeof(struct _TableNode));
    node->field = field;
    node->next = NULL;

    int key = cal_key_by_field(field);
    TableNode temp = table[key]->next;
    table[key]->next = node;
    node->next = temp;
    printf("Add new node %s\n", node->field->name);
    table[key]->len++;
}

FieldList find_field(Table table, char* name) {
    size_t key = cal_key(name);
    TableNode temp = table[key]->next;
    while (temp != NULL) {
        // printf("current table entry first node name: %s\n", temp->field->name);
        if (!strcmp(temp->field->name, name)) {
            printf("Find field %s\n", name);
            return temp->field;
        }
        temp = temp->next;
    }
    return NULL;
}

FieldList find_func_field(Table table, char* name) {
    FieldList field = find_field(table, name);
    if (field == NULL) return NULL;
    if (field->type->kind == FUNC)
        return field;
    return NULL;
}

/*
// table 中找到等价 field 则返回 true
// todo: 根据不同的等价规则 若等价则返回 true
bool find_field(Table table, FieldList field) {
    printf("cal_key_by_field");
    size_t key = cal_key_by_field(field);
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
*/

// Hash Table funcs end ---------------------------------------------------------------------