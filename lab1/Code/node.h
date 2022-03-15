#ifndef NAME_H
#define NAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
// #define YYDEBUG 1

extern int yylineno;
extern char* yytext;
extern void yyrestart(FILE*);
extern int yyparse(void);
// extern int yyerror(char*);
/* left node is child and right node is the first sibling */
typedef struct tree_node {
    int lineno;
    char *id;
    // 3 different types: string, int and float
    union Data{
        char* text;
        uint32_t i;
        float f;
    }data;
    struct tree_node* child;
    struct tree_node* sibling;
    bool is_terminal;
} Node;

Node *new_node(char *id);
Node *build_tree(char *id, int arg_len, ...);
void print_tree(Node *root, int indent);
void print_errorA(int lineno, char* msg, char* text);

#endif
