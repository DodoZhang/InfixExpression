#include <QCoreApplication>

#include <QDebug>

#include "infixexpression.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InfixExpression expression("1 + 2 * 3 ^ 4 + ((5 > 7) || 6 < 2 ? max(5, 7) : min(6, 2))");
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc();

    expression = InfixExpression("a > b ? (a > c ? a : c) : (b > c ? b : c)");
    QMap<QString, double> para;
    para.insert("a", 7);
    para.insert("b", 5);
    para.insert("c", 9);
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc(para);

    return 0;
}
