#include "constraint.h"
#include <QChar>
#include <QDebug>

Constraint::Constraint()
{
}

Constraint::Constraint(StringVector initstrings) {
    StringVector::iterator it;
    for (it = initstrings.begin(); it < initstrings.end(); it++) {
        if (!createCondition(*it)) {
            qCritical() << "Error: cannot create constraints properly.";
            return;
        }
    }
}

StringToPntypeMap Constraint::chooseValues(StringToTokensMap possible_values)
{
    StringToPntypeMap results;
    //todo: backtrackingem prohleda possible_values
    // pracuje se s tim takto: je to slovnik, kde vzdy na pozici jmena promenne ("x")
    // je ulozen vektor hodnot, co ta promenna muze nabyvat

    //vysledkem je mapa string na hodnotu, napr. "x" : 5. pokud neni mozno, vysledek je nulove delky
    return results;
}

bool Constraint::createCondition(QString string_condition)
{
    //proparsuju string
    //todo: kontrolovat, zda to muzu rozrezavat v poradku
    QString first_var = string_condition.section(' ',0,0);
    int op = string_condition.section(' ',1,1).toInt();
    QString second_var = string_condition.section(' ',2,2);
    struct condition cond;
    cond.first = first_var;
    if (second_var.startsWith("v:")) {
        cond.type = TYPEVAR;
        cond.second_var = second_var.remove(0,2);
    } else if (second_var.startsWith("c:")) {
        cond.type = TYPECONST;
        cond.second_const = second_var.toInt();
    } else {
        qCritical() << "Error: bad value in condition in XML";
        return false;
    }
    if (op < 0 || op > 5) {
        qCritical() << "Error: bad operator in condition in XML";
        return false;
    }
    cond.op = Operators(op);
    conditions.push_back(cond);
    return true;
}

bool Constraint::conditionAccepts(struct condition cond, StringToPntypeMap values)
{
    pntype first = values[cond.first];
    pntype second = (cond.type == TYPEVAR)? values[cond.second_var] : cond.second_const;

    bool ok;

    switch(cond.op) {
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
