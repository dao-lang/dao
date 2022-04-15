#pragma once

#include <string>
#include <vector>

namespace dao
{
    /**
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
        Func,
        Dynamic
    };

    /**
     * 类型
     */
    class Type
    {
    public:
        TypeId typeId;

        explicit Type(TypeId typeId) : typeId(typeId) {}

        template <TypeId TYPE_ID>
        static Type *get()
        {
            static Type type(TYPE_ID);
            return &type;
        }
    };

    /**
     * 指针类型
     */
    class PointerType : public Type
    {
    public:
        Type *elementType;

        PointerType(Type *elementType) : Type(TypeId::Pointer), elementType(elementType) {}

        template <TypeId TYPE_ID>
        static PointerType *get()
        {
            static PointerType type(Type::get<TYPE_ID>());
            return &type;
        }
    };

    /**
     * 函数类型
     */
    class FunctionType : public Type
    {
    public:
        std::vector<Type *> parameterTypes;

        Type *outputType;

        bool isVariable;

        explicit FunctionType(const std::initializer_list<Type *> &parameterTypes, Type *outputType, bool isVariable) 
        : Type(TypeId::Func), parameterTypes(parameterTypes), outputType(outputType), isVariable(isVariable)
        {
        }
    };

    /**
     * 类型推断
     */
    Type *inferType(int op, Type *left, Type *right);

    /**
     * 类型推断
     */
    Type *inferType(int op, Type *value);
}