#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include "DaoParserBaseVisitor.h"

class DaoVisitor : public DaoParserBaseVisitor
{
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;

    llvm::Function *main_func;
    llvm::BasicBlock *block;

    std::map<std::string, llvm::FunctionCallee> func_list;
    std::map<std::string, llvm::AllocaInst *> var_list;

public:
    DaoVisitor();

    virtual antlrcpp::Any visitFile_input(DaoParser::File_inputContext *context) override;

    virtual antlrcpp::Any visitStatement(DaoParser::StatementContext *context) override;

    virtual antlrcpp::Any visitVarDeclaration(DaoParser::VarDeclarationContext *context) override;

    virtual antlrcpp::Any visitVarDeclarationSpecifier(DaoParser::VarDeclarationSpecifierContext *context) override;

    virtual antlrcpp::Any visitExpression(DaoParser::ExpressionContext *context) override;

    virtual antlrcpp::Any visitAssignmentExpression(DaoParser::AssignmentExpressionContext *context) override;

    virtual antlrcpp::Any visitAssignmentOperator(DaoParser::AssignmentOperatorContext *context) override;

    virtual antlrcpp::Any visitConditionalExpression(DaoParser::ConditionalExpressionContext *context) override;

    virtual antlrcpp::Any visitLogicalOrExpression(DaoParser::LogicalOrExpressionContext *context) override;

    virtual antlrcpp::Any visitLogicalAndExprression(DaoParser::LogicalAndExprressionContext *context) override;

    virtual antlrcpp::Any visitInclusiveOrExprression(DaoParser::InclusiveOrExprressionContext *context) override;

    virtual antlrcpp::Any visitExclusiveOrExprression(DaoParser::ExclusiveOrExprressionContext *context) override;

    virtual antlrcpp::Any visitAndExprression(DaoParser::AndExprressionContext *context) override;

    virtual antlrcpp::Any visitRelationalExprression(DaoParser::RelationalExprressionContext *context) override;

    virtual antlrcpp::Any visitRelationalOperator(DaoParser::RelationalOperatorContext *context) override;

    virtual antlrcpp::Any visitShiftExpression(DaoParser::ShiftExpressionContext *context) override;

    virtual antlrcpp::Any visitAdditiveExpression(DaoParser::AdditiveExpressionContext *context) override;

    virtual antlrcpp::Any visitMultiplicativeExpression(DaoParser::MultiplicativeExpressionContext *context) override;

    virtual antlrcpp::Any visitCastExpression(DaoParser::CastExpressionContext *context) override;

    virtual antlrcpp::Any visitUnaryExpression(DaoParser::UnaryExpressionContext *context) override;

    virtual antlrcpp::Any visitUnaryOperator(DaoParser::UnaryOperatorContext *context) override;

    virtual antlrcpp::Any visitPostfixExpression(DaoParser::PostfixExpressionContext *context) override;

    virtual antlrcpp::Any visitPrimaryExpression(DaoParser::PrimaryExpressionContext *context) override;

    virtual antlrcpp::Any visitConstantExpression(DaoParser::ConstantExpressionContext *context) override;

    virtual antlrcpp::Any visitArgumentExpressionList(DaoParser::ArgumentExpressionListContext *context) override;

    virtual antlrcpp::Any visitIdentifier(DaoParser::IdentifierContext *context) override;

    virtual antlrcpp::Any visitDotName(DaoParser::DotNameContext *context) override;

    virtual antlrcpp::Any visitTypeName(DaoParser::TypeNameContext *context) override;

    virtual antlrcpp::Any visitFuncName(DaoParser::FuncNameContext *context) override;

private:
    llvm::Value *add(llvm::Value *left, llvm::Value *right);

    llvm::Value *sub(llvm::Value *left, llvm::Value *right);

    llvm::Value *mul(llvm::Value *left, llvm::Value *right);

    llvm::Value *div(llvm::Value *left, llvm::Value *right);

    llvm::Value *mod(llvm::Value *left, llvm::Value *right);

    llvm::Value *leftShift(llvm::Value *left, llvm::Value *right);

    llvm::Value *rightShift(llvm::Value *left, llvm::Value *right);

    llvm::Value *equal(llvm::Value *left, llvm::Value *right);

    llvm::Value *notEqual(llvm::Value *left, llvm::Value *right);

    llvm::Value *less(llvm::Value *left, llvm::Value *right);

    llvm::Value *lessEqual(llvm::Value *left, llvm::Value *right);

    llvm::Value *greater(llvm::Value *left, llvm::Value *right);

    llvm::Value *greaterEqual(llvm::Value *left, llvm::Value *right);
};