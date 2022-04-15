#include "Type.h"

namespace dao
{

    Type *inferType(int op, Type *left, Type *right)
    {
        if (left->typeId == Dynamic || right->typeId == Dynamic)
            return Type::get<Dynamic>();

        return left; // TODO 类型推断，只要包含一个动态类型，则结果就是动态类型
    }

    Type *inferType(int op, Type *value)
    {
        return value;
    }
}