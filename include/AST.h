#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Type.h"

namespace dao
{
    /**
     * 语法树基类
     */
    class BaseAST
    {
    public:
        virtual void print(int level) {}
    };

    /**
     * 表达式基类
     */
    class Expression : public BaseAST
    {
    public:
        Type *type;
    };

    /**
     * 一元表达式基类
     */
    class UnaryExpression : public Expression
    {
    public:
        int op;

        std::shared_ptr<Expression> value;

        UnaryExpression(int op, std::shared_ptr<Expression> value) : op(op), value(value)
        {
            this->type = inferType(op, value->type);
        }
    };

    /**
     * 二元表达式基类
     */
    class BinaryExpression : public Expression
    {
    public:
        int op;

        std::shared_ptr<Expression> left;

        std::shared_ptr<Expression> right;

        BinaryExpression(int op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) : op(op), left(left), right(right)
        {
            this->type = inferType(op, left->type, right->type);
        }

        static std::shared_ptr<Expression> get(int op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        {
            return std::make_shared<BinaryExpression>(op, left, right);
        }
    };

    /**
     * 函数调用表达式
     */
    class CallExpression : public Expression
    {
    public:
        std::shared_ptr<Expression> function; // 函数声明|表达式

        std::vector<std::shared_ptr<Expression>> arguments;

        CallExpression(std::shared_ptr<Expression> function, std::vector<std::shared_ptr<Expression>> arguments);

        static std::shared_ptr<Expression> get(std::shared_ptr<Expression> function, std::vector<std::shared_ptr<Expression>> arguments)
        {
            return std::make_shared<CallExpression>(function, arguments);
        }
    };

    /**
     * 类型转换表达式
     */
    class CastExpression : public Expression
    {
    public:
        std::shared_ptr<Expression> value;

        CastExpression(std::shared_ptr<Expression> value, Type *type) : value(value)
        {
            this->type = type;
        }

        static std::shared_ptr<Expression> get(std::shared_ptr<Expression> value, Type *type)
        {
            return std::make_shared<CastExpression>(value, type);
        }
    };

    /**
     * 常量，即字面量
     */
    template <typename VALUE_TYPE, TypeId TYPE_ID>
    class Constant : public Expression
    {
    public:
        VALUE_TYPE value;

        Constant(VALUE_TYPE value) : value(value)
        {
            this->type = Type::get<TYPE_ID>();
        }

        static std::shared_ptr<Expression> get(VALUE_TYPE value)
        {
            return std::make_shared<Constant<VALUE_TYPE, TYPE_ID>>(value);
        }
    };

    /**
     * 声明基类
     */
    class Declaration : public BaseAST
    {
    };

    /**
     * 变量声明
     */
    class VariableDeclaration : public Declaration
    {
    public:
        std::string name;

        Type *type;

        std::shared_ptr<Expression> initialValue;

        VariableDeclaration(const std::string &name, Type *type, std::shared_ptr<Expression> initialValue = nullptr)
            : name(name), type(type), initialValue(initialValue)
        {
        }
    };

    /**
     * 函数形参
     */
    class FunctionParameter : public BaseAST
    {
    public:
        std::string name;

        Type *type;

        std::shared_ptr<Expression> defaultValue;

        FunctionParameter(const std::string &name, Type *type, std::shared_ptr<Expression> defaultValue = nullptr) : name(name), type(type), defaultValue(defaultValue)
        {
        }

        FunctionParameter(Type *type, std::shared_ptr<Expression> defaultValue) : type(type), defaultValue(defaultValue)
        {
        }

        FunctionParameter(Type *type) : type(type), defaultValue(nullptr)
        {
        }
    };

    /**
     * 函数声明
     */
    class FunctionDeclaration : public Declaration
    {
    public:
        std::string name;

        std::vector<FunctionParameter> parameters;

        Type *outputType;

        bool isVariable;

        FunctionDeclaration(const std::string &name, std::vector<FunctionParameter> parameters, Type *outputType, bool isVariable)
            : name(name), parameters(parameters), outputType(outputType), isVariable(isVariable)
        {
        }

        explicit FunctionDeclaration(const std::string &name) : name(name), outputType(Type::get<Void>()), isVariable(false)
        {
        }
    };

    /**
     * 引用
     */
    class Reference : public Expression
    {
    };

    /**
     * 变量引用
     */
    class VariableReference : public Reference
    {
    public:
        std::shared_ptr<VariableDeclaration> declaration;

        explicit VariableReference(std::shared_ptr<VariableDeclaration> declaration) : declaration(declaration)
        {
            this->type = declaration->type;
        }

        static std::shared_ptr<Expression> get(std::shared_ptr<VariableDeclaration> declaration)
        {
            return std::make_shared<VariableReference>(declaration);
        }
    };

    /**
     * 函数引用
     */
    class FunctionReference : public Reference
    {
    public:
        std::shared_ptr<FunctionDeclaration> declaration;

        explicit FunctionReference(std::shared_ptr<FunctionDeclaration> declaration) : declaration(declaration) {}

        static std::shared_ptr<Expression> get(std::shared_ptr<FunctionDeclaration> declaration)
        {
            return std::make_shared<FunctionReference>(declaration);
        }
    };

    /**
     * 语句基类
     */
    class Statement : public BaseAST
    {
    };

    /**
     * 语句块
     */
    class StatementBlock : public std::vector<std::shared_ptr<Statement>>
    {
    };

    /**
     * 表达式语句
     */
    class ExpressionStatement : public Statement
    {
    public:
        std::shared_ptr<Expression> expression;

        ExpressionStatement(std::shared_ptr<Expression> expression) : expression(expression) {}
    };

    /**
     * 声明语句
     */
    class DeclarationStatement : public Statement
    {
    public:
        std::shared_ptr<Declaration> declaration;

        DeclarationStatement(std::shared_ptr<Declaration> declaration) : declaration(declaration) {}
    };

    /**
     * 函数
     */
    class Function : public BaseAST
    {
    public:
        std::shared_ptr<FunctionDeclaration> declaration;

        StatementBlock body;

        explicit Function(std::shared_ptr<FunctionDeclaration> declaration) : declaration(declaration) {}
    };

    /**
     * 函数语句
     */
    class FunctionStatement : public Statement
    {
    public:
        std::shared_ptr<Function> function;

        FunctionStatement(std::shared_ptr<Function> function) : function(function) {}
    };

    /**
     * 条件语句
     */
    class IfStatement : public Statement
    {
    public:
        std::shared_ptr<Expression> condition;

        StatementBlock thenBlock;

        StatementBlock elseBlock;
    };

    /**
     * 循环语句
     */
    class ForStatement : public Statement
    {
    public:
        std::shared_ptr<Statement> initialStatement;

        std::shared_ptr<Expression> condition;

        std::shared_ptr<Statement> nextStatement;

        StatementBlock block;
    };

    /**
     * 模块
     */
    class Module : public BaseAST
    {
    public:
        StatementBlock statements;
    };
}