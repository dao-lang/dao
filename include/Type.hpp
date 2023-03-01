#pragma once

#include <llvm/IR/IRBuilder.h>

namespace dao
{
    /***
     * 类型Id
     */
    enum TypeId
    {
        Byte,
        Int16,
        Int32,
        Int64,
        UInt16,
        UInt32,
        UInt64,
        Half,
        Float,
        Double,
        String,
        Char,
        Bool,
        Null,
        Void,
        Pointer,
        Function
    };

    /**
     * 类型
     */
    class Type
    {
    public:
        TypeId typeId;

        llvm::Type *type;

        Type() = default;

        Type(TypeId typeId, llvm::Type *type);

        static Type Create(llvm::IRBuilder<> &builder, TypeId typeId);

        bool isIntegerTy();

        bool isIntegerTy(bool isUnsigned);

        bool isFloatingPointTy();
    };

    /**
     * 指针类型
     */
    class PointerType : public Type
    {
    public:
        Type elementType;

        llvm::PointerType *type;

        PointerType() = default;

        PointerType(Type elementType, llvm::PointerType *type);

        static PointerType Create(llvm::IRBuilder<> &builder, Type elementType, unsigned addrSpace = 0);
    };
}