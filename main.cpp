#include <QCoreApplication>

#include <QDebug>

#include "infixexpression.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InfixExpression expression("1 + 2 * pow(-3, 4) + ((5 > 7) || 6 < 2 ? max(5, 7) : min(6, 2))");
    //1 2 3 (-) 4 pow * + 5 7 > 6 2 < || 5 7 max 6 2 min ? +
    //165
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc();

    expression.set("-1");
    //1 (-)
    //-1
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc();

    expression.set("1 + 2 * 3 > 4 ? 5 / abs(6) : (7 + 8) * 9");
    //1 2 3 * + 4 > 5 6 abs / 7 8 + 9 * ?
    //0.833
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc();

    expression.set("a > b ? (a > c ? a : c) : (b > c ? b : c)");
    //a b > a c > a c ? b c > b c ? ?
    QMap<QString, double> para;
    para.insert("a", 7);
    para.insert("b", 5);
    para.insert("c", 9);
    qDebug() << expression.getPostfix();
    //qDebug() << QString::fromStdString(expression.getPostfix());
    qDebug() << expression.calc(para);

    return 0;
}
