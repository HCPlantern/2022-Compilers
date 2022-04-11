#include "semantic.h"

// global variables that help sematic check.
// make sure that when the current checking process is done,
// the two vars below is set NULL.
// because whether they are NULL means different checking procedure.
static Type current_def_type;
static Node args_for_func_def;

