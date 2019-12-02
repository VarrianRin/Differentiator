#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys\stat.h>
#include <math.h>
#include "Enum.h"

// тех, количество объектов, упроститель для sin, cos, ln
/*
TreeOk, ненулевую инфу, цвета, nill,
*/

#define CHECKNULLPTR(ptr) if(!ptr) { printf("\n%s: NULL POINTER TO "#ptr"!\n", __PRETTY_FUNCTION__); return NULL; }
#define CHECKTREE(tree)   if(TreeOk(tree)) TreeDump(tree);

#define dL                  Diff(elem->left)
#define dR                  Diff(elem->right)
#define cL                  CopyElem(elem->left)
#define cR                  CopyElem(elem->right)
#define LN(right)           CreateElem(OPERATOR, OPER_LN, NULL, right)
#define COS(right)          CreateElem(OPERATOR, OPER_COS, NULL, right)
#define SIN(right)          CreateElem(OPERATOR, OPER_SIN, NULL, right)
#define VAR(ID)             CreateElem(VARIABLE, ID)
#define NUM(number)         CreateElem(NUMBER, number)
#define PLUS(left, right)   CreateElem(OPERATOR, OPER_ADD, left, right)
#define MINUS(left, right)  CreateElem(OPERATOR, OPER_SUB, left, right)
#define MUL(left, right)    CreateElem(OPERATOR, OPER_MUL, left, right)
#define DIV(left, right)    CreateElem(OPERATOR, OPER_DIV, left, right)
#define POW(left, right)    CreateElem(OPERATOR, OPER_POW, left, right)

#define OPER_ADD(elem)      Calculate(elem->left) + Calculate(elem->right)
#define OPER_SUB(elem)      Calculate(elem->left) - Calculate(elem->right)
#define OPER_MUL(elem)      Calculate(elem->left) * Calculate(elem->right)
#define OPER_DIV(elem)      Calculate(elem->left) / Calculate(elem->right)
#define OPER_POW(elem)      pow(Calculate(elem->left), Calculate(elem->right))
#define OPER_LN(elem)       POISON//ln(Calculate(elem->left))
#define OPER_SIN(elem)      POISON//sin(Calculate(elem->left))
#define OPER_COS(elem)      POISON//cos(Calculate(elem->left))

const int POISON     = -1073676287,
          ELEMSIZE   = 15,
          N_VAR      = 20,
          VARSIZE    = 20,
          NUMBER     = 0,
          OPERATOR   = 1,
          VARIABLE   = 2,
          NULL_POINT = 1,
          HEAD_ERROR = 2;

const char* str = "";

struct Variable {

    char* name;
    int value;
};

Variable VarList[N_VAR] = {};
int ID = 0;

struct Element {

    char     type;
    int      data;
    Element* right;
    Element* left;
};

struct Tree {

    Element* head;
};

int TreeDump(const Tree* tree);

char* WinReader (const char* FileName, int SizeFile);

int GetFileSize(const char* FileName);

int TreeInit(Tree* tree);

int TreeCoder(const Tree* tree, const char* FileName);

int TreeDecoder(Tree* tree, const char* FileName);

void Simplifier(Tree* tree);

Element* SimplifyElem(Element* elem);

int TreeKill(Tree* tree);

int SearchFunc(const char* name);

Element* Diff(const Element* elem);

Element* GetE();

Element* GetG(const char* data);

int main() {

    Tree tree = {};
    TreeInit(&tree);

    TreeDecoder(&tree, "TreeCode.txt");
    int SizeFile = GetFileSize("TreeCode.txt");
    char* code = WinReader("TreeCode.txt", SizeFile);
    tree.head = GetG(code);

    //Simplifier(&tree);
    TreeDump(&tree);

    /*Tree diftree = {};
    TreeInit(&diftree);

    diftree.head = Diff(tree.head);

    //Simplifier(&diftree);
    //SimplifyElem(diftree.head->right);

    TreeDump(&diftree);

    TreeCoder(&diftree, "DiffTreeCode.txt");

    TreeKill(&diftree); */
    TreeKill(&tree);

    return 0;
}

char* WinReader (const char* FileName, int SizeFile) {

    assert(FileName != NULL);

    FILE* TextFile = fopen(FileName, "rb");
    assert(TextFile != NULL);

    char* text  = (char*) calloc(SizeFile + 1, sizeof(text[0]));

    assert(text != NULL);

    int ReadReturn = fread(text, sizeof(text[0]), SizeFile, TextFile);
    assert (ReadReturn == SizeFile);
    fclose(TextFile);

    text[SizeFile] = '\0';

    return text;
}

