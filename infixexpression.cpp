#include "infixexpression.h"

#include "math.h"

#include <QDebug>

InfixExpression::InfixExpression(const IEstring &expression)
{
    set(expression);
}

void InfixExpression::set(const IEstring &expression)
{
    this->expression = expression;
    IEstring::const_iterator iter = expression.cbegin();
    IEstack operators;
    while (iter != expression.cend()) {
        char ch = IEGetChar(iter);
        while (ch == ' ' || ch == '\t' || ch == '\n') {
            iter ++;
            ch = IEGetChar(iter);
        }
        if (iter == expression.cend()) break;
        ExpressionUnit unit = getExpressionUnit(iter);
        if (unit.type == ExpressionUnit::Operator) {
            if (unit.oper == ExpressionUnit::L_BRACKET) operators.push(unit);
            else if (unit.oper == ExpressionUnit::R_BRACKET)
            {
                ExpressionUnit tmp = operators.top();
                operators.pop();
                while (tmp.oper != ExpressionUnit::L_BRACKET) {
                    postfix.push_back(tmp);
                    tmp = operators.top();
                    operators.pop();
                }
                if (!operators.empty() && operators.top().getPriority() == 9)
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
            }
            else if (unit.oper == ExpressionUnit::COMMA)
            {
                ExpressionUnit tmp = operators.top();
                operators.pop();
                while (tmp.oper != ExpressionUnit::L_BRACKET) {
                    postfix.push_back(tmp);
                    tmp = operators.top();
                    operators.pop();
                }
                operators.push(tmp);
            }
            else if (unit.oper == ExpressionUnit::TERNARY)
            {
                while (!operators.empty()) {
                    ExpressionUnit tmp = operators.top();
                    operators.pop();
                    if (tmp.oper == ExpressionUnit::TERNARY || tmp.oper == ExpressionUnit::L_BRACKET)
                    {
                        operators.push(tmp);
                        break;
                    }
                    postfix.push_back(tmp);
                }
                operators.push(unit);
            }
            else if (unit.oper == ExpressionUnit::TERNARY2)
            {
                ExpressionUnit tmp = operators.top();
                operators.pop();
                while (tmp.oper != ExpressionUnit::TERNARY) {
                    postfix.push_back(tmp);
                    tmp = operators.top();
                    operators.pop();
                }
                operators.push(tmp);
            }
            else
            {
                while (!operators.empty() && operators.top().getPriority() >= unit.getPriority())
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(unit);
            }
        } else postfix.push_back(unit);
    }
    while (!operators.empty())
    {
        postfix.push_back(operators.top());
        operators.pop();
    }
}

const IEstring &InfixExpression::get()
{
    return expression;
}

IEstring InfixExpression::getPostfix()
{
    IEstring str;
    for (IElist::const_iterator iter = postfix.cbegin();
         iter != postfix.cend(); iter ++)
    {
        if (iter->type == ExpressionUnit::Number) IEAppendNum(str,iter->num);
        else if (iter->type == ExpressionUnit::Variable) str.append(iter->var);
        else
        {
            switch (iter->oper) {
                case ExpressionUnit::L_BRACKET: str.append("("); break;
                case ExpressionUnit::COMMA: str.append(","); break;
                case ExpressionUnit::TERNARY: str.append("?"); break;
                case ExpressionUnit::TERNARY2: str.append(":"); break;
                case ExpressionUnit::SIN: str.append("sin"); break;
                case ExpressionUnit::COS: str.append("cos"); break;
                case ExpressionUnit::TAN: str.append("tan"); break;
                case ExpressionUnit::MAX: str.append("max"); break;
                case ExpressionUnit::MIN: str.append("min"); break;
                case ExpressionUnit::ABS: str.append("abs"); break;
                case ExpressionUnit::FLOOR: str.append("floor"); break;
                case ExpressionUnit::ROUND: str.append("round"); break;
                case ExpressionUnit::CEIL: str.append("ceil"); break;
                case ExpressionUnit::OR: str.append("||"); break;
                case ExpressionUnit::AND: str.append("&&"); break;
                case ExpressionUnit::EQUALS: str.append("=="); break;
                case ExpressionUnit::NOT_EQUALS: str.append("!="); break;
                case ExpressionUnit::GREATER: str.append(">"); break;
                case ExpressionUnit::LESS: str.append("<"); break;
                case ExpressionUnit::GREATER_EQUALS: str.append(">="); break;
                case ExpressionUnit::LESS_EQUALS: str.append("<="); break;
                case ExpressionUnit::ADD: str.append("+"); break;
                case ExpressionUnit::SUB: str.append("-"); break;
                case ExpressionUnit::MULT: str.append("*"); break;
                case ExpressionUnit::DIV: str.append("/"); break;
                case ExpressionUnit::MOD: str.append("%"); break;
                case ExpressionUnit::POWER: str.append("^"); break;
                case ExpressionUnit::NOT: str.append("!"); break;
                case ExpressionUnit::R_BRACKET: str.append("("); break;
            }
        }
        str.append(" ");
    }
    return str;
}

