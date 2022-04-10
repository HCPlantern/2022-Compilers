#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
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
void del_table(Table);
void del_next(TableNode next);
size_t cal_key_by_field(FieldList field);
size_t cal_key(const char* name);
void add_table_node(Table table, FieldList field);
FieldList find_field(Table table, const char* name);
FieldList find_func_field(Table table, const char* name);
#endif