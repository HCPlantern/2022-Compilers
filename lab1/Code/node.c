
#include "node.h"

/* create node for terminals */
Node* new_node(char* id) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->lineno = yylineno;

    node->id = (char*) malloc((strlen(id) + 1) * sizeof(char));
    strcpy(node->id, id);
    node->text = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
    strcpy(node->text, yytext);

    node->child = NULL;
    node->sibling = NULL;
    node->is_terminal = true;
    printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
    return node;
}

/* build a tree for non-terminals and return this node as its value */
Node* build_tree(char* id, int arg_len, ...) {
    Node* this = (Node*) malloc(sizeof(Node));
    this->id = (char*) malloc((strlen(id) + 1) * sizeof(char));
    strcpy(this->id, id);

    this->sibling = NULL;
    this->is_terminal = false;

    va_list args;
    Node* prev;
    Node* next;

    va_start(args, arg_len);
    next = va_arg(args, Node*);
    this->child = next;

    this->lineno = next->lineno;
    printf("build child link %s -> %s\n", this->id, next->id);

    for (int i = 0; i < arg_len - 1; i++) {
        prev = next;
        next = va_arg(args, Node*);
        prev->sibling = next;
        printf("build sibling link %s -> %s\n", prev->id, next->id);

    }
    return this;
}

/* print tree from root in pre-order */
void print_tree(Node* root, int indent) {
    if (root == NULL || !strcmp(root->id, "Epsilon")) {
        return;
    }
    // print the indent
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    if (!root->is_terminal) {
        printf("%s (%d)\n", root->id, root->lineno);
    } else {
        if (!strcmp(root->id, "INT") || !strcmp(root->id, "ID") || !strcmp(root->id, "TYPE") || !strcmp(root->id ,"FLOAT"))
            printf("%s: %s\n", root->id, root->text);
        else
            printf("%s\n", root->id);
    }

    root = root->child;
    while (root) {
        print_tree(root, indent + 2);
        root = root->sibling;
    }
    return;
}

yyerror(char* msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}