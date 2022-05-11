#include <stdio.h>

#include "semantic.h"
#include "semantic_check.h"
#include "hash_table.h"
#include "stack.h"
#include "ir.h"

extern Node* syntax_tree_root;
extern void del_tree(Node* root);
extern void check_undefined_func();
extern void init_read_write_func();
extern Stack stack;

void init() {
    // init stack and table;
    stack = new_stack();
    Table table = new_table();
    push(table);
    // init temp var lsit
    new_temp_var_list();
    new_ir_code_list();
    init_read_write_func();
}

int main(int argc, char** argv) {
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    // extern yydebug;
    // yydebug = 1;
    init();
    yyparse();
    check_undefined_func();
    // del_tree(syntax_tree_root);
    print_ir();
    return 0;
}
