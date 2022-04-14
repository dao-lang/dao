#include "Value.h"

namespace dao
{
    Value::Value(Type type, llvm::Value *value) : type(type), value(value){};
}