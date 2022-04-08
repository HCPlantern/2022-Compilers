#include "type.h"
#include "semantic_check.h"

// table 中找到等价 field 则返回 true
// todo: 根据不同的等价规则 若等价则返回 true
bool find_field(FieldList field) {
    int key = cal_key(field);
    TableNode temp = table[key]->next;
    while (temp != NULL) {
        if (!field_equal(temp->field, field)) {
            temp = temp->next;
        } else {
            printf("Find same field %s\n", temp->field->name);
            return true;
        }
    }
    return false;
}

// todo: only compare type. (in structural equivalence)
bool type_equal(Type type1, Type type2) {
    if (type1->kind == BASIC && type2->kind == BASIC) {
        return type1->u.basic == type2->u.basic;
    }
    else if (type1->kind == ARRAY && type2->kind == ARRAY) {
        return type_equal(type1->u.array.elem, type2->u.array.elem);
    }
    else if (type1->kind == STRUCTURE && type2->kind == STRUCTURE) {
        FieldList field1 = type1->u.structure;
        FieldList field2 = type2->u.structure;
        while (field1 != NULL && field2 != NULL) {
            if (!type_equal(field1->type, field2->type)) {
                return false;
            }
            field1 = field1->next;
            field2 = field2->next;
        }

        // only when one of the var field are NULL can reach here
        if (field1 == NULL && field2 == NULL) {
            return true;
        }

        // nums of fields not equal.
        return false;
    }
    else if (type1->kind == FUNC && type2->kind == FUNC) {
        if (type1->u.function.arg_len != type2->u.function.arg_len) {
            return false;
        }
        if (!type_equal(type1->u.function.return_type, type2->u.function.return_type)) {
            return false;
        }
        FieldList arg1 = type1->u.function.args;
        FieldList arg2 = type2->u.function.args;
        while (arg1 != NULL && arg2 != NULL) {
            if (!type_equal(arg1->type, arg2->type)) {
                return false;
            }
            arg1 = arg1->next;
            arg2 = arg2->next;
        }

        // only when one of the var field are NULL can reach here
        if (arg1 == NULL && arg2 == NULL) {
            return true;
        }

        // nums of fields not equal.
        return false;
    }

    // even the kinds do not match each other.
    return false;
}

// WARNING: the FieldList below is regarded as a single field.
// todo: 根据不同的等价规则 若等价则返回 true
bool field_equal(FieldList field1, FieldList field2) {
    return !strcmp(field1->name, field2->name);
}

// Create Types begin ---------------------------------------------------------------------

Type create_basic_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "TYPE"));

    Type new_type = malloc(sizeof(struct Type));
    new_type->kind = BASIC;
    if (!strcmp(specifier->child->data.text, "int")) {
        new_type->u.basic = INT;
    } else if (!strcmp(specifier->child->data.text, "float")) {
        new_type->u.basic = FLOAT;
    }
    return new_type;
}

// u.array.elem is null
Type create_array_type(int size) {
    Type new_type = malloc(sizeof(struct Type));
    new_type->kind = ARRAY;
    new_type->u.array.elem = NULL;
    new_type->u.array.size = size;
}

Type create_struct_type(Node* specifier) {
    assert(!strcmp(specifier->child->id, "StructSpecifier"));
    // todo
}

// todo
Type create_func_type() {
}

// Create Types end ---------------------------------------------------------------------

// Create fields begin ---------------------------------------------------------------------
// 创建并返回一个 basic field
// todo : 将单个 struct 结构也加入到哈希表中
FieldList create_basic_and_struct_field(char* name, Node* specifier) {
    Type new_type;
    if (!strcmp(specifier->child->id, "TYPE")) {
        new_type = create_basic_type(specifier);
    } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
        new_type = create_struct_type(specifier);
    }
    FieldList field = malloc(sizeof(struct FieldList));
    field->name = name;
    field->type = new_type;
    field->next = NULL;
    return field;
}

// 传入一个数组的 VarDec 结点（最上层的）
// 除了最上层的 VarDec 结点 其他都创建 array field
FieldList create_array_field(Node* node, Node* specifier) {
    // printf("Create Array Field\n");
    assert(!strcmp(node->id, "VarDec"));
    if (!strcmp(node->child->id, "ID")) {  // 最底层 VarDec
        // printf("最底层\n");
        FieldList field = malloc(sizeof(struct FieldList));
        field->name = node->child->data.text;
        Type new_type = create_array_type(node->sibling->sibling->data.i);
        field->type = new_type;
        return field;
    } else if (!strcmp(node->child->id, "VarDec")) {
        FieldList field;
        Type new_type;
        if (node->sibling == NULL || strcmp(node->sibling->id, "LB") != 0) {
            // 最上层的 VarDec 增加一个最深层的 type
            // 两种情况 basic 或 struct
            // printf("最上层\n");
            field = create_array_field(node->child, specifier);
            if (!strcmp(specifier->child->id, "TYPE")) {
                new_type = create_basic_type(specifier);
            } else if (!strcmp(specifier->child->id, "StructSpecifier")) {
                new_type = create_struct_type(specifier);
            }
        } else {
            // 中间层 VarDec
            // printf("中间层\n");
            field = create_array_field(node->child, specifier);
            new_type = create_array_type(node->sibling->sibling->data.i);
        }
        Type prev_type = field->type;
        // find the deepest array.elem which is null
        while (prev_type->u.array.elem != NULL) {
            prev_type = prev_type->u.array.elem;
        }
        prev_type->u.array.elem = new_type;
        return field;
    }
    // impossible to execute
    return NULL;
}


// Create fields end ---------------------------------------------------------------------