double InfixExpression::calc(const IEmap &parameters)
{
    IEnumstack numbers;
    for (IElist::const_iterator iter = postfix.cbegin();
         iter != postfix.cend(); iter ++)
    {
        if (iter->type == ExpressionUnit::Number) numbers.push(iter->num);
        else if (iter->type == ExpressionUnit::Variable) numbers.push(IEMapFind(parameters,iter->var));
        else iter->calculate(numbers);
    }
    return numbers.top();
}

InfixExpression::ExpressionUnit InfixExpression::getExpressionUnit(IEstring::const_iterator &iter)
{
    char ch = IEGetChar(iter);
    if (ch >= '0' && ch <= '9')
    {
        double number = 0;
        int power10 = 0;
        bool decimal = false;
        while (ch != '\0') {
            if (ch >= '0' && ch <= '9')
            {
                number *= 10;
                number += ch - '0';
                if (decimal) power10 ++;
            }
            else if (ch == '.') decimal = true;
            else break;
            iter ++;
            ch = IEGetChar(iter);
        }
        for (int i = 0; i < power10; i ++) number /= 10;
        return ExpressionUnit(number);
    }
    else if ((ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') ||
              ch == '_')
    {
        IEstring str;
        while ((ch >= 'A' && ch <= 'Z') ||
               (ch >= 'a' && ch <= 'z') ||
               (ch >= '0' && ch <= '9') ||
               ch == '_') {
            str += ch;
            iter ++;
            ch = IEGetChar(iter);
        }
        return ExpressionUnit(str);
    }
    else
    {
        switch (ch) {
        case '(': iter ++; return ExpressionUnit(ExpressionUnit::L_BRACKET);
        case ')': iter ++; return ExpressionUnit(ExpressionUnit::R_BRACKET);
        case ',': iter ++; return ExpressionUnit(ExpressionUnit::COMMA);
        case '+': iter ++; return ExpressionUnit(ExpressionUnit::ADD);
        case '-': iter ++; return ExpressionUnit(ExpressionUnit::SUB);
        case '*': iter ++; return ExpressionUnit(ExpressionUnit::MULT);
        case '/': iter ++; return ExpressionUnit(ExpressionUnit::DIV);
        case '%': iter ++; return ExpressionUnit(ExpressionUnit::MOD);
        case '^': iter ++; return ExpressionUnit(ExpressionUnit::POWER);
        case '=':
            iter ++;
            ch = IEGetChar(iter);
            if (ch == '=')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::EQUALS);
            }
            else throw "invalid operator";
        case '!':
            iter ++;
            if (ch == '=')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::NOT_EQUALS);
            }
            else return ExpressionUnit(ExpressionUnit::NOT);
        case '>':
            iter ++;
            if (ch == '=')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::GREATER_EQUALS);
            }
            return ExpressionUnit(ExpressionUnit::GREATER);
        case '<':
            iter ++;
            if (ch == '=')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::LESS_EQUALS);
            }
            return ExpressionUnit(ExpressionUnit::LESS);
        case '|':
            iter ++;
            ch = IEGetChar(iter);
            if (ch == '|')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::OR);
            }
            else throw "invalid operator";
        case '&':
            iter ++;
            ch = IEGetChar(iter);
            if (ch == '&')
            {
                iter ++;
                return ExpressionUnit(ExpressionUnit::AND);
            }
            else throw "invalid operator";
        case '?': iter ++; return ExpressionUnit(ExpressionUnit::TERNARY);
        case ':': iter ++; return ExpressionUnit(ExpressionUnit::TERNARY2);
        default: throw "invalid operator: \'" + std::to_string(ch) + "\'";
        }
    }
}

InfixExpression::ExpressionUnit::ExpressionUnit()
{

}

InfixExpression::ExpressionUnit::ExpressionUnit(double num)
{
    type = Number;
    this->num = num;
}

