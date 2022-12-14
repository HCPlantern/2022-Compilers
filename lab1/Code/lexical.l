%option yylineno

%{
    #include "node.h"
    #include "syntax.tab.h"
%}

dec 0|([1-9][0-9]*)
oct (0[0-7]{1,10})|(0[0-3]([0-7]{10}))
hex 0(x|X)[0-9a-fA-F]{1,8}

float ([0-9]+\.[0-9]+)|((([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[eE][+-]?[0-9]+)
letter [_a-zA-Z]
delim [ \t\n]
ws {delim}+
type int|float
id {letter}[_a-zA-Z0-9]*
relop >|<|>=|<=|==|!=
nan [nN][aA][nN]
inf [iI][nN][fF]([iI][nN][iI][tT][yY])?


%%

{ws} {/* no actioin */}
(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+)|\.)[eE][+-]?[0-9]*\.[0-9]* {print_errorA(yylineno, "Illegal float number:", yytext);}
{float} {/*printf("float\n");*/ yylval.node = new_node("FLOAT"); return FLOAT;}
{dec}|{oct}|{hex} {/*printf("int\n");*/ yylval.node = new_node("INT"); return INT;}
"strtof(\""({nan}|{inf})"\")" {/*printf("shortcut\n");*/ yylval.node = new_node("FLOAT"); return FLOAT;}

{type} {yylval.node = new_node("TYPE"); return TYPE;}
"struct" {yylval.node = new_node("STRUCT"); return STRUCT;}
"return" {yylval.node = new_node("RETURN"); return RETURN;}
"if" {yylval.node = new_node("IF"); return IF;}
"else" {yylval.node = new_node("ELSE"); return ELSE;}
"while" {yylval.node = new_node("WHILE"); return WHILE;}

{id} {yylval.node = new_node("ID"); return ID;}
";" {yylval.node = new_node("SEMI"); return SEMI;}
"," {yylval.node = new_node("COMMA"); return COMMA;}
"=" {yylval.node = new_node("ASSIGNOP"); return ASSIGNOP;}
{relop} {yylval.node = new_node("RELOP"); return RELOP;}
"+" {yylval.node = new_node("PLUS"); return PLUS;}
"-" {yylval.node = new_node("MINUS"); return MINUS;}
"*" {yylval.node = new_node("STAR"); return STAR;}
"/" {yylval.node = new_node("DIV"); return DIV;}
"&&" {yylval.node = new_node("AND"); return AND;}
"||" {yylval.node = new_node("OR"); return OR;}
"." {yylval.node = new_node("DOT"); return DOT;}
"!" {yylval.node = new_node("NOT"); return NOT;}
"(" {yylval.node = new_node("LP"); return LP;}
")" {yylval.node = new_node("RP"); return RP;}
"[" {yylval.node = new_node("LB"); return LB;}
"]" {yylval.node = new_node("RB"); return RB;}
"{" {yylval.node = new_node("LC"); return LC;}
"}" {yylval.node = new_node("RC"); return RC;}

\/\/ {
    char c = input();
    while (c != '\0' && c != '\n') c = input();
}
"/*" {
    char c;
    int currentlineno = yylineno;
    while ('\0' != (c = input())) {
        if (c == '*') {
            while ('*' == (c = input()));
            // printf("%c ", c);
            if (c == '/') {
                break;
            }
        }
    }
    if (c == '\0') {
        print_errorA(currentlineno, "Unmatched", "/*");
    }
}

0(x|X)[_0-9a-zA-Z]* {print_errorA(yylineno, "Illegal hexical number:", yytext);}

0[_0-9a-zA-Z]+ {print_errorA(yylineno, "Illegal octal number:", yytext);}


. {print_errorA(yylineno, "Mysterious characters", yytext);}

%%
