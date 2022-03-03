%option yylineno

%{
    #include "syntax.tab.h"
    /* left node is child and right node is the first sibling */
    typedef struct tree_node {
        int lineno;
        char* id;
        char* text;
        struct tree_node* child;
        struct tree_node* sibling;
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

%%
/* create node for terminals */
Node* new_node(char* id) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->lineno = yylineno;
    node->id = id;
    node->text = yytext;
    node->child = NULL;
    node->sibling = NULL;
    printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
}