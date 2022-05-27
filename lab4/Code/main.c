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
extern bool has_syntax_error;
extern Type arg_type;
extern Stack stack;

void init() {
    // init stack and table;
    arg_type = NULL;
    stack = new_stack();
    Table table = new_table();
    push(table);
    // init temp var lsit
    new_temp_var_list();
    new_ir_code_list();
    init_read_write_func();
}

void write_file(FILE* f) {
    if (has_syntax_error) return;
    IR* code = ir_list->next;
    while (code != ir_list) {
        if (0 != strcmp(code->ir, "GOTO")) {
            fprintf(f, "%s\n", code->ir);
        }
        code = code->next;
    }
    fclose(f);
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
    ir_optimization();
    print_ir();

    // write to file
    FILE* w = fopen(argv[2], "w+");
    if (!w) {
        perror(argv[2]);
        return 1;
    }
    write_file(w);

    return 0;
}
