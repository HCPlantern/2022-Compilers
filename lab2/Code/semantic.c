#include "semantic.h"
#include "type.h"
#include "node.h"

// global variables that help sematic check.
// make sure that when the current checking process is done,
// the two vars below is set NULL.
// because whether they are NULL means different checking procedure.
static Type current_def_type;
static Node args_for_func_def;

void var_dec_check(Node* varDec) {

}

void fun_dec_check(Node* funDec, Node* varList/*, bool isDef*/) {

}

void new_scope() {

}

void exit_scope() {

}

void return_type_check(Node* ret_exp) {

}

void condition_type_check(Node* condition_exp) {

}

void dec_assign_check(Node* father, Node* varDec, Node* exp) {

}

void binary_cal_check(Node* father, Node* exp1, Node* exp2) {

}

void assignment_check(Node* father, Node* lValue, Node* rValue) {

}

void logical_check(Node* father, Node* exp1, Node* exp2) {

}

void relop_check(Node* father, Node* exp1, Node* exp2) {

}

void parathese_check(Node* father, Node* inner_exp) {

}

void minus_check(Node* father, Node* exp) {

}

void not_check(Node* father, Node* exp) {

}

// void tilde_check(Node* father, Node* exp);   // ~

void func_call_check(Node* father, Node* func, Node* args) {

}

void array_check(Node* father, Node* array, Node* index) {

}

void field_access_check(Node* father, Node* base, Node* field) {

}

void id_check(Node* father) {

}

void literal_check(Node* father) {

}