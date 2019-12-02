#define DEF_OPER(operator, name, code) OPER_##name = code,

enum command_t {

    #include "Operators.h"

    };

#undef  DEF_OPER
