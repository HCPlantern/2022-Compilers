
#include "hash_table.h"

// Hash Table funcs begin ----------------------------------------------------------------------
// 初始化 哈希表
TableEntry* new_table() {
    Table table = malloc(sizeof(TableEntry) * TABLE_LEN);
    for (size_t i = 0; i < TABLE_LEN; i++) {
        table[i] = malloc(sizeof(struct _TableEntry));
        table[i]->len = 0;
        table[i]->next = NULL;
    }
    return table;
}

// 计算给定 field 的 hash table 键值
size_t cal_key(FieldList field) {
    char* name = field->name;
    unsigned int hash = 0, i;
    for (; *name; ++name) {
        hash = (hash << 2) + *name;
        if (i = hash & ~TABLE_LEN) hash = (hash ^ (i >> 12)) & TABLE_LEN;
    }
    return hash % TABLE_LEN;
}

// 在链表的开头增加新节点 输入为一个 field
void add_table_node(Table table, FieldList field) {
    // create a new table_node
    TableNode node = malloc(sizeof(struct _TableNode));
    node->field = field;
    node->next = NULL;

    int key = cal_key(field);
    TableNode temp = table[key]->next;
    table[key]->next = node;
    node->next = temp;

    table[key]->len++;
    printf("Add table node \"%s\"\n", field->name);
}

// table 中找到等价 field 则返回 true
// todo: 根据不同的等价规则 若等价则返回 true
bool find_field(Table table, FieldList field) {
    // printf("cal_key");
    size_t key = cal_key(field);
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

// Hash Table funcs end ---------------------------------------------------------------------