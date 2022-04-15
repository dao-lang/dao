#include "AST.h"

namespace dao
{
    CallExpression::CallExpression(std::shared_ptr<Expression> function, std::vector<std::shared_ptr<Expression>> arguments) : function(function), arguments(arguments)
    {
        auto ref = dynamic_cast<FunctionReference *>(function.get());
        if (ref != nullptr)
        {
            this->type = ref->declaration->outputType;
        }
        else if (function->type->typeId == TypeId::Func)
        {
            auto func_type = (FunctionType *)function->type;
            this->type = func_type->outputType;
        }
        else
        {
            this->type = Type::get<Dynamic>();
        }
    }
}