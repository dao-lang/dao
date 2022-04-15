#include "DaoVisitor.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "DaoLexer.h"
#include "Type.h"
#include "Utils.h"
#include "Error.h"
#include "AST.h"

namespace dao
{
    DaoVisitor::DaoVisitor()
    {
        module = std::make_shared<Module>();

        std::vector<FunctionParameter> parameters = {Type::get<String>()};
        auto printf_decl = std::make_shared<FunctionDeclaration>("printf", parameters, Type::get<Int32>(), true);
        func_list["打印"] = printf_decl;

        auto main_decl = std::make_shared<FunctionDeclaration>(std::string("main"), std::vector<FunctionParameter>(), Type::get<Int32>(), false);
        auto main_func = std::make_shared<Function>(main_decl);

        this->function = main_func;
        this->block = &main_func->body;

        module->statements.push_back(std::make_shared<FunctionStatement>(main_func));
    }

    antlrcpp::Any DaoVisitor::visitFileInput(DaoParser::FileInputContext *context)
    {
        for (auto statement : context->statement())
        {
            visit(statement);
        }

        return module;
    }

    antlrcpp::Any DaoVisitor::visitStatement(DaoParser::StatementContext *context)
    {
        if (context->expression())
        {
            auto expr = visit(context->expression()).as<std::shared_ptr<Expression>>();
            auto stmt = std::make_shared<ExpressionStatement>(expr);
            block->push_back(stmt);
            return stmt;
        }
        else if (context->varDeclaration())
        {
            return visit(context->varDeclaration());
        }
        return 0;
    }

    antlrcpp::Any DaoVisitor::visitVarDeclaration(DaoParser::VarDeclarationContext *context)
    {
        return visit(context->varDeclarationSpecifier());
    }

    antlrcpp::Any DaoVisitor::visitVarDeclarationSpecifier(DaoParser::VarDeclarationSpecifierContext *context)
    {
        Type *type;
        if (context->typeName())
            type = visit(context->typeName()).as<Type *>();
        else
            type = visit(context->varDeclarationSpecifier()).as<Type *>();

        auto name = visit(context->identifier()).as<std::string>();

        std::shared_ptr<Expression> initialValue = nullptr;
        if (context->expression())
        {
            initialValue = visit(context->expression()).as<std::shared_ptr<Expression>>();
        }

        auto decl = std::make_shared<VariableDeclaration>(name, type, initialValue);
        auto stmt = std::make_shared<DeclarationStatement>(decl);
        block->push_back(stmt);

        var_list[name] = decl;
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
        auto left = visit(args[0]).as<std::shared_ptr<Expression>>();

        std::shared_ptr<Expression> result = nullptr;
        for (size_t i = 1; i < args.size(); i++)
        {
            auto op = visit(ops[i - 1]).as<int>();
            auto right = visit(args[i]).as<std::shared_ptr<Expression>>();

            auto value = BinaryExpression::get(op, left, right);
            result = result == nullptr ? value : BinaryExpression::get(DaoLexer::And, result, value);
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
            throw SyntaxError("语法错误！");
    }

    antlrcpp::Any DaoVisitor::visitShiftExpression(DaoParser::ShiftExpressionContext *context)
    {
        if (!context->shiftExpression())
            return visit(context->additiveExpression());

        auto left = visit(context->shiftExpression()).as<std::shared_ptr<Expression>>();
        auto right = visit(context->additiveExpression()).as<std::shared_ptr<Expression>>();
        auto op = (int)context->op->getType();

        return BinaryExpression::get(op, left, right);
    }

    antlrcpp::Any DaoVisitor::visitAdditiveExpression(DaoParser::AdditiveExpressionContext *context)
    {
        if (!context->additiveExpression())
            return visit(context->multiplicativeExpression());

        auto left = visit(context->additiveExpression()).as<std::shared_ptr<Expression>>();
        auto right = visit(context->multiplicativeExpression()).as<std::shared_ptr<Expression>>();
        auto op = (int)context->op->getType();

        return BinaryExpression::get(op, left, right);
    }

    antlrcpp::Any DaoVisitor::visitMultiplicativeExpression(DaoParser::MultiplicativeExpressionContext *context)
    {
        if (!context->multiplicativeExpression())
            return visit(context->castExpression());

        auto left = visit(context->multiplicativeExpression()).as<std::shared_ptr<Expression>>();
        auto right = visit(context->castExpression()).as<std::shared_ptr<Expression>>();
        auto op = context->op->getType();

        return BinaryExpression::get(op, left, right);
    }

    antlrcpp::Any DaoVisitor::visitCastExpression(DaoParser::CastExpressionContext *context)
    {
        if (context->unaryExpression())
            return visit(context->unaryExpression());

        auto value = visit(context->castExpression()).as<std::shared_ptr<Expression>>();
        auto type = visit(context->typeName()).as<Type *>();

        return CastExpression::get(value, type);
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
            auto func = previous.as<std::shared_ptr<Expression>>();

            if (dynamic_cast<FunctionReference *>(func.get()) == nullptr)
            {
                throw SyntaxError("不支持的函数调用");
            }

            auto arguments = visit(context->argumentExpressionList()).as<std::vector<std::shared_ptr<Expression>>>();
            return CallExpression::get(func, arguments);
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
                return VariableReference::get(var);
            }
            else if (func_list.find(text) != func_list.end())
            {
                auto func = func_list[text];
                return FunctionReference::get(func);
            }
            else
                throw SyntaxError("未知的标识符");
        }
        else if (context->StringLiteral())
        {
            std::string text = context->StringLiteral()->getText();
            text = convert_string(text);
            return Constant<std::string, String>::get(text);
        }
        else if (context->IntegerLiteral())
        {
            std::string text = context->IntegerLiteral()->getText();
            int value = atoi(text.c_str());
            return Constant<int, Int32>::get(value);
        }
        else if (context->FloatLiteral())
        {
            std::string text = context->FloatLiteral()->getText();
            float value = atof(text.c_str());
            return Constant<float, Float>::get(value);
        }
        else if (context->True())
        {
            return Constant<bool, Bool>::get(true);
        }
        else if (context->False())
        {
            return Constant<bool, Bool>::get(false);
        }
        else if (context->Null())
        {
            return Constant<void *, Pointer>::get(nullptr);
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
        std::vector<std::shared_ptr<Expression>> args;
        for (auto expr : context->assignmentExpression())
        {
            auto arg = visit(expr).as<std::shared_ptr<Expression>>();
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
        if (context->Byte())
            return Type::get<TypeId::Byte>();
        else if (context->Int16())
            return Type::get<TypeId::Int16>();
        else if (context->Int32())
            return Type::get<TypeId::Int32>();
        else if (context->Int64())
            return Type::get<TypeId::Int64>();
        else if (context->UInt16())
            return Type::get<TypeId::UInt16>();
        else if (context->UInt32())
            return Type::get<TypeId::UInt32>();
        else if (context->UInt64())
            return Type::get<TypeId::UInt64>();
        else if (context->Half())
            return Type::get<TypeId::Half>();
        else if (context->Float())
            return Type::get<TypeId::Float>();
        else if (context->Double())
            return Type::get<TypeId::Double>();
        else if (context->Bool())
            return Type::get<TypeId::Bool>();
        else if (context->Text())
            return Type::get<TypeId::String>();
        else
            throw SyntaxError("不支持的类型!");
    }

    antlrcpp::Any DaoVisitor::visitFuncName(DaoParser::FuncNameContext *context)
    {
        return DaoParserBaseVisitor::visitFuncName(context);
    }
}