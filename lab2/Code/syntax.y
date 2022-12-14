%{
    #include "lex.yy.c"
    #include "semantic.h"
    #include "semantic_check.h"
    #include "node.h"
    #include "hash_table.h"
    #include "stack.h"
    extern Node* syntax_tree_root;
    extern Node* current_specifier_node;
    extern Node* temp_ExtDef;
    extern bool is_in_compst;
    extern bool is_in_struct;
    extern Type arg_type;
    void check_ExtDef(Node* node);
    void check_ExtDecList(Node* specifier, Node* node);
    void check_func(Node* specifier);
    void add_args_into_table();
    void if_in_struct(bool enter);
%}

%union {
    struct tree_node* node;
}

/* declared tokens */
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE TILDE 

/* declared non-terminals */
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args LValue

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS TILDE 
%left STAR DIV
%right NOT
%left DOT LP RP LB RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

/* High-level Definitions */
Program : ExtDefList {$$ = build_tree("Program", 1, $1); /*print_tree($$, 0);*/ syntax_tree_root = $$;}
    ;
/* 0 or some ExtDef */
ExtDefList : ExtDef ExtDefList {$$ = build_tree("ExtDefList", 2, $1, $2);}
    | /* empty */ {$$ = new_node("Epsilon");}
    ;
/* def of global var, struct and function */
ExtDef : Specifier ExtDecList SEMI {$$ = build_tree("ExtDef", 3, $1, $2, $3);check_ExtDecList($1, $2);}
    | Specifier SEMI {$$ = build_tree("ExtDef", 2, $1, $2); check_ExtDef($$);}
    | Specifier FunDec { temp_ExtDef = build_tree("ExtDef", 2, $1, $2); check_func($1); is_in_compst = true; arg_type = &($2->type);} CompSt {is_in_compst = false; $$ = temp_ExtDef; $2->sibling = $4;}
    | Specifier FunDec SEMI {$$ = build_tree("ExtDef", 3, $1, $2, $3);check_func($1);}
    | Specifier ExtDecList ASSIGNOP error SEMI {print_errorB($2->lineno, ", global variable cannot be initialized.");}
    | Specifier error SEMI {print_errorB($$->lineno, "");}
    | error SEMI {print_errorB($$->lineno, "");}
    ;
/* 1 or some def of vardec: int global1, global2 */
ExtDecList : VarDec {$$ = build_tree("ExtDecList", 1, $1);}
    | VarDec COMMA ExtDecList {$$ = build_tree("ExtDecList", 3, $1, $2, $3);}
    ;

/* Specifiers */
Specifier : TYPE {$$ = build_tree("Specifier", 1, $1);}
    | StructSpecifier {$$ = build_tree("Specifier", 1, $1);}
    ;

StructSpecifier : STRUCT OptTag LC {if_in_struct(true);} DefList RC {if_in_struct(false); $$ = build_tree("StructSpecifier", 5, $1, $2, $3, $5, $6);}
    | STRUCT Tag {$$ = build_tree("StructSpecifier", 2, $1, $2);}
    ;

OptTag : ID {$$ = build_tree("OptTag", 1, $1);}
    | /* empty */ {$$ = new_node("Epsilon");}
    ;
Tag : ID {$$ = build_tree("Tag", 1, $1);}
    ;

/* Declarators */
VarDec : ID {$$ = build_tree("VarDec", 1, $1);}
    | VarDec LB INT RB {$$ = build_tree("VarDec", 4, $1, $2, $3, $4);}
    ;

