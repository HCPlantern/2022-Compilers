
#include "ir.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

size_t ir_count = 0;
size_t temp_var_count = 0;
TempVar* temp_var_list;

char* new_ir() {
    char* res = malloc(sizeof(char) * (10));
    sprintf(res, "%s%lu", "v", ir_count);
    ir_count++;
    return res;
}

// init the list
void new_temp_var_list() {
    temp_var_list = malloc(sizeof(TempVar));
    temp_var_list->is_const = false;
    temp_var_list->next = NULL;
}

// call this when creating non-const temp var
TempVar* new_temp_var() {
    TempVar* res = malloc(sizeof(TempVar));
    char* name = malloc(sizeof(char) * (8));
    sprintf(name, "%s%lu", "t", temp_var_count);
    temp_var_count++;
    res->name = name;
    res->is_const = false;
    
    res->next = temp_var_list->next;
    temp_var_list->next = res;

    return res;
}

// call this when creating or finding const temp var
// first find, if not exist then create a new var
TempVar* find_const_var(int i) {
    TempVar* var = temp_var_list->next;
    while (var != NULL) {
        if (var->is_const && var->const_val == i) {
            return var;
        }
        var = var->next;
    }
    // create new temp var
    TempVar* new_var = new_temp_var();
    new_var->is_const = true;
    new_var->const_val = i;
    return new_var;
}

