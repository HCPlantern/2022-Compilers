#ifndef TYPE_H
#define TYPE_H
#include <assert.h>
#include <stdbool.h>

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
        enum _basic {T_INT, T_FLOAT} basic;
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
    int size; // memory that this type takes
};

struct _FieldList {
    char* name;
    Type type;
    FieldList next;
    bool is_var; // should this field be able to hold three kinds of vals: VAR, STRUCT_DEF, FUNC?
    int size; // memory that the field takes
    char* ir_var;
};


bool type_equal(Type type1, Type type2);
char* anonymous_struct_name();
Type create_int_type();
Type create_float_type();
Type create_basic_type(Node* specifier);
Type create_array_type(int size);
Type create_struct_type(Node* specifier);
Type create_func_type(Node* specifier, Node* fundec);
Type get_exp_type(Node* exp);
FieldList create_basic_and_struct_field_for_var(char* name, Node* specifier);
FieldList create_array_field(Node* node, Node* specifier);

FieldList create_struct_field_for_struct(Node* struct_specifier);
int get_field_size(FieldList field);
int get_type_size(Type type);
int get_struct_field_offset(FieldList temp, const char* field);

char* get_ir_var_by_name(char* name);
char* get_ir_var_by_field(FieldList fieldList);
#endif