FunDec : ID LP VarList RP {$$ = build_tree("FunDec", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = build_tree("FunDec", 3, $1, $2, $3);}
    | error RP {print_errorB($$->lineno, "");}
    ;

VarList : ParamDec COMMA VarList {$$ = build_tree("VarList", 3, $1, $2, $3);}
    | ParamDec {$$ = build_tree("VarList", 1, $1);}
    ;

ParamDec : Specifier VarDec {$$ = build_tree("ParamDec", 2, $1, $2);}
    ;

/* Statements */
CompSt : LC {new_scope();add_args_into_table();} DefList StmtList RC {exit_scope();} {$$ = build_tree("CompSt", 4, $1, $3, $4, $5);}
    | error RC {print_errorB($$->lineno, "");}
    ;

StmtList : Stmt StmtList {$$ = build_tree("StmtList", 2, $1, $2);} 
    | /* empty */ {$$ = new_node("Epsilon");}
    ;

Stmt : Exp SEMI {$$ = build_tree("Stmt", 2, $1, $2);}
    | CompSt {$$ = build_tree("Stmt", 1, $1);}
    | RETURN Exp SEMI {$$ = build_tree("Stmt", 3, $1, $2, $3); return_type_check($2);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = build_tree("Stmt", 5, $1, $2, $3, $4, $5); condition_type_check($3);}
    | IF LP Exp RP Stmt ELSE Stmt {$$ = build_tree("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); condition_type_check($3);}
    | WHILE LP Exp RP Stmt {$$ = build_tree("Stmt", 5, $1, $2, $3, $4, $5); condition_type_check($3);}
    | error RP {print_errorB($$->lineno, "");}
    | error SEMI {print_errorB($$->lineno, "");}
    ;

/* Local Definitions */
DefList : Def DefList {$$ = build_tree("DefList", 2, $1, $2);}
    | /* empty */ {$$ = new_node("Epsilon");}
    ;

Def : Specifier{current_specifier_node = $1;} DecList SEMI {$$ = build_tree("Def", 3, $1, $3, $4);current_specifier_node = NULL;}
    | Specifier error SEMI {print_errorB($$->lineno, "");}
    ;

DecList : Dec {$$ = build_tree("DecList", 1, $1);}
    | Dec COMMA DecList {$$ = build_tree("DecList", 3, $1, $2, $3);}
    ;

Dec : VarDec {$$ = build_tree("Dec", 1, $1); var_dec_check($1);}
    | VarDec {var_dec_check($1);} ASSIGNOP Exp {$$ = build_tree("Dec", 3, $1, $3, $4); dec_assign_check($$, $1, $4);}
    ;

/* Expressions */
Exp : LValue ASSIGNOP Exp {$$ = build_tree("Exp", 3, $1, $2, $3); assignment_check($$, $1, $3);}
    | Exp AND Exp {$$ = build_tree("Exp", 3, $1, $2, $3); logical_check($$, $1, $3);}
    | Exp OR Exp {$$ = build_tree("Exp", 3, $1, $2, $3); logical_check($$, $1, $3);}
    | Exp RELOP Exp {$$ = build_tree("Exp", 3, $1, $2, $3); relop_check($$, $1, $3);}
    | Exp PLUS Exp {$$ = build_tree("Exp", 3, $1, $2, $3); binary_cal_check($$, $1, $3);}
    | Exp MINUS Exp {$$ = build_tree("Exp", 3, $1, $2, $3); binary_cal_check($$, $1, $3);}
    | Exp STAR Exp {$$ = build_tree("Exp", 3, $1, $2, $3); binary_cal_check($$, $1, $3);}
    | Exp DIV Exp {$$ = build_tree("Exp", 3, $1, $2, $3); binary_cal_check($$, $1, $3);}
    | LP Exp RP {$$ = build_tree("Exp", 3, $1, $2, $3); parathese_check($$, $2);}
    | MINUS Exp {$$ = build_tree("Exp", 2, $1, $2); minus_check($$, $2);}
    | NOT Exp {$$ = build_tree("Exp", 2, $1, $2); not_check($$, $2);}
    | TILDE Exp{$$ = build_tree("Exp", 2, $1, $2); /* tilde_check($$, $2); */}
    | ID LP Args RP {$$ = build_tree("Exp", 4, $1, $2, $3, $4); func_call_check($$, $1, $3);}
    | ID LP RP {$$ = build_tree("Exp", 3, $1, $2, $3); func_call_check($$, $1, NULL);}
    | Exp LB Exp RB {$$ = build_tree("Exp", 4, $1, $2, $3, $4); array_check($$, $1, $3);}
    | Exp DOT ID {$$ = build_tree("Exp", 3, $1, $2, $3); field_access_check($$, $1, $3);}
    | ID {$$ = build_tree("Exp", 1, $1); id_check($$, $1);}
    | INT {$$ = build_tree("Exp", 1, $1); literal_check($$);}
    | FLOAT {$$ = build_tree("Exp", 1, $1); literal_check($$);}
    ;

LValue : ID {$$ = build_tree("LValue", 1, $1); id_check($$, $1);}
    | Exp LB Exp RB {$$ = build_tree("LValue", 4, $1, $2, $3, $4); array_check($$, $1, $3);}
    | Exp DOT ID {$$ = build_tree("LValue", 3, $1, $2, $3); field_access_check($$, $1, $3);}
    | error {printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", $$->lineno);}
    ;

Args : Exp COMMA Args {$$ = build_tree("Args", 3, $1, $2, $3);}
    | Exp {$$ = build_tree("Args", 1, $1);}
    ;

%%

