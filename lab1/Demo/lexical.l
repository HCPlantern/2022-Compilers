%option yylineno

%{
    #include "syntax.tab.h"
    struct tree_node* new_node(char* id);
    struct tree_node {
        int lineno;
        char* id;
        char* text;
        struct tree_node* child;
    };
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
struct tree_node* new_node(char* id) {
    struct tree_node* node = (struct tree_node*)malloc(sizeof(struct tree_node));
    node->lineno = yylineno;
    node->id = id;
    node->text = yytext;
    node->child = NULL;
    printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
}