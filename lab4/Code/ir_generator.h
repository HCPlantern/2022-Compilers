#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H
#include "node.h"

void assign_gen(Node* father, Node* lValue, Node* exp);
void not_gen(Node* father, Node* exp);
void and_gen(Node* father, Node* exp1, Node*M, Node* exp2);
void or_gen(Node* father, Node* exp1, Node* M, Node* exp2);
void relop_gen(Node* father, Node* exp1, Node* relop, Node* exp2);
void plus_gen(Node* father, Node* exp1, Node* exp2);
void minus_gen(Node* father, Node* exp1, Node* exp2);
void star_gen(Node* father, Node* exp1, Node* exp2);
void div_gen(Node* father, Node* exp1, Node* exp2);
void negative_gen(Node* father, Node* exp);
void parentheses_reduce(Node* father, Node* exp);
void negative_gen(Node* father, Node* exp);
// void tilde_gen(Node* exp); // vm does not support tilde.
void func_call_gen(Node* father, Node* id, Node* args);  // special process for read() and write(x)
void array_store_gen(Node* father, Node* array, Node* index);
void array_access_gen(Node* father, Node* array, Node* index);
void field_access_gen(Node* father, Node* base, Node* field);
void field_load_gen(Node* father, Node* base, Node* field);
void id_gen(Node* father, Node* id);
void int_gen(Node* father, Node* _int);
void float_gen(Node* father, Node* _float);

void return_gen(Node* exp);
void var_dec_gen(Node* vardec);
void func_dec_gen(Node* id);
void param_dec_gen(Type arg_type);
void M_gen(Node* node);
void N_gen(Node* node);
char* relop_negative(char* relop);

void if_gen(Node* father, Node* cond_exp, Node* M, Node* stmt);
void if_else_gen(Node* father, Node* cond_exp, Node* M1, Node* true_stmt, Node* N, Node* M2, Node* false_stmt);
void while_gen(Node* father, Node* M1, Node* cond_exp, Node* M2, Node* stmt);
void trans_bool_to_value_gen(Node* exp);
void trans_value_to_bool_gen(Node* exp);
void star_reduce(Node* exp);
#endif