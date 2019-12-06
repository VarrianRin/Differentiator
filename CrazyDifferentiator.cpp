#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys\stat.h>
#include <math.h>
#include "Enum.h"
#include <locale.h>

/*
TreeOk, íĺíóëĺâóţ číôó, öâĺňŕ, nill,
*/

#define CHECKNULLPTR(ptr) if(!ptr) { /*printf("\n%s: NULL POINTER TO "#ptr"!\n", __PRETTY_FUNCTION__); */return NULL; }
#define CHECKTREE(tree)   if(TreeOk(tree)) TreeDump(tree);

#define dL                  Diff(elem->left, TreeTex)
#define dR                  Diff(elem->right, TreeTex)
#define cL                  CopyElem(elem->left)
#define cR                  CopyElem(elem->right)
#define LN(right)           CreateElem(FUNCTION, OPER_LN, NULL, right)
#define COS(right)          CreateElem(FUNCTION, OPER_COS, NULL, right)
#define SIN(right)          CreateElem(FUNCTION, OPER_SIN, NULL, right)
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
          VARIABLE   = 1,
          OPERATOR   = 2,
          FUNCTION   = 3,
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

void Tex(const Element* elem, FILE* TreeTex);

int TreeDecoder(Tree* tree, const char* FileName);

void Simplifier(Tree* tree);

Element* SimplifyElem(Element* elem);

int TreeKill(Tree* tree);

Element* Diff(const Element* elem, FILE* TreeTex);

bool TexMake(Tree* tree, FILE* TreeTex);

Element* GetE();

Element* GetG(const char* data);

int main() {

    //setlocale(LC_ALL, "Rus");
    //system("chcp 1251");

    Tree tree = {};
    TreeInit(&tree);

    int SizeFile = GetFileSize("TreeCode.txt");
    char* code = WinReader("TreeCode.txt", SizeFile);
    tree.head = GetG(code);

    FILE* TreeTex = fopen("TreeTex.tex", "wb");
    //TreeDump(&tree);
    //Tex(tree.head, TreeTex);
    TexMake(&tree, TreeTex);
    //Simplifier(&diftree);
    //TreeDump(&diftree);
    fclose(TreeTex);
    TreeKill(&tree);

    return 0;
}

bool Lbracket(const Element* elem) {

    if (elem->left->type == VARIABLE || elem->left->type == NUMBER)
        return 0;

    if (elem->data / 2 < elem->left->data / 2)
        return 0;

    if (elem->data == OPER_SUB || elem->data == OPER_ADD)
        return 0;

    return 1;
}

bool Rbracket(const Element* elem) {

    if (elem->right->type == VARIABLE || elem->right->type == NUMBER)
        return 0;

    if (elem->data / 2 < elem->right->data / 2)
        return 0;
    if (elem->data == OPER_ADD)
        return 0;

    return 1;
}

