%{
    // #define YYDEBUG 1
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.c"
    #include <stdarg.h>
    Node* build_tree(char* id, int arg_len, ...);
    void print_tree(Node* root, int indent);
%}

%union {
    struct tree_node* node;
}

/* declared tokens */
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

/* declared non-terminals */
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%%
/* High-level Definitions */
Program : ExtDefList {$$ = build_tree("Program", 1, $1); print_tree($$, 0);}
    ;

ExtDefList : ExtDef ExtDefList {$$ = build_tree("ExtDefList", 2, $1, $2);}
    | /* empty */
    ;

ExtDef : Specifier ExtDecList SEMI {$$ = build_tree("ExtDef", 3, $1, $2, $3);}
    | Specifier SEMI {$$ = build_tree("ExtDef", 2, $1, $2);}
    | Specifier FunDec CompSt {$$ = build_tree("ExtDef", 3, $1, $2, $3);}
    ;

ExtDecList : VarDec {$$ = build_tree("ExtDecList", 1, $1);}
    | VarDec COMMA ExtDecList {$$ = build_tree("ExtDefList", 3, $1, $2, $3);}
    ;

/* Specifiers */
Specifier : TYPE {$$ = build_tree("Specifier", 1, $1);}
    | StructSpecifier {$$ = build_tree("Specifier", 1, $1);}
    ;

StructSpecifier : STRUCT OptTag LC DefList RC {$$ = build_tree("StructSpecifier", 5, $1, $2, $3, $4, $5);}
    | STRUCT Tag {$$ = build_tree("StructSpecifier", 2, $1, $2);}
    ;

OptTag : ID {$$ = build_tree("OptTag", 1, $1);}
    | /* empty */
    ;
Tag : ID {$$ = build_tree("Tag", 1, $1);}
    ;

/* Declarators */
VarDec : ID {$$ = build_tree("VarDec", 1, $1);}
    | VarDec LB INT RB {$$ = build_tree("VarDec", 4, $1, $2, $3, $4);}
    ;

FunDec : ID LP VarList RP {$$ = build_tree("FunDec", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = build_tree("FunDec", 3, $1, $2, $3);}
    ;

VarList : ParamDec COMMA VarList {$$ = build_tree("VarList", 3, $1, $2, $3);}
    | ParamDec {$$ = build_tree("VarList", 1, $1);}
    ;

ParamDec : Specifier VarDec {$$ = build_tree("ParamDec", 2, $1, $2);}
    ;

/* Statements */
CompSt : LC DefList StmtList RC {$$ = build_tree("CompSt", 4, $1, $2, $3, $4);}
    ;

StmtList : Stmt StmtList {$$ = build_tree("StmtList", 2, $1, $2);} 
    | /* empty */
    ;

Stmt : Exp SEMI {$$ = build_tree("Stmt", 2, $1, $2);}
    | CompSt {$$ = build_tree("Stmt", 1, $1);}
    | RETURN Exp SEMI {$$ = build_tree("Stmt", 3, $1, $2, $3);}
    | IF LP Exp RP Stmt {$$ = build_tree("Stmt", 5, $1, $2, $3, $4, $5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$ = build_tree("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Exp RP Stmt {$$ = build_tree("Stmt", 5, $1, $2, $3, $4, $5);}
    ;

/* Local Definitions */
DefList : Def DefList {$$ = build_tree("DefList", 2, $1, $2);}
    | /* empty */
    ;

Def : Specifier DecList SEMI {$$ = build_tree("Def", 3, $1, $2, $3);}
    ;

DecList : Dec {$$ = build_tree("DecList", 1, $1);}
    | Dec COMMA DecList {$$ = build_tree("DecList", 3, $1, $2, $3);}
    ;

Dec : VarDec {$$ = build_tree("Dec", 1, $1);}
    | VarDec ASSIGNOP EXP {$$ = build_tree("Dec", 3, $1, $2, $3);}
    ;

/* Expressions */
Exp : Exp ASSIGNOP Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp AND Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp OR Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp RELOP Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp PLUS Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp MINUS Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp STAR Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp DIV Exp {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | LP Exp RP {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | MINUS Exp {$$ = build_tree("Exp", 2, $1, $2);}
    | NOT Exp {$$ = build_tree("Exp", 2, $1, $2);}
    | ID LP Args RP {$$ = build_tree("Exp", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | Exp LB Exp RB {$$ = build_tree("Exp", 4, $1, $2, $3, $4);}
    | Exp DOT ID {$$ = build_tree("Exp", 3, $1, $2, $3);}
    | ID {$$ = build_tree("Exp", 1, $1);}
    | INT {$$ = build_tree("Exp", 1, $1);}
    | FLOAT {$$ = build_tree("Exp", 1, $1);}
    ;

Args : Exp COMMA Args {$$ = build_tree("Args", 3, $1, $2, $3);}
    | Exp {$$ = build_tree("Args", 1, $1);}
    ;

%%

/* build a tree for non-terminals and return this node as its value */
Node* build_tree(char* id, int arg_len, ...) {
    Node* this = (Node*) malloc(sizeof(Node));
    this->id = (char*) malloc((strlen(id) + 1) * sizeof(char));
    strcpy(this->id, id);

    this->sibling = NULL;
    this->is_terminal = false;

    va_list args;
    Node* prev;
    Node* next;

    va_start(args, arg_len);
    next = va_arg(args, Node*);
    this->lineno = next->lineno;

    this->child = next;
    printf("build child link %s -> %s\n", this->id, next->id);
    
    for (int i = 0; i < arg_len - 1; i++) {
        prev = next;
        next = va_arg(args, Node*);
        prev->sibling = next;
        printf("build sibling link %s -> %s\n", prev->id, next->id);
    }
    return this;
}

/* print tree from root in pre-order */
void print_tree(Node* root, int indent) {
    if (root == NULL) {
        return;
    }
    // print the indent
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    if (!root->is_terminal) {
        printf("%s (%d)\n", root->id, root->lineno);
    } else {
        printf("%s: %s\n", root->id, root->text);
    }

    root = root->child;
    while (root) {
        print_tree(root, indent + 2);
        root = root->sibling;
    }
    return;
}

yyerror(char* msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}