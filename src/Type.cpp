#include "Type.hpp"

#include "Error.hpp"

namespace dao
{
    Type::Type(TypeId typeId, llvm::Type *type) : typeId(typeId), type(type) {}

    Type Type::Create(llvm::IRBuilder<> &builder, TypeId typeId)
    {
        switch (typeId)
        {
        case TypeId::Byte:
            return Type(typeId, builder.getInt8Ty());

        case TypeId::Int16:
            return Type(typeId, builder.getInt16Ty());

        case TypeId::Int32:
            return Type(typeId, builder.getInt32Ty());

        case TypeId::Int64:
            return Type(typeId, builder.getInt64Ty());

        case TypeId::UInt16:
            return Type(typeId, builder.getInt16Ty());

        case TypeId::UInt32:
            return Type(typeId, builder.getInt32Ty());

        case TypeId::UInt64:
            return Type(typeId, builder.getInt64Ty());

        case TypeId::Half:
            return Type(typeId, builder.getHalfTy());

        case TypeId::Float:
            return Type(typeId, builder.getFloatTy());

        case TypeId::Double:
            return Type(typeId, builder.getDoubleTy());

        case TypeId::Bool:
            return Type(typeId, builder.getInt1Ty());

        case TypeId::Null:
            return Type(typeId, builder.getInt32Ty());

        case TypeId::Void:
            return Type(typeId, builder.getVoidTy());

        case TypeId::String:
            return Type(typeId, builder.getInt8PtrTy());

        case TypeId::Char:
            return Type(typeId, builder.getInt8Ty());

        default:
            throw SyntaxError("不支持的类型");
        }
    }

    bool Type::isIntegerTy()
    {
        return type->isIntegerTy();
    }

    bool Type::isIntegerTy(bool isUnsigned)
    {
        if (isUnsigned)
            return typeId == TypeId::UInt16 || typeId == TypeId::UInt32 || typeId == TypeId::UInt64;
        else
            return isIntegerTy();
    }

    bool Type::isFloatingPointTy()
    {
        return type->isFloatingPointTy();
    }

    PointerType::PointerType(Type elementType, llvm::PointerType *type) : Type(TypeId::Pointer, type), elementType(elementType), type(type)
    {
    }

    PointerType PointerType::Create(llvm::IRBuilder<> &builder, Type elementType, unsigned addrSpace)
    {
        return PointerType(elementType, llvm::PointerType::get(elementType.type, addrSpace));
    }
}