void TypeTex(const Element* elem, FILE* TreeCode) {

    if (elem->type == OPERATOR || elem->type == FUNCTION) {

        if (elem->data == OPER_MUL)
            fprintf(TreeCode, "\\cdot ");

        else {
                #define DEF_OPER(operator, name, code) case code:                \
                                                    fprintf(TreeCode, #operator);\
                                                    break;
            switch(elem->data) {
                #include "Operators.h"
            }

            #undef DEF_OPER
        }
    }

    else if (elem->type == NUMBER)
        fprintf(TreeCode, "%d", elem->data);

    else if (elem->type == VARIABLE)
        fprintf(TreeCode, "%s", VarList[elem->data].name);
}

void Tex(const Element* elem, FILE* TreeTex) {

    if (elem->left) {
        int bracket = Lbracket(elem);
        if (bracket) fprintf(TreeTex, "(");
        Tex(elem->left, TreeTex);
        if (bracket) fprintf(TreeTex, ")");
    }

    TypeTex(elem, TreeTex);

    if (elem->right != NULL && elem->data != OPER_POW) {
        int bracket = Rbracket(elem);
        if (bracket) fprintf(TreeTex, "(");
        Tex(elem->right, TreeTex);
        if (bracket) fprintf(TreeTex, ")");
    }
    else if (elem->data == OPER_POW) {
        fprintf(TreeTex, "{");
        Tex(elem->right, TreeTex);
        fprintf(TreeTex, "}");
    }
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

    if (elem->type == OPERATOR || elem->type == FUNCTION) {

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

    if (elem->type == OPERATOR || elem->type == FUNCTION) {

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

    #define DEF_OPER(operator, name, code)     if ( !strcmp(#operator, data) ) {                    \
                                               elem->type = (code > 19) ? FUNCTION : OPERATOR;      \
                                               elem->data = code; /*ń 20 čäóň ôóíęöčč */            \
                                               return OPERATOR;                                     \
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

    if (elem->type == OPERATOR || elem->type == FUNCTION) {

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

    sscanf(TreeCode + *offset, "%[^()]%n", data, &n_bytes); //ńčěâîë ďđîáĺëŕ?
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
        //printf("%d, ", elem->type);
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

///////ęŕđňčíęč

void OnePicture(FILE* TreeTex, const char* who) {

    fprintf(TreeTex, "\n");

    fprintf(TreeTex, "\n\\includegraphics[width = 1\\linewidth]{%s.jpg}", who);
    fprintf(TreeTex, "\n");
}

void ThreePicture(FILE* TreeTex, const char* who) {

    fprintf(TreeTex, "\n");

    int ran = rand() % 3;

    fprintf(TreeTex, "\n\\includegraphics[width = 1\\linewidth]{%s%d.jpg}", who, ran + 1);
    fprintf(TreeTex, "\n");
}

void Petrovich(const Element* elem, FILE* TreeTex, int var_l, int var_r) {


        ThreePicture(TreeTex, "Petrovich");
        fprintf(TreeTex, "\n");

        int ran = rand() % 3;

        if (ran == 0)
            fprintf(TreeTex, "\nĎĺňđîâč÷: \"Îď˙ňü íĺ ěîăóň âç˙ňü ďđîńňĺéřóţ ďđîčçâîäíóţ! Ĺńëč âű íĺ âîńďđčěĺňĺ ňîăî, ÷ňî ˙ ńĺé÷ŕń âŕě ńęŕćó.... ŕ íó č áîă ń íčě! \"\n");

        if (ran == 1)
            fprintf(TreeTex, "\nĎĺňđîâč÷: \"Äŕ ňóň ćĺ ĺćó ďîí˙ňíî, ÷ňî \"\n");

        if (ran == 2)
            fprintf(TreeTex, "\nĎĺňđîâč÷: \"Íĺ çŕíčěŕéňĺńü ÷ëĺíîâđĺäčňĺëüńňâîě! Ňóň âń¸ î÷ĺâčäíî \"\n");

        if (elem->type == FUNCTION) {

            switch(elem->data) {
                case OPER_COS:
                    fprintf(TreeTex, "\n$$cos(x)^\\prime = -sin(x)$$\n");
                    break;

                case OPER_SIN:
                    fprintf(TreeTex, "\n$$sin(x)^\\prime = cos(x)$$\n");
                    break;

                case OPER_LN:
                    fprintf(TreeTex, "\n$$ln(X)^\\prime = 1/x$$\n");
                    break;
            }
        }
        else
            fprintf(TreeTex, "\n $$ u^v = e^vlnu$$"
                             "$$ (e^{v*lnu})^\\prime = e^{v*lnu}*(ln(u) +v/u)$$\n");

        fprintf(TreeTex, "\nĎĺňđîâč÷: \"Č íĺ çŕáűâŕéňĺ äîěíîćŕňü íŕ ďđîčçâîäíóţ âűđŕćĺíč˙ ďîä ôóíęöčĺé \"\n");
        fprintf(TreeTex, "\nÄŕřŕ: \"íč÷ĺăî íĺ ďîí˙ëŕ, íî î÷ĺíü číňĺđĺńíî!\"\n\n");
}

void Botan(const Element* elem, FILE* TreeTex, int var_l, int var_r) {    /////////ęŕđňčíęč

        fprintf(TreeTex, " ");

        OnePicture(TreeTex, "Botan");

        int ran = rand() % 4;

        if (ran == 0)
            fprintf(TreeTex, "\nÁîňŕí: \"Äŕřŕ ˙ ńďŕńó ňĺá˙.\"\n");

        if (ran == 1)
            fprintf(TreeTex, "\nÁîňŕí: \"Ýő ďîäđóăŕ, äčôôĺđĺíöčđîâŕíčĺ ýňî ďđîńňî.\"\n");

        if (ran == 2)
            fprintf(TreeTex, "\nÁîňŕí: \"Íčęîăäŕ íĺ îńňŕâëţ äĺâóřęó â áĺäĺ.\"\n");

        if (ran == 3)
            fprintf(TreeTex, "\nÁîňŕí: \"ß çŕęîí÷čë óćĺ 3 ęëŕńń, ďîýňîěó çíŕţ âńĺ ôîđěóëű ďđîčçâîäíűő.\"\n");

        if (elem->data == OPER_POW || elem->type == FUNCTION) {

            if (var_l && var_r) {

                OnePicture(TreeTex, "SadBotan");

                fprintf(TreeTex, "\nÁîňŕí: \"Íč÷ĺăî íĺ ďîéěó ďĺđĺěĺííŕ˙ č ňŕě č ňŕě, íî âĺäü Ěŕđü˙ Čâŕíîâíŕ ăîâîđčëŕ, ÷ňî ýňî íĺâîçěîćíî!!"
                                     "Óâű, íî ˙ íĺ ńěîăó ýňî đĺřčňü\"\n");
                Petrovich(elem, TreeTex, var_l, var_r);
            }
            else if (elem->type == FUNCTION) {

                OnePicture(TreeTex, "SadBotan");

                fprintf(TreeTex, "\nÁîňŕí: \"\Äčôôĺđĺíöčđîâŕíčĺ ôóíęöčé?! ß ęîíĺ÷íî ÷čňŕë î ňŕęîě â çŕďđĺňíîé ęíčăĺ, íî č ďđĺäńňŕâčňü ńĺáĺ íĺ ěîă, ÷ňî ęîăäŕ-íčáóäü ń ňŕęčě âńňđĺ÷óńü!\"\n");
                Petrovich(elem, TreeTex, var_l, var_r);
            }
            else if (var_l)
                fprintf(TreeTex, "\n $$(x^a)^\\prime = a\\cdot x^{a-1}$$\n");

            else if (var_r)
                fprintf(TreeTex, "\n $$(a^x)^\\prime = a^x\\cdot ln(a)$$\n");
        }

        else {
            switch(elem->data) {

                case OPER_ADD:
                    fprintf(TreeTex, "\n$$(a+b)^\\prime = a^\\prime+b^\\prime$$\n");
                    break;

                case OPER_SUB:
                    fprintf(TreeTex, "\n$$(a-b)^\\prime = a^\\prime-b^\\prime$$\n");
                    break;

                case OPER_MUL:
                    fprintf(TreeTex, "\n$$(a*b)^\\prime = a^\\prime\\cdot b+b^\\prime\\cdot a$$\n");
                    break;

                case OPER_DIV:
                    fprintf(TreeTex, "\n$$(a/b)^\\prime = (a^\\prime\\cdot b-b^\\prime\\cdot a)/b^2$$\n");
                    break;
            }
        }
        fprintf(TreeTex, "\n");
}

void OperTexBeg(const Element* elem, FILE* TreeTex, int var_l, int var_r) {

        ThreePicture(TreeTex, "SadDasha");
        fprintf(TreeTex, "\n\n\nŇĺďĺđü íŕéä¸ě ďđîčçâîäíóţ $");

        Tex(elem, TreeTex);
        fprintf(TreeTex, "$\n");

        int ran = rand() % 5;

        if (ran == 0)
            fprintf(TreeTex, "\nÄŕřŕ: \"×ŇÎ!? Ňŕęîěó ěĺí˙ â äĺňńęîě ńŕäó íĺ ó÷čëč! ß âűçűâŕţ áîňŕíŕ!\"\n");

        if (ran == 1)
            fprintf(TreeTex, "\nÄŕřŕ: \"Ýő, íĺ ó÷čëč ěĺí˙ ňŕęîěó ĺůĺ, âîň ďîéäó â 1 ęëŕńń ňŕě îá˙çŕňĺëüíî óçíŕţ, ŕ ďîęŕ áîňŕí - ďđčäč!\"\n");

        if (ran == 2)
            fprintf(TreeTex, "\nÄŕřŕ: \"Íó ýňî ˙âíî óćĺ âűńřŕ˙ ěŕňčěŕňčęŕ ňŕęîĺ ďîä ńčëó ňîëüęî áîňŕíŕě\"\n");

        if (ran == 3)
            fprintf(TreeTex, "\nÄŕřŕ: \"Íĺ ďîíčěŕţ. Íó ˙ ęîíĺ÷íî íĺ ńŕěŕ˙ óěíŕ˙, íî î÷ĺíü îáŕ˙ňĺëüíŕ˙, ó ěĺí˙ âńĺăäŕ ďîä đóęîé íŕéä¸ňń˙ ëčříčé áîňŕí\"\n");

        if (ran == 4)
            fprintf(TreeTex, "\nÄŕřŕ: \"Âîîáůĺ ýňŕ ěŕňĺěŕňĺęŕ íĺ ńîçäŕíŕ äë˙ íŕńňî˙ůčő äĺâóřĺę, ďóńęŕé ĺé çŕíčěŕţňń˙ ýňč áîňŕíű\"\n");

        Botan(elem, TreeTex, var_l, var_r);
}

void OperTexEnd(const Element* elem, FILE* TreeTex, int var_l, int var_r) {   //////////////
    /*
    fprintf(TreeTex, "\nŇî ĺńňü ďîëó÷ŕĺě $");
    Tex(elem, TreeTex);

    fprintf(TreeTex, "$ Äčôôĺđĺíöčđóĺě äŕüëřĺ\n");

    if (!var_l)
        fprintf(TreeTex, "\nas we can see a - const so a' = 0 which means that our formula is much easier");

    if (!var_r)
        fprintf(TreeTex, "\nas we can see b - const so b' = 0 which means that our formula is much easier");

    fprintf(TreeTex, "\n");     */
}

void NumTex(int number, FILE* TreeTex) {

    fprintf(TreeTex, "\n\n\nŇĺďĺđü ŕéä¸ě ďđîčçâîäíóţ %d\n", number);

    int ran = rand() % 5;

    if (ran == 0)
        fprintf(TreeTex, "\nÍó äčôôĺđĺíöčđîŕňü %d íĺ číňĺđĺńíî, äŕćĺ Äŕřŕ çíŕĺň ÷ňî ďđîčçâîäíŕ˙ ęîíńňŕíňű - íîëü\n", number);

    if (ran == 1)
        fprintf(TreeTex, "\nÂ ńëó÷ŕĺ %d Äŕřĺ äŕćĺ ďîěîůü íĺ íóćíŕ, âńĺ çíŕţň, ÷ňî ďđîčçâîäíŕ˙ %d - íîëü\n", number, number);

    if (ran == 2)
        fprintf(TreeTex, "\nÄŕřŕ - ńčëüíŕ˙ č íĺçŕâčńčěŕ˙ äĺâóřęŕ, ĺé íĺ íóćíŕ ďîěîůü ęŕęčő-ňî ěóć÷čí â íŕőîćäĺíčč ďđîčçâäíîé %d\n", number);

    if (ran == 3)
        fprintf(TreeTex, "\nĎîëó÷čâ ďđîčçâîäíóţ %d Äŕřŕ ňŕę îáđŕäîâŕëŕńü - íčęîăäŕ íĺ äóěŕëŕ, ÷ňî ěŕňŕí ňŕęîé ďđîńňîé\n", number);

    if (ran == 4)
        fprintf(TreeTex, "\nÁűńňđî óńňŕíîâčâ, ÷ňî ďđîčçâîäíŕ˙ %d - íîëü, Äŕřŕ îňďđŕâë˙ĺňń˙ äŕëüřĺ\n", number);
}

void VarTex(int var, FILE* TreeTex) {

    fprintf(TreeTex, "\n\n\nŇĺďĺđü íŕéä¸ě ďđîčçâîäíóţ %s\n", VarList[var].name);

    ThreePicture(TreeTex, "Dasha");

    int ran = rand() % 4;

    if (ran == 0)
        fprintf(TreeTex, "\nÄŕřŕ: \"ß č áĺç âŕń çíŕţ ÷ňî ďđîčçâîäíŕ˙ ďĺđĺěĺííîé - îäčí\" - ńęŕçŕëŕ Äŕřŕ, ňîďíóâ íîćęîé č îňďđŕâčâřčńü äŕëüřĺ\n");

    if (ran == 1)
        fprintf(TreeTex, "\nÄŕřŕ: \"Ěíĺ íĺ íóćĺí áîňŕí ÷ňîáű ďîí˙ňü, ÷ňî ďđîčçâîäíŕ˙ ďĺđĺěĺííîé - 1\"\n");

    if (ran == 2)
        fprintf(TreeTex, "\nÄŕřŕ âűó÷čëŕ ôîđěóëó ďđîčçâîäíîé ďĺđĺěĺííîé íĺäĺëţ íŕçŕä č âîň îíŕ ďđčăîäčëŕńü\n");

    if (ran == 3)
        fprintf(TreeTex, "\nĎóńňü Äŕřŕ č âűăë˙äčň óâĺđĺííî, íî íŕ ńŕěîě äĺëĺ îíŕ ĺů¸ číîăäŕ ďóňŕĺň ďĺđĺěĺííűĺ ń ÷čńëŕěč, ňŕę ÷ňî çŕ íĺé ëó÷řĺ ńëĺäčňü\n");
}

Element* Diff(const Element* elem, FILE* TreeTex) { /////ęŕđňčíęč

    switch(elem->type) {

        case NUMBER:
            NumTex(elem->data, TreeTex);
            return NUM(0);

        case VARIABLE:
            VarTex(elem->data, TreeTex);
            return NUM(1);

        case OPERATOR: {

            int var_l = Search(elem->left, VARIABLE);
            int var_r = Search(elem->right, VARIABLE);

            OperTexBeg(elem, TreeTex, var_l, var_r);
            Element* node = NULL;

            if (!var_l && !var_r)
                return NUM(0);

            switch(elem->data) {

                case OPER_ADD:
                    if      ( var_l && !var_r) node = dL;
                    else if (!var_l &&  var_r) node = dR;
                    else node = PLUS(dL, dR);
                    OperTexEnd(elem, TreeTex, var_l, var_r);
                    return node;

                case OPER_SUB:
                    if      ( var_l && !var_r) node = dL;
                    else if (!var_l &&  var_r) node = MUL( NUM(-1), dR);
                    else node = MINUS(dL, dR);
                    OperTexEnd(elem, TreeTex, var_l, var_r);
                    return node;

                case OPER_MUL:
                    if      ( var_l && !var_r) node = MUL(dL, cR);
                    else if (!var_l &&  var_r) node = MUL(cL, dR);
                    else node = PLUS( MUL(dL, cR), MUL(cL, dR));
                    OperTexEnd(elem, TreeTex, var_l, var_r);
                    return node;

                case OPER_DIV:
                    if      ( var_l && !var_r) node = DIV(dL, cR);
                    else if (!var_l &&  var_r) node = DIV( MINUS( NUM(0), MUL(cL, dR)), MUL(cR, cR));
                    else node = DIV( MINUS( MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));
                    OperTexEnd(elem, TreeTex, var_l, var_r);
                    return node;

                case OPER_POW:
                    if      ( var_l && !var_r) node = MUL( MUL( POW(cL, MINUS(cR, NUM(1))), cR), dL);
                    else if (!var_l &&  var_r) node = MUL( MUL( POW(cL, cR), LN(cL)), dR);
                    else node = MUL( POW(cL, cR), PLUS( MUL( DIV(dL, cL), cR), MUL( LN(cL), dR)));
                    OperTexEnd(elem, TreeTex, var_l, var_r);
                    return node;
            }
        }

        case FUNCTION:

            int var_r = Search(elem->right, VARIABLE);
            OperTexBeg(elem, TreeTex, 0, var_r);
            Element* node = NULL;

            if (!var_r)
                return NUM(0);

            switch(elem->data) {

                case OPER_LN:
                    //fprintf(TreeTex, "\n(ln(x))' = 1/x * x'\n");
                    node = DIV(dR, cR);
                    OperTexEnd(elem, TreeTex, 0, var_r);
                    return node;

                case OPER_SIN:
                    //fprintf(TreeTex, "\n(sin(x))' = cos(x) * x'\n");
                    node = MUL(COS(cR), dR);
                    OperTexEnd(elem, TreeTex, 0, var_r);
                    return node;

                case OPER_COS:
                    //fprintf(TreeTex, "\n(cos(x))' = -sin(x) * x'\n");
                    node = MUL( MUL( NUM(-1), SIN(cR)), dR);
                    OperTexEnd(elem, TreeTex, 0, var_r);
                    return node;
            }
    }

    return NULL;
}

bool TexMake(Tree* tree, FILE* TreeTex) {

    CHECKNULLPTR(tree)

    fprintf(TreeTex, "\\documentclass{article}\n"
                     "\\usepackage[T2A,T1]{fontenc}\n"
                     "\\usepackage[cp1251]{inputenc}\n"
                     "\\usepackage[russian,english]{babel}\n"
                     "\\usepackage{graphicx}\n"
                     "\\graphicspath{}\n"
                     "\\DeclareGraphicsExtensions{.pdf,.png,.jpg}\n"
                     "\\begin{document}\n"
                     "\\begin{otherlanguage*}{russian}\n");
  /*\documentclass{article}
\usepackage{amsmath}
\usepackage{amsfonts,amssymb}
\usepackage{amsthm}
\usepackage[electronic]{ifsym}
\usepackage[cp1251]{inputenc}
\usepackage[matrix,arrow,curve,frame,poly,arc]{xy}
\usepackage[english, russian]{babel}
\usepackage[final]{graphicx}
\usepackage{mathrsfs}
\usepackage{color}
\usepackage{tikz}
\usepackage[nointegrals]{wasysym}
\pagestyle{empty}
\usepackage{graphicx}
\graphicspath{}
\DeclareGraphicsExtensions{.pdf,.png,.jpg}

\begin{document}




Äŕřŕ ďóňĺřĺńňâĺííčöŕ

\begin{figure}[h]
\center{\includegraphics[scale=0.5]{Dasha1.jpg}}
\label{fig:image}
\end{figure}


\end{document}*/

    OnePicture(TreeTex, "Dasha1");
    fprintf(TreeTex, "Äŕâŕéňĺ ďîěîćĺě Äŕřĺ čç ôčçňĺő-˙ńëĺé íŕéňč ďđîčçâîäíóţ čç âńňóďčňĺëüíîăî ýęçŕěĺíŕ â 1 ęëŕńń ôčçňĺő-řęîëű.\n"
                     "Äŕřŕ óćĺ çíŕĺň íĺęîđňîđűĺ ďđîčçâîäíűĺ, íŕďđčěĺđ ęîíńňŕíňű č ďĺđĺěĺííîé.\n"
                     "Ń äđóăčěč ĺé ďîěîăŕĺň áîňŕí ń 3 ęëŕńńŕ, ęîňîđűé çíŕĺň áîëüřĺ ôîđěóë ÷ĺě ĺăî ó÷čňĺëüíčöŕ, ęîňîđŕ˙ ăîňîâčň ĺăî ę ńäŕ÷ĺ ÍĂÝ (íŕ÷ŕëüíűé ăîńóäŕđńňâĺííűé ýęçŕěĺí) â 4 ęëŕńńĺ.\n"
                     "Ďîăîâŕđčâŕţň, ÷ňî ďîěčěî ďđîčçâîäíűő  ń ÍĂÝ ńóůĺńňâóţň č äđóăčĺ, íî â řęîëĺ đŕçăîâŕđčâŕňü î íčő çŕďđĺůĺíî, ŕ ňî âäđóă ĺů¸ ńňŕíóň ńëčřęîě óěíűěč č íŕ÷íóň çŕěĺ÷ŕňü, ÷ňî öĺííčę íŕ ÷¸đíóţ ď˙ňíčöó âűđŕńňŕĺň, äŕćĺ ń ó÷¸ňîě 99,9 ďđîöĺíňíîé ńęčäęîé.\n"
                     "Íî ÷ňî-ňî ěĺí˙ ďîíĺńëî, íŕě ćĺ ďđîńňî íóćíî íŕéňč áĺçîáčäíóţ, ěŕëĺíüęóţ ďđîčçâîäíóţ - ňŕę íŕ÷í¸ě ćĺ.\n\n");


    Tree diftree = {};
    TreeInit(&diftree);
    diftree.head = Diff(tree->head, TreeTex);
    Simplifier(&diftree);

    fprintf(TreeTex, "\nÂ čňîăĺ ďîëó÷ŕĺňń˙ $$");
    Tex(diftree.head, TreeTex);
    fprintf(TreeTex, "$$\nÂîň ěű č íŕřëč ďđîčçâîäíóţ, ňĺďĺđü Äŕřŕ ńěîćĺň ďîńňóďčňü â řęîëó ńâîĺé ěĺ÷ňű, ăäĺ áóäĺň ďîëó÷ŕňü îäíč ď˙ň¸đęč, ŕ ďîňîě ďîńňóďčň â ôčçňĺő-číńňčňóň. Óđŕ!!\n"
                     "\\end {otherlanguage*}\n"
                     "\\end {document}");

    TreeKill(&diftree);

    return 1;
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

    p_elem->type  = c_elem->type;
    p_elem->data  = c_elem->data;
    p_elem->left  = c_elem->left;
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

    CHECKNULLPTR(elem)

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

    if (elem->type == OPERATOR || elem->type == FUNCTION) {

        int var_l   = Search(elem->left, VARIABLE);
        int var_r   = Search(elem->right, VARIABLE);
        int func_l  = Search(elem->left, FUNCTION);
        int func_r  = Search(elem->right, FUNCTION);

        //printf("%p", elem);
        //printf("'%d', '%d'\n", func_l, func_r);
        if (!var_l &&  !var_r && !func_l && !func_r && elem->left && elem->right)
            KillSons(elem);

        else if (!var_l && elem->left) {

            if (elem->left->left && elem->left->right && !func_l)
                KillSons(elem->left);
            ObviElem(elem, 'l');
        }

        else if (!var_r && elem->right) {

            if (elem->right->left && elem->right->right && !func_r)
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
                                               return code; //ôóíęöčč čäóň ń ęîäîě îň 20
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
        return CreateElem(FUNCTION, code, NULL, node);
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

