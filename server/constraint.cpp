/**
 * @file constraint.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida constraint reprezentuje jednu podminku, co se muze vyskytnout u hrany.
 */

#include "constraint.h"
#include <QChar>
#include <QDebug>

Constraint::Constraint()
{
    type = TYPENONE; //inicializace typu na "zadny"
}

Constraint::Constraint(QString var1, int op, QString var2)
{
    first = var1;
    second_var = var2; //ulozim jako promennou
    if (op < 0 || op > 5) { //operator je cislo (dle enum) 0-5
        qCritical() << "Error: bad operator in condition in XML";
        return;
    }
    this->op = Operators(op); //prevedu na enum
    type = TYPEVAR; //druhe cislo je "promenna"
}

Constraint::Constraint(QString var1, int op, int cons)
{
    first = var1;
    second_const = cons; //ulozim jako konstantu
    if (op < 0 || op > 5) {
        qCritical() << "Error: bad operator in condition in XML";
        return;
    }
    this->op = Operators(op);
    type = TYPECONST; //druhe cislo je konstanta
}

bool Constraint::conditionAccepts(StringToPntypeMap values)
{
    if (type == TYPEANYTHING) return true; //pokud muze by promenna libovolna, podminka odpovida

    pntype first = values[this->first]; //prvni cislo
    pntype second = (type == TYPEVAR)? values[second_var] : second_const; //druhe cislo

    bool ok;

    switch(op) { //podle operatoru vyhodnotim podminku
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

    return ok; //true/false
}
