#include <stdio.h>

#include "semantic.h"
#include "semantic_check.h"
#include "hash_table.h"
#include "stack.h"
#include "ir.h"
#include "object_code_gen.h"

extern Node* syntax_tree_root;
extern void del_tree(Node* root);
extern void check_undefined_func();
extern void init_read_write_func();
extern void object_code_gen_go();
extern bool has_syntax_error;
extern Type arg_type;
extern Stack stack;
extern IR* ir_list;
extern ObjectCode* object_code;

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
    // write ir code
    // if (has_syntax_error) return;
    // IR* code = ir_list->next;
    // while (code != ir_list) {
    //     if (0 != strcmp(code->ir, "GOTO")) {
    //         fprintf(f, "%s\n", code->ir);
    //     }
    //     code = code->next;
    // }
    // fclose(f);
    // FILE* init = fopen("object_code_init", "r");
    // if (!init) {
    //     perror("object_code_init");
    // }
    // char buf[255];
    // while (fgets(buf, 255, init)) {
    //     fprintf(f, "%s", buf);
    // }
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n");
    fprintf(f, "read:\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _prompt\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 5\n");
    fprintf(f, "syscall\n");
    fprintf(f, "jr $ra\n");
    fprintf(f, "write:\n");
    fprintf(f, "li $v0, 1\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _ret\n");
    fprintf(f, "syscall\n");
    fprintf(f, "move $v0, $0\n");
    fprintf(f, "jr $ra\n");
    if (has_syntax_error) return;
    ObjectCode* code = object_code->next;
    while (code != object_code) {
        fprintf(f, "%s\n", code->code);
        code = code->next;
    }
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
    // ir_optimization();
    // print_ir();

    object_code_gen_go();

    // write to file
    FILE* w = fopen(argv[2], "w+");
    if (!w) {
        perror(argv[2]);
        return 1;
    }
    write_file(w);

    return 0;
}
