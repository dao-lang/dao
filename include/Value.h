#pragma once

#include <llvm/IR/IRBuilder.h>

#include "Type.h"

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

        Value(Type type, llvm::Value *value);
    };
}