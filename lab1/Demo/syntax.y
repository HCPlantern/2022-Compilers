%{
    #include <stdio.h>
    #include "lex.yy.c"

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
/* build a tree for non-terminals and return this node */
struct tree_node* build_tree(char* id, int arg_len, ...) {
    struct tree_node* this = (struct tree_node*)malloc(sizeof(struct tree_node));
    // TODO
    return this;
}
yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}