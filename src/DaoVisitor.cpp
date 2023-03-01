#include "DaoVisitor.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "DaoLexer.h"
#include "Type.hpp"
#include "Value.hpp"
#include "Utils.hpp"
#include "Error.hpp"

namespace dao
{
    DaoVisitor::DaoVisitor() : context(), builder(context), module("test", context)
    {
        auto *puts_type = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8PtrTy()}, true);
        llvm::FunctionCallee puts_func = module.getOrInsertFunction("printf", puts_type);
        func_list["打印"] = puts_func;

        auto main_type = llvm::FunctionType::get(builder.getInt32Ty(), false);
        main_func = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", &module);

        block = llvm::BasicBlock::Create(context, "entry", main_func);
        builder.SetInsertPoint(block);
    }

    antlrcpp::Any DaoVisitor::visitFile_input(DaoParser::File_inputContext *context)
    {
        for (auto statement : context->statement())
        {
            visit(statement);
        }
        builder.CreateRet(builder.getInt32(0));

        llvm::outs() << module;

        std::cout << "=======================================================================" << std::endl;

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        std::unique_ptr<llvm::Module> owner(&module);
        llvm::ExecutionEngine *engine = llvm::EngineBuilder(std::move(owner)).create();

        return engine->runFunction(main_func, {});
    }

    antlrcpp::Any DaoVisitor::visitStatement(DaoParser::StatementContext *context)
    {
        if (context->expression())
            return visit(context->expression());
        else if (context->varDeclaration())
            return visit(context->varDeclaration());

        return 0;
    }

    antlrcpp::Any DaoVisitor::visitVarDeclaration(DaoParser::VarDeclarationContext *context)
    {
        return visit(context->varDeclarationSpecifier());
    }

    antlrcpp::Any DaoVisitor::visitVarDeclarationSpecifier(DaoParser::VarDeclarationSpecifierContext *context)
    {
        Type type;
        if (context->typeName())
            type = visit(context->typeName()).as<Type>();
        else
            type = visit(context->varDeclarationSpecifier()).as<Type>();

        std::string name = visit(context->identifier()).as<std::string>();
        llvm::AllocaInst *var = builder.CreateAlloca(type.type, nullptr, name);

        if (context->expression())
        {
            Value value = visit(context->expression()).as<Value>();
            if (value.type.isFloatingPointTy() && type.isIntegerTy())
            {
                value = Value(type, builder.CreateFPToSI(value.value, type.type)); // TODO 类型转换
            }
            builder.CreateStore(value.value, var);
        }

        var_list[name] = Value(type, var);
        return type;
    }

    antlrcpp::Any DaoVisitor::visitExpression(DaoParser::ExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitAssignmentExpression(DaoParser::AssignmentExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitAssignmentExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitAssignmentOperator(DaoParser::AssignmentOperatorContext *context)
    {
        return DaoParserBaseVisitor::visitAssignmentOperator(context);
    }

    antlrcpp::Any DaoVisitor::visitConditionalExpression(DaoParser::ConditionalExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitConditionalExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitLogicalOrExpression(DaoParser::LogicalOrExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitLogicalOrExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitLogicalAndExprression(DaoParser::LogicalAndExprressionContext *context)
    {
        return DaoParserBaseVisitor::visitLogicalAndExprression(context);
    }

    antlrcpp::Any DaoVisitor::visitInclusiveOrExprression(DaoParser::InclusiveOrExprressionContext *context)
    {
        return DaoParserBaseVisitor::visitInclusiveOrExprression(context);
    }

    antlrcpp::Any DaoVisitor::visitExclusiveOrExprression(DaoParser::ExclusiveOrExprressionContext *context)
    {
        return DaoParserBaseVisitor::visitExclusiveOrExprression(context);
    }

    antlrcpp::Any DaoVisitor::visitAndExprression(DaoParser::AndExprressionContext *context)
    {
        return DaoParserBaseVisitor::visitAndExprression(context);
    }

    antlrcpp::Any DaoVisitor::visitRelationalExprression(DaoParser::RelationalExprressionContext *context)
    {
        auto ops = context->relationalOperator();
        auto args = context->shiftExpression();

        Value left = visit(args[0]).as<Value>();

        // TODO
        // Value result;
        // for (size_t i = 1; i < args.size(); i++)
        // {
        //     auto op = visit(ops[i - 1]).as<int>();
        //     Value right = visit(args[i]).as<Value>();

        //     std::cout << "op:" << op << std::endl;

        //     Value tmp;
        //     if (op == DaoLexer::Equal)
        //         tmp = equal(left, right);
        //     else if (op == DaoLexer::NotEqual)
        //         tmp = notEqual(left, right);
        //     else if (op == DaoLexer::Less)
        //         tmp = less(left, right);
        //     else if (op == DaoLexer::LessEqual)
        //         tmp = lessEqual(left, right);
        //     else if (op == DaoLexer::Greater)
        //         tmp = greater(left, right);
        //     else if (op == DaoLexer::GreaterEqual)
        //         tmp = greaterEqual(left, right);
        //     else
        //         throw SyntaxError("语法错误！");

        //     result = result == nullptr ? tmp : builder.CreateAnd(result, tmp);
        //     left = right;
        // }

        // return result == nullptr ? left : result;

        return left;
    }

    antlrcpp::Any DaoVisitor::visitRelationalOperator(DaoParser::RelationalOperatorContext *context)
    {
        if (context->Equal())
            return (int)DaoLexer::Equal;
        else if (context->NotEqual())
            return (int)DaoLexer::NotEqual;
        else if (context->Less())
            return (int)DaoLexer::Less;
        else if (context->LessEqual())
            return (int)DaoLexer::LessEqual;
        else if (context->Greater())
            return (int)DaoLexer::Greater;
        else if (context->GreaterEqual())
            return (int)DaoLexer::GreaterEqual;
        else
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitShiftExpression(DaoParser::ShiftExpressionContext *context)
    {
        if (!context->shiftExpression())
            return visit(context->additiveExpression());

        Value left = visit(context->shiftExpression()).as<Value>();
        Value right = visit(context->additiveExpression()).as<Value>();
        auto op = context->op->getType();

        if (op == DaoLexer::LeftShift)
            return leftShift(left, right);
        else if (op == DaoLexer::RightShift)
            return rightShift(left, right);
        else
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitAdditiveExpression(DaoParser::AdditiveExpressionContext *context)
    {
        if (!context->additiveExpression())
            return visit(context->multiplicativeExpression());

        Value left = visit(context->additiveExpression()).as<Value>();
        Value right = visit(context->multiplicativeExpression()).as<Value>();
        auto op = context->op->getType();

        if (op == DaoLexer::Plus)
            return add(left, right);
        else if (op == DaoLexer::Minus)
            return sub(left, right);
        else
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitMultiplicativeExpression(DaoParser::MultiplicativeExpressionContext *context)
    {
        if (!context->multiplicativeExpression())
            return visit(context->castExpression());

        llvm::Value *left = visit(context->multiplicativeExpression()).as<llvm::Value *>();
        llvm::Value *right = visit(context->castExpression()).as<llvm::Value *>();
        auto op = context->op->getType();

        if (op == DaoLexer::Star)
            return mul(left, right);
        else if (op == DaoLexer::Div)
            return div(left, right);
        else if (op == DaoLexer::Mod)
            return mod(left, right);
        else
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitCastExpression(DaoParser::CastExpressionContext *context)
    {
        if (context->unaryExpression())
            return visit(context->unaryExpression());

        llvm::Value *value = visit(context->castExpression()).as<llvm::Value *>();
        llvm::Type *type = visit(context->typeName()).as<llvm::Type *>();
        return cast(value, type);
    }

    antlrcpp::Any DaoVisitor::visitUnaryExpression(DaoParser::UnaryExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitUnaryExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitUnaryOperator(DaoParser::UnaryOperatorContext *context)
    {
        return DaoParserBaseVisitor::visitUnaryOperator(context);
    }

    antlrcpp::Any DaoVisitor::visitPostfixExpression(DaoParser::PostfixExpressionContext *context)
    {
        if (context->primaryExpression())
        {
            return visit(context->primaryExpression());
        }

        auto previous = visit(context->postfixExpression());
        if (context->argumentExpressionList())
        {
            auto func = previous.as<llvm::FunctionCallee>();

            auto arguments = visit(context->argumentExpressionList()).as<std::vector<Value>>();
            std::vector<llvm::Value *> args;
            for (auto arg : arguments)
                args.push_back(arg.value);

            // func.getFunctionType()->getReturnType()
            return Value(Type::Create(builder, TypeId::Int32), builder.CreateCall(func, args));
        }
        else
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitPrimaryExpression(DaoParser::PrimaryExpressionContext *context)
    {
        if (context->identifier())
        {
            std::string text = visit(context->identifier()).as<std::string>();
            if (var_list.find(text) != var_list.end())
            {
                auto var = var_list[text];
                return Value(var.type, builder.CreateLoad(var.type.type, var.value));
            }
            else if (func_list.find(text) != func_list.end())
            {
                llvm::FunctionCallee func = func_list[text];
                // return Value(Type::Create(builder, TypeId::Function), func);
                return func;
            }
            else
                throw SyntaxError("未知的标识符");
        }
        else if (context->StringLiteral())
        {
            std::string text = context->StringLiteral()->getText();
            text = convert_string(text);
            return Value(Type::Create(builder, TypeId::String), builder.CreateGlobalStringPtr(text));
        }
        else if (context->IntegerLiteral())
        {
            std::string text = context->IntegerLiteral()->getText();
            int value = atoi(text.c_str());
            return Value(Type::Create(builder, TypeId::Int32), builder.getInt32(value));
        }
        else if (context->FloatLiteral())
        {
            std::string text = context->FloatLiteral()->getText();
            float value = atof(text.c_str());
            return Value(Type::Create(builder, TypeId::Float), llvm::ConstantFP::get(builder.getFloatTy(), value));
        }
        else if (context->True())
        {
            return Value(Type::Create(builder, TypeId::Bool), builder.getTrue());
        }
        else if (context->False())
        {
            return Value(Type::Create(builder, TypeId::Bool), builder.getFalse());
        }
        else if (context->Null())
        {
            return Value(Type::Create(builder, TypeId::Null), builder.getInt32(0));
        }
        else
            throw SyntaxError("不支持的Primary表达式");
    }

    antlrcpp::Any DaoVisitor::visitConstantExpression(DaoParser::ConstantExpressionContext *context)
    {
        return DaoParserBaseVisitor::visitConstantExpression(context);
    }

    antlrcpp::Any DaoVisitor::visitArgumentExpressionList(DaoParser::ArgumentExpressionListContext *context)
    {
        std::vector<Value> args;
        for (auto expr : context->assignmentExpression())
        {
            Value arg = visit(expr).as<Value>();
            args.push_back(arg);
        }
        return args;
    }

    antlrcpp::Any DaoVisitor::visitIdentifier(DaoParser::IdentifierContext *context)
    {
        return context->Identifier()->getText();
    }

    antlrcpp::Any DaoVisitor::visitDotName(DaoParser::DotNameContext *context)
    {
        return DaoParserBaseVisitor::visitDotName(context);
    }

    antlrcpp::Any DaoVisitor::visitTypeName(DaoParser::TypeNameContext *context)
    {
        builder.getInt16Ty();
        if (context->Byte())
            return Type::Create(builder, TypeId::Byte);
        else if (context->Int16())
            return Type::Create(builder, TypeId::Int16);
        else if (context->Int32())
            return Type::Create(builder, TypeId::Int32);
        else if (context->Int64())
            return Type::Create(builder, TypeId::Int64);
        else if (context->UInt16())
            return Type::Create(builder, TypeId::UInt16);
        else if (context->UInt32())
            return Type::Create(builder, TypeId::UInt32);
        else if (context->UInt64())
            return Type::Create(builder, TypeId::UInt64);
        else if (context->Half())
            return Type::Create(builder, TypeId::Half);
        else if (context->Float())
            return Type::Create(builder, TypeId::Float);
        else if (context->Double())
            return Type::Create(builder, TypeId::Double);
        else if (context->Bool())
            return Type::Create(builder, TypeId::Bool);
        else if (context->Text())
            return Type::Create(builder, TypeId::String);
        else
            throw SyntaxError("不支持的类型!");
    }

    antlrcpp::Any DaoVisitor::visitFuncName(DaoParser::FuncNameContext *context)
    {
        return DaoParserBaseVisitor::visitFuncName(context);
    }

    Value DaoVisitor::add(Value &left, Value &right)
    {
        auto left_type = left.type;
        auto right_type = right.type;

        if (left_type.isIntegerTy() && right_type.isIntegerTy())
        {
            return Value(left_type, builder.CreateAdd(left.value, right.value));
        }
        else if (left_type.isIntegerTy() && right_type.isFloatingPointTy())
        {
            auto left_value = left.value;
            if (left_type.isIntegerTy(true))
                left_value = builder.CreateUIToFP(left_value, right_type.type);
            else
                left_value = builder.CreateSIToFP(left_value, right_type.type);
            return Value(right_type, builder.CreateFAdd(left_value, right.value));
        }
        else if (left_type.isFloatingPointTy() && right_type.isIntegerTy())
        {
            auto right_value = right.value;
            if (left_type.isIntegerTy(true))
                right_value = builder.CreateUIToFP(right_value, left_type.type);
            else
                right_value = builder.CreateSIToFP(right_value, left_type.type);

            return Value(left_type, builder.CreateFAdd(left.value, right_value));
        }
        else if (left_type.isFloatingPointTy() && right_type.isFloatingPointTy())
        {
            return Value(left_type, builder.CreateFAdd(left.value, right.value));
        }
        else
            throw SyntaxError("语法错误！");
    }

    Value DaoVisitor::sub(Value &left, Value &right)
    {
        auto left_type = left.type;
        auto right_type = right.type;

        if (left_type.isIntegerTy() && right_type.isIntegerTy())
        {
            return Value(left_type, builder.CreateSub(left.value, right.value));
        }
        else if (left_type.isIntegerTy() && right_type.isFloatingPointTy())
        {
            auto left_value = left.value;
            if (left_type.isIntegerTy(true))
                left_value = builder.CreateUIToFP(left_value, right_type.type);
            else
                left_value = builder.CreateSIToFP(left_value, right_type.type);
            return Value(right_type, builder.CreateFSub(left_value, right.value));
        }
        else if (left_type.isFloatingPointTy() && right_type.isIntegerTy())
        {
            auto right_value = right.value;
            if (left_type.isIntegerTy(true))
                right_value = builder.CreateUIToFP(right_value, left_type.type);
            else
                right_value = builder.CreateSIToFP(right_value, left_type.type);

            return Value(left_type, builder.CreateFSub(left.value, right_value));
        }
        else if (left_type.isFloatingPointTy() && right_type.isFloatingPointTy())
        {
            return Value(left_type, builder.CreateFSub(left.value, right.value));
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::mul(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateMul(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFMul(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFMul(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFMul(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::div(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateSDiv(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFDiv(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFDiv(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFDiv(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::mod(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateSRem(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFRem(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFRem(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFRem(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    Value DaoVisitor::leftShift(const Value &left, const Value &right)
    {
        auto left_type = left.type.type;
        auto right_type = right.type.type;

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return Value(left.type, builder.CreateShl(left.value, right.value));
        }
        else
            throw SyntaxError("语法错误！");
    }

    Value DaoVisitor::rightShift(const Value &left, const Value &right)
    {
        auto left_type = left.type.type;
        auto right_type = right.type.type;

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return Value(left.type, builder.CreateLShr(left.value, right.value));
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::equal(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpEQ(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpOEQ(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpOEQ(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpOEQ(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::notEqual(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpNE(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpONE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpONE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpONE(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::less(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpSLT(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpOLT(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpOLT(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpOLT(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::lessEqual(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpSLE(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpOLE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpOLE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpOLE(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::greater(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpSGT(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpOGT(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpOGT(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpOGT(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::greaterEqual(llvm::Value *left, llvm::Value *right)
    {
        auto left_type = left->getType();
        auto right_type = right->getType();

        if (left_type->isIntegerTy() && right_type->isIntegerTy())
        {
            return builder.CreateICmpSGE(left, right);
        }
        else if (left_type->isIntegerTy() && right_type->isFloatTy())
        {
            left = builder.CreateSIToFP(left, right_type);
            return builder.CreateFCmpOGE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isIntegerTy())
        {
            right = builder.CreateSIToFP(right, left_type);
            return builder.CreateFCmpOGE(left, right);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy())
        {
            return builder.CreateFCmpOGE(left, right);
        }
        else
            throw SyntaxError("语法错误！");
    }

    llvm::Value *DaoVisitor::cast(llvm::Value *value, llvm::Type *type)
    {
        return value; // TODO 类型转换
    }
}