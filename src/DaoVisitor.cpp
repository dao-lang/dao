#include "DaoVisitor.h"

#include <stdio.h>

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
    return DaoParserBaseVisitor::visitAdditiveExpression(context);
}

antlrcpp::Any DaoVisitor::visitMultiplicativeExpression(DaoParser::MultiplicativeExpressionContext *context)
{
    return DaoParserBaseVisitor::visitMultiplicativeExpression(context);
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
    return DaoParserBaseVisitor::visitPostfixExpression(context);
}

antlrcpp::Any DaoVisitor::visitPrimaryExpression(DaoParser::PrimaryExpressionContext *context)
{
    if (context->StringLiteral())
    {
        auto text = context->StringLiteral()->getText();
        std::cout << text << std::endl;
    }

    return DaoParserBaseVisitor::visitPrimaryExpression(context);
}

antlrcpp::Any DaoVisitor::visitConstantExpression(DaoParser::ConstantExpressionContext *context)
{
    return DaoParserBaseVisitor::visitConstantExpression(context);
}

antlrcpp::Any DaoVisitor::visitArgumentExpressionList(DaoParser::ArgumentExpressionListContext *context)
{
    return DaoParserBaseVisitor::visitArgumentExpressionList(context);
}

antlrcpp::Any DaoVisitor::visitIdentifier(DaoParser::IdentifierContext *context)
{
    auto text = context->Identifier()->getText();
    std::cout << text << std::endl;

    return DaoParserBaseVisitor::visitIdentifier(context);
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