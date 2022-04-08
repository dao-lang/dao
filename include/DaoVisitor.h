#pragma once

#include "DaoParserBaseVisitor.h"

class DaoVisitor : public DaoParserBaseVisitor
{
    virtual antlrcpp::Any visitExpression(DaoParser::ExpressionContext *context) override;

    virtual antlrcpp::Any visitAssignmentExpression(DaoParser::AssignmentExpressionContext *context) override;

    virtual antlrcpp::Any visitAssignmentOperator(DaoParser::AssignmentOperatorContext *context) override;

    virtual antlrcpp::Any visitConditionalExpression(DaoParser::ConditionalExpressionContext *context) override;

    virtual antlrcpp::Any visitLogicalOrExpression(DaoParser::LogicalOrExpressionContext *context) override;

    virtual antlrcpp::Any visitLogicalAndExprression(DaoParser::LogicalAndExprressionContext *context) override;

    virtual antlrcpp::Any visitInclusiveOrExprression(DaoParser::InclusiveOrExprressionContext *context) override;

    virtual antlrcpp::Any visitExclusiveOrExprression(DaoParser::ExclusiveOrExprressionContext *context) override;

    virtual antlrcpp::Any visitAndExprression(DaoParser::AndExprressionContext *context) override;

    virtual antlrcpp::Any visitEqualityExprression(DaoParser::EqualityExprressionContext *context) override;

    virtual antlrcpp::Any visitRelationalExprression(DaoParser::RelationalExprressionContext *context) override;

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
};