int GetFileSize(const char* FileName) {

    assert(FileName != NULL);

    struct _stat buff = {};
    int StatReturn = _stat(FileName, &buff);
    assert(StatReturn == 0);

    return buff.st_size;
}

int TreeOk(const Tree* tree) {

    if (!tree)
        return NULL_POINT;

    if (!tree->head)
        return HEAD_ERROR;

    return 0;
}

bool TypeDump(const Element* elem, FILE* TreeDump) {

    if (elem->type == OPERATOR) {

        #define DEF_OPER(operator, name, code) case code:\
                                             fprintf(TreeDump, "\""#operator"\\nelem: %p\\nleft: %p\\nright: %p\"", elem, elem->left, elem->right);\
                                             break;
        switch(elem->data) {
            #include "Operators.h"
        }

        #undef DEF_OPER
    }

    else if (elem->type == NUMBER)
        fprintf(TreeDump, "\"number: %d\\nelem: %p\\nleft: %p\\nright: %p\"", elem->data, elem, elem->left, elem->right);

    else if (elem->type == VARIABLE)
        fprintf(TreeDump, "\"%s\\nelem: %p\\nleft: %p\\nright: %p\"", VarList[elem->data].name, elem, elem->left, elem->right);

    return 1;
}

void ColorDump(const Element* elem, FILE* TreeDump) {

    if (elem->type == OPERATOR) {

        #define DEF_OPER(operator, name, code) case code:\
                                             fprintf(TreeDump, "\n\""#operator"\\nelem: %p\\nleft: %p\\nright: %p\"[style=\"filled\",fillcolor=\"deeppink\"];\n", elem, elem->left, elem->right);\
                                             break;
        switch(elem->data) {
            #include "Operators.h"
        }

        #undef DEF_OPER
    }

    else if (elem->type == NUMBER)
        fprintf(TreeDump, "\n\"number: %d\\nelem: %p\\nleft: %p\\nright: %p\"[style=\"filled\",fillcolor=\"lightslateblue\"];\n", elem->data, elem, elem->left, elem->right);

   else if (elem->type == VARIABLE)
        fprintf(TreeDump, "\n\"%s\\nelem: %p\\nleft: %p\\nright: %p\"[style=\"filled\",fillcolor=\"yellow2\"];\n", VarList[elem->data].name, elem, elem->left, elem->right);


}

bool ElemDump(const Element* elem, FILE* TreeDump) {

    TypeDump(elem, TreeDump);
    ColorDump(elem, TreeDump);
    TypeDump(elem, TreeDump);

    if (elem->left) {
        fprintf(TreeDump, "->");
        ElemDump(elem->left, TreeDump);
    }

    fprintf(TreeDump, "\n");
    TypeDump(elem, TreeDump);

    if (elem->right) {
        fprintf(TreeDump, "->");
        ElemDump(elem->right, TreeDump);
    }

    return 1;
}

int TreeDump(const Tree* tree) {

    CHECKNULLPTR(tree)

    FILE* TreeDump = fopen("TreeDump.txt", "w");
    if (!TreeDump) {
        printf("\n COULD NOT OPEN DUMP FILE\n");
        return 0;
    }

    fprintf(TreeDump, "digraph TreeDump {\n");

    ElemDump(tree->head, TreeDump);

    fprintf(TreeDump, "\n}");

    fclose(TreeDump);

    system("dot -Tjpg -o F:\\proga\\Differentiator\\TreeDump.jpg F:\\proga\\Differentiator\\TreeDump.txt");
    system("F:\\proga\\Differentiator\\TreeDump.jpg");
    return 1;
}

int TreeInit(Tree* tree) {

    CHECKNULLPTR(tree)

    Element* reserve = (Element*)calloc(1, sizeof(Element));
    if (!reserve) {
        printf("COULD NOT CALLOC HEAD");
        return 0;
    }
    tree->head = reserve;

    CHECKTREE(tree)
    return 1;
}

int VarId(const char* name) {

    for(int i = 0; i < ID; i++)
        if ( !strcmp(VarList[i].name, name) )
            return i;
    return -1;
}

int ChangeData(Element* elem, const char* data) {

    CHECKNULLPTR(elem)
    CHECKNULLPTR(data)

    #define DEF_OPER(operator, name, code)     if ( !strcmp(#operator, data) ) {  \
                                               elem->type = OPERATOR;        \
                                               elem->data = code;            \
                                               return OPERATOR;              \
                                           }
    #include "Operators.h"

    #undef DEF_OPER

    if (isdigit(*data)) {

        elem->type = NUMBER;
        elem->data = atoi(data);
        return NUMBER;
    }

    else if (isalpha(*data)) {

        elem->type = VARIABLE;
        int id = VarId(data);
        elem->data = (id == -1) ? ID : id;

        if (id == -1)
            VarList[ID++].name = strdup(data);
        return VARIABLE;
    }

    return -1;
}

Element* AddRightElem(Element* elem, const char* data) {

    CHECKNULLPTR(elem)
    CHECKNULLPTR(data)

    Element* son = (Element*)calloc(1, sizeof(Element));

    if (data[0])
        ChangeData(son, data);

    elem->right = son;

    return son;
}

Element* AddLeftElem(Element* elem, const char* data) {

    CHECKNULLPTR(elem)
    CHECKNULLPTR(data)

    Element* son = (Element*)calloc(1, sizeof(Element));

    ChangeData(son, data);

    elem->left = son;

    return son;
}

bool TypeCoder(const Element* elem, FILE* TreeCode) {

    if (elem->type == OPERATOR) {

        #define DEF_OPER(operator, name, code) case code:\
                                             fprintf(TreeCode, #operator);\
                                             break;
        switch(elem->data) {
            #include "Operators.h"
        }

        #undef DEF_OPER
    }

    else if (elem->type == NUMBER)
        fprintf(TreeCode, "%d", elem->data);

    else if (elem->type == VARIABLE)
        fprintf(TreeCode, "%s", VarList[elem->data].name);

    return 1;
}

bool Coder(const Element* elem, FILE* TreeCode) {

    fprintf(TreeCode, "(");

    if (elem->left)
        Coder(elem->left, TreeCode);

    TypeCoder(elem, TreeCode);

    if (elem->right)
        Coder(elem->right, TreeCode);

    fprintf(TreeCode, ")");

    return 1;
}

int TreeCoder(const Tree* tree, const char* FileName) {

    CHECKTREE(tree)
    CHECKNULLPTR(FileName)

    FILE* TreeCode = fopen(FileName, "w");
    if (!TreeCode) {
        printf("COULD NOT OPEN FILE");
        return 0;
    }

    Coder(tree->head, TreeCode);

    fclose(TreeCode);
    CHECKTREE(tree)
    return 1;
}

bool Decoder(Element* elem, const char* TreeCode, int* offset) {///////WITHOUT SPACES!!

    char symbol         = 0;
    int  n_bytes        = 0;
    char data[ELEMSIZE] = {};
    Element* root       = NULL;

    sscanf(TreeCode + *offset, "(%n", &n_bytes);
    *offset += n_bytes;
    sscanf(TreeCode + *offset, "%c%n", &symbol, &n_bytes);

    if (symbol == '(') {
        root = AddLeftElem(elem, "\0");
        Decoder(root, TreeCode, offset);
    }

    sscanf(TreeCode + *offset, "%[^()]%n", data, &n_bytes); //символ пробела?
    *offset += n_bytes;
    ChangeData(elem, data);

    sscanf(TreeCode + *offset, "%c%n", &symbol, &n_bytes);

    if (symbol == '(') {

        root = AddRightElem(elem, "\0");
        Decoder(root, TreeCode, offset);
    }
    else if (symbol == ')') {
        *offset += n_bytes;
        return 0;
    }

    sscanf(TreeCode + *offset, ")%n", &n_bytes);
    *offset += n_bytes;
    return 0;
}


int TreeDecoder(Tree* tree, const char* FileName) {

    CHECKTREE(tree)
    CHECKNULLPTR(FileName)

    int SizeFile = GetFileSize(FileName);
    char* code = WinReader(FileName, SizeFile);

    int offset = 0;
    Decoder(tree->head, code, &offset);

    free(code);
    CHECKTREE(tree)
    return 1;
}

int Search(const Element* elem, int type, int data = POISON) {

    CHECKNULLPTR(elem)
    CHECKNULLPTR(data)

    if (data != POISON) {

        if (elem->type == type && elem->data == data)
            return 2;

        int ret = 0;

        if (elem->left && (ret = Search(elem->left, data)) != 0)
            return 10 * ret + 1;

        if (elem->right && (ret = Search(elem->right, data)) != 0)
            return 10 * ret;
    }
    else {

        if (elem->type == type)
            return 2;

        int ret = 0;

        if (elem->left && (ret = Search(elem->left, type)) != 0)
            return 10 * ret + 1;

        if (elem->right && (ret = Search(elem->right, type)) != 0)
            return 10 * ret;
    }
    return 0;
}

Element* CreateElem(char type, int data, Element* left = nullptr, Element* right= nullptr) {

    Element* elem = (Element*)calloc(1, sizeof(Element));

    elem->type = type;
    elem->data = data;

    elem->left = left;
    elem->right = right;

    return elem;
}

Element* CopyElem(Element* elem) {

    return (elem) ? CreateElem(elem->type, elem->data, cL, cR) : NULL;
}

Element* Diff(const Element* elem) {

    switch(elem->type) {

        case NUMBER:
            return NUM(0);

        case VARIABLE:
            return NUM(1);

        case OPERATOR:

            int var_l = Search(elem->left, VARIABLE);
            int var_r = Search(elem->right, VARIABLE);

            if (!var_l && !var_r)
                return NUM(0);

            switch(elem->data) {

                case OPER_ADD:
                    if ( var_l && !var_r) return dL;
                    if (!var_l &&  var_r) return dR;
                    return PLUS(dL, dR);

                case OPER_SUB:
                    if ( var_l && !var_r) return dL;
                    if (!var_l &&  var_r) return MUL( NUM(-1), dR);
                    return MINUS(dL, dR);

                case OPER_MUL:

                    if ( var_l && !var_r) return MUL(dL, cR);
                    if (!var_l &&  var_r) return MUL(cL, dR);
                    return PLUS( MUL(dL, cR), MUL(cL, dR));

                case OPER_DIV:

                    if ( var_l && !var_r) return DIV(dL, cR);
                    if (!var_l &&  var_r) return DIV( MINUS( NUM(0), MUL(cL, dR)), MUL(cR, cR));
                    return DIV( MINUS( MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));

                case OPER_POW:

                    if ( var_l && !var_r) return MUL( MUL( POW(cL, MINUS(cR, NUM(1))), cR), dL);
                    if (!var_l &&  var_r) return MUL( MUL( POW(cL, cR), LN(cL)), dR);
                    return MUL( POW(cL, cR), PLUS( MUL( DIV(dL, cL), cR), MUL( LN(cL), dR)));

                case OPER_LN:
                    return DIV(dR, cR);

                case OPER_SIN:
                    return MUL(COS(cR), dR);

                case OPER_COS:
                    return MUL(SIN(cR), dR);
        }
    }

    return NULL;
}

bool KillElem(Element* elem) {//doesnt free elem

    if (elem->left) {

        KillElem(elem->left);

        free(elem->left);
        elem->left = NULL;
    }

    if (elem->right) {

        KillElem(elem->right);

        free(elem->right);
        elem->right = NULL;
    }

    elem->data = 0;
    elem->type = 0;

    return 0;
}

int TreeKill(Tree* tree) {

    CHECKTREE(tree)

    KillElem(tree->head);

    free(tree->head);
    tree->head = NULL;

    return 1;
}

int ElemDup(Element* p_elem, Element* c_elem) {

    CHECKNULLPTR(p_elem)
    CHECKNULLPTR(c_elem)

    p_elem->type = c_elem->type;
    p_elem->data = c_elem->data;
    p_elem->left = c_elem->left;
    p_elem->right = c_elem->right;

    return 1;
}

int Calculate(Element* elem) {
    //printf("'%d', %d\n", elem->type, elem->data);
    CHECKNULLPTR(elem)

    if (elem->type == OPERATOR) {

        #define DEF_OPER(operator, name, code) case code:\
                                                    return OPER_##name(elem);
        switch(elem->data) {
            #include "Operators.h"
        }

        #undef DEF_OPER
    }
    else if (elem->type == NUMBER)
        return elem->data;

    return POISON;
}

void KillSons(Element* elem, int number = -1, char ConSon = 0) {
    //printf("%d\n", number);
    if (number == VARIABLE) {

        if (ConSon == 'r') {

            KillElem(elem->right);
            Element* exleft = elem->left;
            ElemDup(elem, exleft);
            free(exleft); //elem-left to elem
        }
        else if (ConSon == 'l') {

            KillElem(elem->left);
            Element* exright = elem->right;
            ElemDup(elem, exright); //elem-right to elem
            free(exright);
        }
    }
    else {

        if (number == -1)
            elem->data = Calculate(elem);
        else
            elem->data = number;

        elem->type = NUMBER;

        KillElem(elem->right);
        KillElem(elem->left);

        free(elem->right);
        free(elem->left);
        elem->right = NULL;
        elem->left = NULL;
    }
    //printf("%d\n", elem->data);
}

Element* ObviElem(Element* elem, char ConSon) {

    if (ConSon == 'r') {
        //printf("%d, %d\n", elem->right->data, elem->data);
        //elem = a: a+0, a-0, a*1, a/1, a^1
        if ((elem->right->data == 0 && (elem->data == OPER_SUB || elem->data == OPER_ADD)) || (elem->right->data == 1 && (elem->data == OPER_MUL || elem->data == OPER_DIV || elem->data == OPER_POW)))
            KillSons(elem, VARIABLE, 'r');
        //elem = 0: a*0
        else if (elem->right->data == 0 && elem->data == OPER_MUL)
            KillSons(elem, 0);
        //elem = 1: a^0
        else if (elem->right->data == 0 && elem->data == OPER_POW)
            KillSons(elem, 1);

        return elem;
    }
    else if (ConSon == 'l') {
        //printf("%d, %d\n", elem->left->data, elem->data);
        //elem = 0: 0*a, 0/a
        if (elem->left->data == 0 && (elem->data == OPER_MUL || elem->data == OPER_DIV))
            KillSons(elem, 0);
        //elem = a: 0+a, 1*a
        else if ((elem->left->data == 0 && elem->data == OPER_ADD) || (elem->left->data == 1 && elem->data == OPER_MUL))
            KillSons(elem, VARIABLE, 'l');
        return elem;
    }

    return NULL;
}

Element* SimplifyElem(Element* elem) {

    if (elem->type == OPERATOR  && elem->data != OPER_LN && elem->data != OPER_SIN && elem->data != OPER_COS) {

        int var_l = Search(elem->left, VARIABLE);
        int var_r = Search(elem->right, VARIABLE);
        /*
        printf("%p", elem);
        printf("'%d', '%d'\n", var_l, var_r);*/

        if (!var_l &&  !var_r)
            KillSons(elem);

        else if (!var_l) {

            if (elem->left->left && elem->left->right)
                KillSons(elem->left);
            ObviElem(elem, 'l');
        }

        else if (!var_r) {

            if (elem->right->left && elem->right->right)
                KillSons(elem->right);
            ObviElem(elem, 'r');
        }
    }

    if (elem->right)
        SimplifyElem(elem->right);
    if (elem->left)
        SimplifyElem(elem->left);

    return elem;
}

void Simplifier(Tree* tree) {

    CHECKTREE(tree)

    SimplifyElem(tree->head);
}

int SearchFunc(const char* func) {

    CHECKNULLPTR(func)

    #define DEF_OPER(operator, name, code)     if (code > 19 && !strcmp(#operator, func) )\
                                               return code;               //функции идут с кодом от 20
    #include "Operators.h"

    #undef DEF_OPER

    return 0;
}

Element* GetId() {

    char name[VARSIZE] = {};

    for(int i = 0; isalpha(*str); i++, str++)
        name[i] = *str;

    if (*str == '(') {      //if function

        str++;
        Element* node = GetE();

        assert(*str == ')');
        str++;
        int code = SearchFunc(name);
        return CreateElem(OPERATOR, code, NULL, node);
    }

    int id = VarId(name);

    if (id == -1) {
        VarList[ID].name = strdup(name);
        return VAR(ID++);
    }
    else if (id >= 0)
        return VAR(id);

}

Element* GetN() {

    char val = 0;
    assert(isdigit(*str));

    while (isdigit(*str)) {

        val = val * 10 + *str - '0';
        str++;
    }
    return NUM(val);
}

Element* GetP() {

    if (*str == '(') {

        str++;
        Element* node = GetE();

        assert(*str == ')');
        str++;
        return node;
    }
    else if (isalpha(*str))
        return GetId();
    else
        return GetN();
}

Element* GetPow() {

    Element* node = GetP();

    while (*str == '^') {
        str++;
        node = POW(node, GetP());
    }

    return node;
}

Element* GetT() {

    Element* node = GetPow();

    while (*str == '*' || *str == '/') {

        char oper = *str;
        str++;

        if (oper == '*')
            node = MUL(node, GetPow());
        else
            node = DIV(node, GetPow());
    }

    return node;
}

Element* GetE() {

    Element* node = GetT();

    while (*str == '+' || *str == '-') {

        char oper = *str;
        str++;

        if(oper == '+')
            node = PLUS(node, GetT());
        else
            node = MINUS(node, GetT());
    }

    return node;
}

Element* GetG(const char* data) {

    str = data;
    Element* node = GetE();
    assert(*str == '\0');

    str++;

    return node;
}