InfixExpression::ExpressionUnit::ExpressionUnit(IEstring str)
{
    type = Operator;
    if (str.compare("(") == 0) oper = L_BRACKET;
    else if (str.compare(")") == 0) oper = R_BRACKET;
    else if (str.compare(",") == 0) oper = COMMA;
    else if (str.compare("+") == 0) oper = ADD;
    else if (str.compare("-") == 0) oper = SUB;
    else if (str.compare("*") == 0) oper = MULT;
    else if (str.compare("/") == 0) oper = DIV;
    else if (str.compare("%") == 0) oper = MOD;
    else if (str.compare("^") == 0) oper = POWER;
    else if (str.compare("==") == 0) oper = EQUALS;
    else if (str.compare("!=") == 0) oper = NOT_EQUALS;
    else if (str.compare(">") == 0) oper = GREATER;
    else if (str.compare("<") == 0) oper = LESS;
    else if (str.compare(">=") == 0) oper = GREATER_EQUALS;
    else if (str.compare("<=") == 0) oper = LESS_EQUALS;
    else if (str.compare("!") == 0) oper = NOT;
    else if (str.compare("||") == 0) oper = OR;
    else if (str.compare("&&") == 0) oper = AND;
    else if (str.compare("?") == 0) oper = TERNARY;
    else if (str.compare(":") == 0) oper = TERNARY2;
    else if (str.compare("sin") == 0) oper = SIN;
    else if (str.compare("cos") == 0) oper = COS;
    else if (str.compare("tan") == 0) oper = TAN;
    else if (str.compare("max") == 0) oper = MAX;
    else if (str.compare("min") == 0) oper = MIN;
    else if (str.compare("abs") == 0) oper = ABS;
    else if (str.compare("floor") == 0) oper = FLOOR;
    else if (str.compare("int") == 0) oper = FLOOR;
    else if (str.compare("round") == 0) oper = ROUND;
    else if (str.compare("ceil") == 0) oper = CEIL;
    else
    {
        type = Variable;
        var = str;
    }
}

InfixExpression::ExpressionUnit::ExpressionUnit(typeof (oper) oper)
{
    type = Operator;
    this->oper = oper;
}

int InfixExpression::ExpressionUnit::getPriority()
{
    if (type != Operator) return -1;
    switch (oper) {
    case L_BRACKET:
    case COMMA:
    case TERNARY:
    case TERNARY2:
        return 1;
    case OR:
        return 2;
    case AND:
        return 3;
    case EQUALS:
    case NOT_EQUALS:
    case GREATER:
    case LESS:
    case GREATER_EQUALS:
    case LESS_EQUALS:
        return 4;
    case ADD:
    case SUB:
        return 5;
    case MULT:
    case DIV:
    case MOD:
        return 6;
    case POWER:
        return 7;
    case NOT:
        return 8;
    case SIN:
    case COS:
    case TAN:
    case MAX:
    case MIN:
    case ABS:
    case FLOOR:
    case ROUND:
    case CEIL:
        return 9;
    case R_BRACKET:
        return 10;
    }
}

void InfixExpression::ExpressionUnit::calculate(IEnumstack &stack) const
{
    switch (oper) {
    double cond, exp1, exp2;
    case TERNARY:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        cond = stack.top();
        stack.pop();
        if (cond >= 0) stack.push(exp1);
        else stack.push(exp2);
        break;
    case SIN:
        exp1 = stack.top();
        stack.pop();
        stack.push(sin(exp1));
        break;
    case COS:
        exp1 = stack.top();
        stack.pop();
        stack.push(cos(exp1));
        break;
    case TAN:
        exp1 = stack.top();
        stack.pop();
        stack.push(tan(exp1));
        break;
    case MAX:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 > exp2 ? exp1 : exp2);
        break;
    case MIN:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 < exp2 ? exp1 : exp2);
        break;
    case ABS:
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 > 0 ? exp1 : -exp1);
        break;
    case FLOOR:
        exp1 = stack.top();
        stack.pop();
        stack.push(floor(exp1));
        break;
    case ROUND:
        exp1 = stack.top();
        stack.pop();
        stack.push(round(exp1));
        break;
    case CEIL:
        exp1 = stack.top();
        stack.pop();
        stack.push(ceil(exp1));
        break;
    case OR:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 >= 0 || exp2 >= 0 ? 1 : -1);
        break;
    case AND:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 >= 0 && exp2 >= 0 ? 1 : -1);
        break;
    case EQUALS:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 == exp2 ? 1 : -1);
        break;
    case NOT_EQUALS:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 != exp2 ? 1 : -1);
        break;
    case GREATER:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 > exp2 ? 1 : -1);
        break;
    case LESS:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 < exp2 ? 1 : -1);
        break;
    case GREATER_EQUALS:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 >= exp2 ? 1 : -1);
        break;
    case LESS_EQUALS:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 <= exp2 ? 1 : -1);
        break;
    case ADD:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 + exp2);
        break;
    case SUB:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 - exp2);
        break;
    case MULT:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 * exp2);
        break;
    case DIV:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 / exp2);
        break;
    case MOD:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 - ((int) (exp1 / exp2)) * exp2);
        break;
    case POWER:
        exp2 = stack.top();
        stack.pop();
        exp1 = stack.top();
        stack.pop();
        stack.push(pow(exp1, exp2));
        break;
    case NOT:
        exp1 = stack.top();
        stack.pop();
        stack.push(exp1 >= 0 ? -1 : 1);
        break;
    default:
        break;
    }
}
