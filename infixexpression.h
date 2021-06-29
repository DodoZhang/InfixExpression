#ifndef INFIXEXPRESSION_H
#define INFIXEXPRESSION_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QList>
#include <QStack>
#include <QMap>
#define IEstring QString
#define IElist QList<ExpressionUnit>
#define IEstack QStack<ExpressionUnit>
#define IEnumstack QStack<double>
#define IEmap QMap<IEstring, double>
#define IEGetChar(iter) ((iter)->toLatin1())
#define IEAppendNum(str, num) (str).append(QString::number(num))
#define IEMapFind(map, key) ((map).value(key))
#else
#include <string>
#include <list>
#include <stack>
#include <map>
#define IEstring std::string
#define IElist std::list<ExpressionUnit>
#define IEstack std::stack<ExpressionUnit>
#define IEnumstack std::stack<double>
#define IEmap std::map<IEstring, double>
#define IEGetChar(iter) (*(iter))
#define IEAppendNum(str, num) (str).append(std::to_string(num))
#define IEMapFind(map, key) ((map).find(key)->second)
#endif


class InfixExpression
{
public:
    InfixExpression(const IEstring &expression);

    void set(const IEstring &expression);
    const IEstring &get();
    IEstring getPostfix();
    double calc(const IEmap &parameters = IEmap());

protected:
    class ExpressionUnit
    {
    public:
        enum : uint8_t { Number, Variable, Operator } type;
        double num;
        IEstring var;
        enum : uint8_t {
            L_BRACKET, R_BRACKET, COMMA,
            ADD, SUB, MULT, DIV, MOD, POWER,
            EQUALS, NOT_EQUALS, GREATER, LESS, GREATER_EQUALS, LESS_EQUALS,
            NOT, OR, AND,
            TERNARY, TERNARY2,
            SIN, COS, TAN,
            MAX, MIN, ABS,
            FLOOR, ROUND, CEIL
        } oper;

    public:
        ExpressionUnit();
        ExpressionUnit(double num);
        ExpressionUnit(IEstring str);
        ExpressionUnit(typeof (oper) oper);

        int getPriority();
        void calculate(IEnumstack &stack) const;
    };

protected:
    ExpressionUnit getExpressionUnit(IEstring::const_iterator &iter);

protected:
    IEstring expression;
    IElist postfix;
};

#endif // INFIXEXPRESSION_H
