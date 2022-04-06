#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct Type* Type;
typedef struct FieldList* FieldList;
typedef struct TableEntry* Table;
typedef struct TableNode* TableNode;

#define TABLE_LEN 16384 

struct Type {
    enum { BASIC, ARRAY, STRUCTURE, FUNC} kind;
    union {
        // basic
        enum {INT, FLOAT} basic;
        // array
        struct {
            Type elem;
            int size;
        } array;
        // struct
        FieldList structure;
        // function
        struct {
            int arg_len;
            Type return_type;
            FieldList args;
        } function;
    } u;
};

struct FieldList {
    char* name;
    Type type;
    FieldList next;
};

struct TableEntry {
    int len;
    TableNode next;
};

struct TableNode {
    FieldList field;
    TableNode next;
};

void init_table();
int cal_key(FieldList field);
void add_table_node(FieldList field);
bool find_field(FieldList field);
bool field_equal(FieldList field1, FieldList field2);

#endif