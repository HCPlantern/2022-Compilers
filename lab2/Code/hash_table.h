#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "type.h"

typedef struct _TableEntry* TableEntry;
typedef struct _TableNode* TableNode;

#define TABLE_LEN 16384 

typedef TableEntry* Table;

struct _TableEntry {
    size_t len;
    TableNode next;
};

struct _TableNode {
    FieldList field;
    TableNode next;
};

Table new_table();
size_t cal_key(FieldList field);
void add_table_node(Table table, FieldList field);
bool find_field(Table table, FieldList field);
bool field_equal(FieldList field1, FieldList field2);
#endif