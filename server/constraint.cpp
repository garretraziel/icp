#include "constraint.h"
#include <QChar>
#include <QDebug>

Constraint::Constraint()
{
    type = TYPENONE;
}

Constraint::Constraint(QString var1, int op, QString var2)
{
    first = var1;
    second_var = var2;
    if (op < 0 || op > 5) {
        qCritical() << "Error: bad operator in condition in XML";
        return;
    }
    this->op = Operators(op);
    type = TYPEVAR;
}

Constraint::Constraint(QString var1, int op, int cons)
{
    first = var1;
    second_const = cons;
    if (op < 0 || op > 5) {
        qCritical() << "Error: bad operator in condition in XML";
        return;
    }
    this->op = Operators(op);
    type = TYPECONST;
}

Constraint::Constraint(QString string_condition)
{
    //proparsuju string
    //todo: kontrolovat, zda to muzu rozrezavat v poradku
    first = string_condition.section(' ',0,0);
    int op = string_condition.section(' ',1,1).toInt();
    QString second_var = string_condition.section(' ',2,2);
    if (second_var.startsWith("v:")) {
        type = TYPEVAR;
        this -> second_var = second_var.remove(0,2);
    } else if (second_var.startsWith("c:")) {
        type = TYPECONST;
        second_var.remove(0,2);
        this -> second_const = second_var.toInt();
    } else {
        qCritical() << "Error: bad value in condition in XML";
        return;
    }
    if (op < 0 || op > 5) {
        qCritical() << "Error: bad operator in condition in XML";
        return;
    }
    this->op = Operators(op);
}

bool Constraint::conditionAccepts(StringToPntypeMap values)
{
    if (type == TYPEANYTHING) return true;

    pntype first = values[this->first];
    pntype second = (type == TYPEVAR)? values[second_var] : second_const;

    bool ok;

    switch(op) {
    case OP_LESS:
        ok = first < second;
        break;
    case OP_LESSEQ:
        ok = first <= second;
        break;
    case OP_MOREEQ:
        ok = first >= second;
        break;
    case OP_MORE:
        ok = first > second;
        break;
    case OP_EQ:
        ok = first == second;
        break;
    case OP_NOTEQ:
        ok = first != second;
        break;
    default:
        ok = false;
        break;
    }

    return ok;
}
