#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "node.h"
#include "type.h"
// #include "hashtable.h"

/*
 * because bison use LALR, so when checking the father node,
 * the children nodes have been checked semantically.
 *
 * the checking process assumes that the child nodes are legal.
 * (if the child node is determined wrong, the father node should not report error
 *  because the child error is more fundamental.)
 * 
 * the callsite of the check is post by default.
 */


void var_dec_check(Node varDec);
// TODO: add support for array dec

// generate the type of the funDec, check the table.
void fun_dec_check(Node funDec, Node varList);  // varList is null if no para


void new_scope();    // meet '{'
void exit_scope();   // meet '}'


void return_type_check(Node ret_exp);
void condition_type_check(Node condition_exp);

// we do not support initialization of array and struct var because exp cannot be array or struct val.
// so the current type must be basic type.
void dec_assign_check(Node dec);

/* check for exps.
   behavior:
   1. check the type of children, 
      if legal append the new type to the father node
      if illegal yield the error.
   2. if the type is legal, do the basic constant propagation.
 */

void binary_cal_check(Node father, Node exp1, Node exp2); // + - * /
void assignment_check(Node father, Node lValue, Node rValue); // not only the type but also the lValue.
void logical_check(Node father, Node exp1, Node exp2);    // AND OR
void relop_check(Node father, Node exp1, Node exp2);      // relop
void parathese_check(Node father, Node inner_exp);  // (exp)
void minus_check(Node father, Node exp);      // -
void not_check(Node father, Node exp);        // !
// void tilde_check(Node father, Node exp);   // ~
void func_call_check(Node father, Node args);  // if has no arg, args is NULL
void array_check(Node father, Node index);      // arr[exp]
void field_access_check(Node father, Node base, Node field); // exp.field
void id_check(Node id); // related error: 1 2
void literal_check(Node node);  // INT FLOAT

#endif