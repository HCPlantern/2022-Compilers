#include <stdio.h>
#include "semantic_check.h"
#include "semantic.h"

extern Node* syntax_tree_root;
extern void del_tree(Node* root);

int main(int argc, char** argv) {
    if (argc<=1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    // extern yydebug;
    // yydebug = 1;
    yyparse();
    semantic_check(syntax_tree_root);
    del_tree(syntax_tree_root);
    return 0;
}

