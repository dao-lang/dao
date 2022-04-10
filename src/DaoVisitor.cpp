#include "DaoVisitor.h"

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
#include "Utils.h"

using namespace llvm;

DaoVisitor::DaoVisitor() : context(), builder(context), module("test", context)
{
    auto *puts_type = FunctionType::get(builder.getInt32Ty(), {builder.getInt8PtrTy()}, true);
    FunctionCallee puts_func = module.getOrInsertFunction("printf", puts_type);
    func_list["打印"] = puts_func;

    auto main_type = FunctionType::get(builder.getInt32Ty(), false);
    main_func = Function::Create(main_type, Function::ExternalLinkage, "main", &module);

    block = BasicBlock::Create(context, "entry", main_func);
    builder.SetInsertPoint(block);
}

antlrcpp::Any DaoVisitor::visitFile_input(DaoParser::File_inputContext *context)
{
    for (auto statement : context->statement())
    {
        visit(statement);
    }
    builder.CreateRet(builder.getInt32(0));

    outs() << module;

    std::cout << "=======================================================================" << std::endl;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    std::unique_ptr<Module> owner(&module);
    ExecutionEngine *engine = EngineBuilder(std::move(owner)).create();

    return engine->runFunction(main_func, {});
}

antlrcpp::Any DaoVisitor::visitStatement(DaoParser::StatementContext *context)
{
    return DaoParserBaseVisitor::visitStatement(context);
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

antlrcpp::Any DaoVisitor::visitEqualityExprression(DaoParser::EqualityExprressionContext *context)
{
    return DaoParserBaseVisitor::visitEqualityExprression(context);
}

antlrcpp::Any DaoVisitor::visitRelationalExprression(DaoParser::RelationalExprressionContext *context)
{
    return DaoParserBaseVisitor::visitRelationalExprression(context);
}

antlrcpp::Any DaoVisitor::visitShiftExpression(DaoParser::ShiftExpressionContext *context)
{
    return DaoParserBaseVisitor::visitShiftExpression(context);
}

antlrcpp::Any DaoVisitor::visitAdditiveExpression(DaoParser::AdditiveExpressionContext *context)
{
    if (!context->additiveExpression())
        return visit(context->multiplicativeExpression());

    Value *left = visit(context->additiveExpression()).as<Value *>();
    Value *right = visit(context->multiplicativeExpression()).as<Value *>();
    auto op = context->op->getType();

    if (op == DaoLexer::Plus)
        return add(left, right);
    else if (op == DaoLexer::Minus)
        return sub(left, right);
    else
        throw "语法错误！";
}

antlrcpp::Any DaoVisitor::visitMultiplicativeExpression(DaoParser::MultiplicativeExpressionContext *context)
{
    if (!context->multiplicativeExpression())
        return visit(context->castExpression());

    Value *left = visit(context->multiplicativeExpression()).as<Value *>();
    Value *right = visit(context->castExpression()).as<Value *>();
    auto op = context->op->getType();

    if (op == DaoLexer::Star)
        return mul(left, right);
    else if (op == DaoLexer::Div)
        return div(left, right);
    else if (op == DaoLexer::Mod)
        return mod(left, right);
    else
        throw "语法错误！";
}

antlrcpp::Any DaoVisitor::visitCastExpression(DaoParser::CastExpressionContext *context)
{
    return DaoParserBaseVisitor::visitCastExpression(context);
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
        auto arguments = visit(context->argumentExpressionList());
        return (Value *)builder.CreateCall(previous.as<FunctionCallee>(), arguments.as<std::vector<Value *>>());
    }
    else
        throw "语法错误！";
}

antlrcpp::Any DaoVisitor::visitPrimaryExpression(DaoParser::PrimaryExpressionContext *context)
{
    if (context->identifier())
    {
        auto text = visit(context->identifier());
        FunctionCallee func = func_list[text.as<std::string>()];
        return func;
    }
    else if (context->StringLiteral())
    {
        std::string text = context->StringLiteral()->getText();
        text = convert_string(text);
        return (Value *)builder.CreateGlobalStringPtr(text);
    }
    else if (context->IntegerLiteral())
    {
        std::string text = context->IntegerLiteral()->getText();
        int value = atoi(text.c_str());
        return (Value *)builder.getInt32(value);
    }
    else if (context->FloatLiteral())
    {
        std::string text = context->FloatLiteral()->getText();
        float value = atof(text.c_str());
        return (Value *)ConstantFP::get(builder.getFloatTy(), value);
    }
    else if (context->True())
    {
        return (Value *)builder.getTrue();
    }
    else if (context->False())
    {
        return (Value *)builder.getFalse();
    }
    else if (context->Null())
    {
        return (Value *)builder.getInt32(0);
    }
    else
        throw "语法错误！";
}

antlrcpp::Any DaoVisitor::visitConstantExpression(DaoParser::ConstantExpressionContext *context)
{
    return DaoParserBaseVisitor::visitConstantExpression(context);
}

antlrcpp::Any DaoVisitor::visitArgumentExpressionList(DaoParser::ArgumentExpressionListContext *context)
{
    std::vector<Value *> args;
    for (auto expr : context->assignmentExpression())
    {
        auto arg = visit(expr);
        if (arg.is<Constant *>())
            args.push_back(arg.as<Constant *>());
        else if (arg.is<ConstantInt *>())
            args.push_back(arg.as<ConstantInt *>());
        else
            args.push_back(arg.as<Value *>());
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
    return DaoParserBaseVisitor::visitTypeName(context);
}

antlrcpp::Any DaoVisitor::visitFuncName(DaoParser::FuncNameContext *context)
{
    return DaoParserBaseVisitor::visitFuncName(context);
}

llvm::Value *DaoVisitor::add(llvm::Value *left, llvm::Value *right)
{
    auto left_type = left->getType();
    auto right_type = right->getType();

    if (left_type->isIntegerTy() && right_type->isIntegerTy())
    {
        return builder.CreateAdd(left, right);
    }
    else if (left_type->isIntegerTy() && right_type->isFloatTy())
    {
        left = builder.CreateSIToFP(left, right_type);
        return builder.CreateFAdd(left, right);
    }
    else if (left_type->isFloatTy() && right_type->isIntegerTy())
    {
        right = builder.CreateSIToFP(right, left_type);
        return builder.CreateFAdd(left, right);
    }
    else if (left_type->isFloatTy() && right_type->isFloatTy())
    {
        return builder.CreateFAdd(left, right);
    }
    else
        throw "语法错误！";
}

llvm::Value *DaoVisitor::sub(llvm::Value *left, llvm::Value *right)
{
    auto left_type = left->getType();
    auto right_type = right->getType();

    if (left_type->isIntegerTy() && right_type->isIntegerTy())
    {
        return builder.CreateSub(left, right);
    }
    else if (left_type->isIntegerTy() && right_type->isFloatTy())
    {
        left = builder.CreateSIToFP(left, right_type);
        return builder.CreateFSub(left, right);
    }
    else if (left_type->isFloatTy() && right_type->isIntegerTy())
    {
        right = builder.CreateSIToFP(right, left_type);
        return builder.CreateFSub(left, right);
    }
    else if (left_type->isFloatTy() && right_type->isFloatTy())
    {
        return builder.CreateFSub(left, right);
    }
    else
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
}