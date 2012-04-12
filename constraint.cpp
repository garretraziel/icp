#include "constraint.h"
#include <QChar>

Constraint::Constraint()
{
}

Constraint::Constraint(StringVector initstrings) {
    StringVector::iterator it;
    for (it = initstrings.begin(); it < initstrings.end(); it++) {

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

bool Constraint::createCondition(QString string_condition) {
    //proparsuju string
    QString first_var;

}
