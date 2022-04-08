#include "node.h"

typedef struct _Type* Type;
typedef struct _FieldList* FieldList;

struct _Type {
    enum { BASIC, ARRAY, STRUCTURE, FUNC} kind;
    union {
        // basic
        enum {INT, FLOAT} basic;
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
        } function;
    } u;
};

struct _FieldList {
    char* name;
    Type type;
    FieldList next;
};

bool find_field(FieldList field);
bool field_equal(FieldList field1, FieldList field2);

Type create_basic_type(Node* specifier);
Type create_array_type(int size);
Type create_struct_type(Node* specifier);
Type create_func_type();

FieldList create_basic_and_struct_field(char* name, Node* specifier);
FieldList create_array_field(Node* node, Node* specifier);