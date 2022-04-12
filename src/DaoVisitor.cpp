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
    Type *type = nullptr;
    if (context->typeName())
        type = visit(context->typeName()).as<Type *>();
    else
        type = visit(context->varDeclarationSpecifier()).as<Type *>();

    std::string name = visit(context->identifier()).as<std::string>();
    AllocaInst *var = builder.CreateAlloca(type, nullptr, name);

    if (context->assignmentExpression())
    {
        Value *value = visit(context->assignmentExpression()).as<Value *>();
        if (value->getType()->isFloatTy() & type->isIntegerTy())
            value = builder.CreateFPToSI(value, type);
        builder.CreateStore(value, var);
    }

    var_list[name] = var;
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

    Value *left = visit(args[0]).as<Value *>();

    Value *result = nullptr;
    for (size_t i = 1; i < args.size(); i++)
    {
        auto op = visit(ops[i - 1]).as<int>();
        Value *right = visit(args[i]).as<Value *>();

        std::cout << "op:" << op << std::endl;

        Value *tmp = nullptr;
        if (op == DaoLexer::Equal)
            tmp = equal(left, right);
        else if (op == DaoLexer::NotEqual)
            tmp = notEqual(left, right);
        else if (op == DaoLexer::Less)
            tmp = less(left, right);
        else if (op == DaoLexer::LessEqual)
            tmp = lessEqual(left, right);
        else if (op == DaoLexer::Greater)
            tmp = greater(left, right);
        else if (op == DaoLexer::GreaterEqual)
            tmp = greaterEqual(left, right);
        else
            throw "语法错误！";

        result = result == nullptr ? tmp : builder.CreateAnd(result, tmp);
        left = right;
    }

    return result == nullptr ? left : result;
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
        throw "语法错误！";
}

antlrcpp::Any DaoVisitor::visitShiftExpression(DaoParser::ShiftExpressionContext *context)
{
    if (!context->shiftExpression())
        return visit(context->additiveExpression());

    Value *left = visit(context->shiftExpression()).as<Value *>();
    Value *right = visit(context->additiveExpression()).as<Value *>();
    auto op = context->op->getType();

    if (op == DaoLexer::LeftShift)
        return leftShift(left, right);
    else if (op == DaoLexer::RightShift)
        return rightShift(left, right);
    else
        throw "语法错误！";
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
        std::string text = visit(context->identifier()).as<std::string>();
        if (var_list.find(text) != var_list.end())
        {
            auto var = var_list[text];
            return (Value *)builder.CreateLoad(var->getType()->getElementType(), var);
        }
        else if (func_list.find(text) != func_list.end())
        {
            FunctionCallee func = func_list[text];
            return func;
        }
        else
            throw "未知的标识符";
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
    if (context->Byte())
        return (Type *)builder.getInt8Ty();
    else if (context->Int16())
        return (Type *)builder.getInt16Ty();
    else if (context->Int32())
        return (Type *)builder.getInt32Ty();
    else if (context->Int64())
        return (Type *)builder.getInt64Ty();
    else if (context->UInt16())
        return (Type *)builder.getInt16Ty();
    else if (context->UInt32())
        return (Type *)builder.getInt32Ty();
    else if (context->UInt64())
        return (Type *)builder.getInt64Ty();
    else if (context->Half())
        return (Type *)builder.getHalfTy();
    else if (context->Float())
        return (Type *)builder.getFloatTy();
    else if (context->Double())
        return (Type *)builder.getDoubleTy();
    else
        throw "不支持的类型";
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

llvm::Value *DaoVisitor::leftShift(llvm::Value *left, llvm::Value *right)
{
    auto left_type = left->getType();
    auto right_type = right->getType();

    if (left_type->isIntegerTy() && right_type->isIntegerTy())
    {
        return builder.CreateShl(left, right);
    }
    else
        throw "语法错误！";
}

llvm::Value *DaoVisitor::rightShift(llvm::Value *left, llvm::Value *right)
{
    auto left_type = left->getType();
    auto right_type = right->getType();

    if (left_type->isIntegerTy() && right_type->isIntegerTy())
    {
        return builder.CreateLShr(left, right);
    }
    else
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
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
        throw "语法错误！";
}