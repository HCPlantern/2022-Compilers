#ifndef TYPE_H
#define TYPE_H
#include <assert.h>
#include <stdbool.h>
#include "syntax.tab.h"

typedef struct tree_node Node;

typedef struct _Type* Type;
typedef struct _FieldList* FieldList;

struct _Type {
    enum { BASIC,
           ARRAY,
           STRUCTURE,
           FUNC,
           UNDEF } kind;  // UNDEF means conflict (in another word, error).
    union {
        // basic
        enum yytokentype basic;
        // array
        struct {
            Type elem;
            int size;
        } array;
        // struct
        FieldList structure;
        // function
        struct {
            int arg_len;
            Type return_type;
            FieldList args;
            bool is_defined;
        } function;
    } u;
};

struct _FieldList {
    char* name;
    Type type;
    FieldList next;
    bool is_var;
};

#include "hash_table.h"

bool type_equal(Type type1, Type type2);
char* anonymous_struct_name();
Type create_basic_type(Node* specifier);
Type create_array_type(int size);
Type create_struct_type(Node* specifier);
Type create_func_type(Node* specifier, Node* fundec);
Type get_exp_type(Node* exp);
FieldList create_basic_and_struct_field_for_var(char* name, Node* specifier);
FieldList create_array_field(Node* node, Node* specifier);

FieldList create_struct_field_for_struct(Node* struct_specifier);

#endif