%{
    // #define YYDEBUG 1
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.c"
    #include <stdarg.h>
    Node* build_tree(char* id, int arg_len, ...);
    void print_tree(Node* root);
%}

%union {
    struct tree_node* node;
}

/* declared tokens */
%token <node> INT PLUS MINUS STAR DIV

/* declared non-terminals */
%type <node> CALC EXP FACTOR TERM

%%

CALC : /* empty */
    | EXP {$$ = build_tree("CALC", 1, $1); print_tree($$);}
    ;

EXP : FACTOR {$$ = build_tree("EXP", 1, $1);}
    | EXP PLUS FACTOR {$$ = build_tree("EXP", 3, $1, $2, $3);}
    | EXP MINUS FACTOR {$$ = build_tree("EXP", 3, $1, $2, $3);}
    ;

FACTOR : TERM {$$ = build_tree("FACTOR", 1, $1);}
    | FACTOR STAR TERM {$$ = build_tree("FACTOR", 3, $1, $2, $3);}
    | FACTOR DIV TERM {$$ = build_tree("FACTOR", 3, $1, $2, $3);}
    ;

TERM : INT {$$ = build_tree("TERM", 1, $1);}
    ;


%%

/* build a tree for non-terminals and return this node as its value */
Node* build_tree(char* id, int arg_len, ...) {
    Node* this = (Node* ) malloc(sizeof(Node));
    // TODO
    this->sibling = NULL;
    this->id = id;

    va_list args;
    Node* prev;
    Node* next;

    va_start(args, arg_len);
    next = va_arg(args, Node*);
    this->lineno = next->lineno;

    this->child = next;
    //printf("build child link %s -> %s\n", this->id, next->id);
    for (int i = 0; i < arg_len - 1; i++) {
        prev = next;
        next = va_arg(args, Node*);
        prev->sibling = next;
        //printf("build sibling link %s -> %s\n", prev->id, next->id);
    }
    return this;
}

void print_tree(Node* root) {

}

yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}