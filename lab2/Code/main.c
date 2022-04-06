#include <stdio.h>
#include "node.h"
#include "semantic_check.h"

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
    return 0;
}

void test_hash_table() {
    init_table();
    Type type1 = malloc(sizeof(struct Type));
    type1->kind = BASIC;
    type1->u.basic = INT;

    Type type2 = malloc(sizeof(struct Type));
    type2->kind = BASIC;
    type2->u.basic = FLOAT;

    FieldList field1 = malloc(sizeof(struct FieldList));
    field1->name = "i";
    field1->type = type1;
    field1->next = NULL;

    FieldList field2 = malloc(sizeof(struct FieldList));
    field2->name = "i";
    field2->type = type2;
    field2->next = NULL;

    add_table_node(field1);
    find_field(field1);
}