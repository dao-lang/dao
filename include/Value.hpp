#pragma once

#include <llvm/IR/IRBuilder.h>

#include "Type.hpp"

namespace dao
{
    /**
     * 带有类型的值
     */
    class Value
    {
    public:
        Type type;

        llvm::Value *value;

        Value() = default;

        Value(Type type, llvm::Value *value);
    };

    /**
     * 函数
     */
    class FunctionValue : public Value
    {
    };
}