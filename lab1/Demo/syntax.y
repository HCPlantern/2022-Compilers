%{
    #define YYDEBUG 1
    #include <stdio.h>
    #include <stdlib.h>
    #include "lex.yy.c"
    #include <stdarg.h>
    Node* build_tree(char* id, int arg_len, ...);
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
    | EXP {$$ = build_tree("CALC", 1, $1);}
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
Node *build_tree(char *id, int arg_len, ...) {
    Node *this = (Node *) malloc(sizeof(Node));
    // TODO
    this->sibling = NULL;
    this->id = id;

    va_list args;
    Node *prev;
    Node *next;

    va_start(args, arg_len);
    next = va_arg(args, Node *);

    this->child = next;
    printf("build child link %s -> %s", this->id, next->id);
    if (arg_len >= 2) {
        for (int i = 0; i < arg_len - 1; i++) {
            prev = next;
            next = va_arg(args, Node *);
            prev->sibling = next;
            printf("build sibling link %s -> %s", prev->id, next->id);
        }
    }
    return this;
}

yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}