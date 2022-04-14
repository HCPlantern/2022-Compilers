# 编译原理 实验1报告

组长姓名：林浩然，学号：201250184，邮箱：201250184@smail.nju.edu.cn

组员姓名：邓尤亮，学号：201250035，邮箱：201250035@smail.nju.edu.cn

## 功能实现部分

基本内容：完成3.1.1节实验要求的全部内容。



特色内容：对于只由int、float字面量以及运算符组成的表达式，我们支持（在lab2阶段还不支持，但为之后的lab保留了接口）进行常量折叠，常量折叠的结果存放在EXP文法节点的constant结构内。节点结构如下(node.h: 23-45)：

```c
typedef struct tree_node {
    int lineno;
    char* id;
    // 3 different types: string, int and float
    union data {
        char* text;
        uint32_t i;
        float f;
    } data;
    struct tree_node* child;
    struct tree_node* sibling;
    bool is_terminal;

    FieldList corresponding_field;
    struct _Type type;
    bool is_constant;
    
    union _Constant {
        uint32_t i;
        float f;
    } constant;
} Node;
```

在进行语义分析的同时，对于可以进行常量折叠的表达式，我们将会通过father.constant = exp1.constant op exp2.constant 的方式将子节点的常量传播到父节点。二元算术运算语义检查中的传播部分代码如下(semantic.c: 146)：

```c
if (exp1->is_constant && exp2->is_constant) { // only exps that consists of literals and ops could be constant.
    if (exp1->type.u.basic == T_INT) {
        set_val(father, exp1->type, true, exp1->constant.i + exp2->constant.i, 0, NULL);
    }
    else if (exp1->type.u.basic == T_FLOAT) {
        set_val(father, exp1->type, true, 0, 0, NULL);
    }
    else {
        assert(0);
    }
    return;
}
```



## 编译方式

在Code/目录下运行命令make，按照Makefile编译。