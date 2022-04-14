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
        Void,
        Pointer
    };

    /**
     * 类型
     */
    class Type
    {
    public:
        TypeId typeId;

        Type(TypeId typeId);

        virtual llvm::Type *llvm(llvm::IRBuilder<> &builder);
    };

    /**
     * 指针类型
     */
    class PointerType : public Type
    {
    public:
        Type elementType;

        unsigned addrSpace;

        PointerType(Type elementType);

        PointerType(Type elementType, unsigned addrSpace);

        virtual llvm::Type *llvm(llvm::IRBuilder<> &builder) override;
    };
}