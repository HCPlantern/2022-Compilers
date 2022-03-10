%option yylineno

%{
    #include "syntax.tab.h"
    #include <stdbool.h>
    #include <string.h>
    /* left node is child and right node is the first sibling */
    typedef struct tree_node {
        int lineno;
        char* id;
        char* text;
        struct tree_node* child;
        struct tree_node* sibling;
        bool is_terminal;
    }Node;
    Node* new_node(char* id);
%}

digit [0-9]
letter [_a-zA-Z]
delim [ \t\n]
ws {delim}+
int 0|([1-9]digit*)
float [0-9]+\.[0-9]+
id {letter}[_a-zA-Z0-9]{0, 31}
relop >|<|>=|<=|==|!=
type "int" | "float"
%%
{ws} {/* no actioin */}
int {yylval.node = new_node("INT"); return INT;}
";" {yylval.node = new_node("SEMI"); return SEMI;}
"," {yylval.node = new_node("COMMA"); return COMMA;}
"=" {yylval.node = new_node("ASSIGNOP"); return ASSIGNOP;}
relop {yylval.node = new_node("RELOP"); return RELOP;}
"+" {yylval.node = new_node("PLUS"); return PLUS;}
"-" {yylval.node = new_node("MINUS"); return MINUS;}
"*" {yylval.node = new_node("STAR"); return STAR;}
"/" {yylval.node = new_node("DIV"); return DIV;}
"&&" {yylval.node = new_node("AND"); return AND;}
"||" {yylval.node = new_node("OR"); return OR;}
"." {yylval.node = new_node("DOT"); return DOT;}
"!" {yylval.node = new_node("NOT"); return NOT;}
type {yylval.node = new_node("TYPE"); return TYPE;}
"(" {yylval.node = new_node("LP"); return LP;}
")" {yylval.node = new_node("RP"); return RP;}
"[" {yylval.node = new_node("LB"); return LB;}
"]" {yylval.node = new_node("RB"); return RB;}
"{" {yylval.node = new_node("LC"); return LC;}
"}" {yylval.node = new_node("RC"); return RC;}
"struct" {yylval.node = new_node("STRUCT"); return STRUCT;}
"return" {yylval.node = new_node("RETURN"); return RETURN;}
"if" {yylval.node = new_node("IF"); return IF;}
"else" {yylval.node = new_node("ELSE"); return ELSE;}
"while" {yylval.node = new_node("WHILE"); return WHILE;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}

%%
/* create node for terminals */
Node* new_node(char* id) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->lineno = yylineno;

    node->id = (char*) malloc((strlen(id) + 1) * sizeof(char));
    strcpy(node->id, id);
    node->text = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
    strcpy(node->text, yytext);

    node->child = NULL;
    node->sibling = NULL;
    node->is_terminal = true;
    printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
    return node;
}