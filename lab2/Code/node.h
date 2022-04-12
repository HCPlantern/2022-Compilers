#ifndef NODE_H
#define NODE_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define YYDEBUG 1

typedef struct _Type* Type;
typedef struct _FieldList* FieldList;

extern int yylineno;
extern char* yytext;
extern void yyrestart(FILE*);
extern int yyparse(void);
extern void yyerror();

/* left node is child and right node is the first sibling */
typedef struct tree_node {
    int lineno;
    char* id;
    // 3 different types: string, int and float
    union data {
        char* text;
        uint32_t i;
        float f;
    } data;
    struct tree_node* child;
    struct tree_node* sibling;
    bool is_terminal;

    FieldList corresponding_field;  // store inner presentation. (modeled by type and FieldList). NULL(abort) for non-field.
    struct _Type type;
    bool is_constant;
    union _Constant {
        uint32_t i;
        float f;
    } constant;
} Node;

Node* new_node(char* id);
Node* build_tree(char* id, int arg_len, ...);
int start_with_strtof(const char* yytext);
void print_tree(Node* root, int indent);
void print_errorA(int lineno, char* msg, char* text);
void print_errorB(int lineno, char* msg);
#endif
