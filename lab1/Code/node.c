
#include "node.h"

/* create node for terminals */
Node* new_node(char* id) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->lineno = yylineno;

    node->id = (char*) malloc((strlen(id) + 1) * sizeof(char));
    strcpy(node->id, id);
    if (!strcmp(id, "ID") || !strcmp(id, "TYPE")) {
        node->data.text = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
        strcpy(node->data.text, yytext);
    }
    if (!strcmp(id, "INT")) {
        // node->data.i = atoi(yytext);
        node->data.i = strtoul(yytext, NULL, 0);
    }
    if (!strcmp(id, "FLOAT")) {
        // node->data.f = atof(yytext);
        node->data.f = strtof(yytext, NULL);
    }

    node->child = NULL;
    node->sibling = NULL;
    node->is_terminal = true;
    // printf("new node line: %d; id: %s; text: %s\n", yylineno, id, yytext);
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
    // printf("line :%d build child link %s -> %s\n",this->lineno, this->id, next->id);

    for (int i = 0; i < arg_len - 1; i++) {
        prev = next;
        next = va_arg(args, Node*);
        prev->sibling = next;
        // printf("line :%d build sibling link %s -> %s\n",this->lineno, prev->id, next->id);

    }
    return this;
}

/* print tree from root in pre-order */
void print_tree(Node* root, int indent) {
    // skip null and epsilon
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
        if (!strcmp(root->id, "ID") || !strcmp(root->id, "TYPE")) {
            printf("%s: %s\n", root->id, root->data.text);
        }
        else if (!strcmp(root->id, "INT"))
            printf("%s: %d\n", root->id, root->data.i);
        else if ( !strcmp(root->id ,"FLOAT"))
            printf("%s: %f\n", root->id, root->data.f);
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

int yyerror(char* msg, int line) {
    fprintf(stderr, "Error type B at Line %d: %s\n", line, msg);
    return 0;
}
