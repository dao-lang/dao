#include "Type.h"

#include "Error.h"

namespace dao
{
    Type::Type(TypeId typeId) : typeId(typeId) {}

    llvm::Type *Type::llvm(llvm::IRBuilder<> &builder)
    {
        switch (typeId)
        {
        case TypeId::Byte:
            return builder.getInt8Ty();

        case TypeId::Int16:
            return builder.getInt16Ty();

        case TypeId::Int32:
            return builder.getInt32Ty();

        case TypeId::Int64:
            return builder.getInt64Ty();

        case TypeId::UInt16:
            return builder.getInt16Ty();

        case TypeId::UInt32:
            return builder.getInt32Ty();

        case TypeId::UInt64:
            return builder.getInt64Ty();

        case TypeId::Half:
            return builder.getHalfTy();

        case TypeId::Float:
            return builder.getFloatTy();

        case TypeId::Double:
            return builder.getDoubleTy();

        case TypeId::Bool:
            return builder.getInt1Ty();

        case TypeId::Void:
            return builder.getVoidTy();

        case TypeId::String:
            return builder.getInt8PtrTy();

        case TypeId::Char:
            return builder.getInt8Ty();

        default:
            throw SyntaxError("不支持的类型");
        }
    }

    PointerType::PointerType(Type elementType) : PointerType(elementType, 0) {}

    PointerType::PointerType(Type elementType, unsigned addrSpace) : elementType(elementType), addrSpace(addrSpace), Type(TypeId::Pointer) {}

    llvm::Type *PointerType::llvm(llvm::IRBuilder<> &builder)
    {
        return llvm::PointerType::get(elementType.llvm(builder), addrSpace);
    }
}