#ifndef NAME_H
#define NAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
// #define YYDEBUG 1

extern int yylineno;
extern char* yytext;
/* left node is child and right node is the first sibling */
typedef struct tree_node {
    int lineno;
    char *id;
    char *text;
    struct tree_node *child;
    struct tree_node *sibling;
    bool is_terminal;
} Node;

Node *new_node(char *id);
// #define YYDEBUG 1
Node *build_tree(char *id, int arg_len, ...);
void print_tree(Node *root, int indent);

#endif