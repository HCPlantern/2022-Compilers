%option yylineno

%{
    #include "syntax.tab.h"
    #include <stdbool.h>
    #include <string.h>
    /* left node is child and right node is the first sibling */
    typedef struct tree_node {
        int lineno;
        char id[30];
        char text[30];
        struct tree_node* child;
        struct tree_node* sibling;
        bool is_terminal;
    }Node;
    Node* new_node(char* id);
%}

digit [0-9]
delim [ \t\n]
ws {delim}+

%%
{ws} {/* no actioin */}
{digit}+ {yylval.node = new_node("INT");return INT;}
"+" {yylval.node = new_node("PLUS"); return PLUS;}
"-" {yylval.node = new_node("MINUS"); return MINUS;}
"*" {yylval.node = new_node("STAR"); return STAR;}
"/" {yylval.node = new_node("DIV"); return DIV;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}

%%
/* create node for terminals */
Node* new_node(char* id) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->lineno = yylineno;
    strcpy(node->id, id);
    strcpy(node->text, yytext);
    node->child = NULL;
    node->sibling = NULL;
    node->is_terminal = true;
    printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
    return node;
}