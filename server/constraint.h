/**
 * @file constraint.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida constraint reprezentuje jednu podminku, co se muze vyskytnout u hrany.
 */

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <QString>
#include <vector>
#include <map>
#include "pnplace.h"

typedef std::vector<QString> StringVector;
typedef std::map<QString,TokenVector> StringToTokensMap;
typedef std::map<QString,pntype> StringToPntypeMap;

/** Seznam operatoru v podmince
 */
enum Operators {
    OP_LESS, ///< mensi nez
    OP_LESSEQ, ///< mensi nebo rovno nez
    OP_MOREEQ, ///< vetsi nebo rovno nez
    OP_MORE, ///< vetsi nez
    OP_EQ, ///< rovno
    OP_NOTEQ ///< nerovno
};

/** Seznam typu druheho parametru
 */
enum ConditionType {
    TYPENONE, ///< zadny typ podminky
    TYPEVAR, ///< druhy operator je promenna
    TYPECONST, ///< druhy operator je konstanta
    TYPEANYTHING ///< druhy operator nepouzit
};

/**
 *
 * Trida reprezentujici jednu podminku u hrany.
 *
 */
class Constraint
{
public:
    ConditionType type; ///< typ podminky
    QString first; ///< prvni promenna podminky
    Operators op; ///< operator podminky
    QString second_var; ///< druha promenna podminky
    int second_const; ///< konstanta v podmince

    /**
      * Konstruktor podminky, nastavy typ na none.
      */
    Constraint();
    /**
      * Konstruktor podminky, ktery nastavy typ a promenne.
      *
      * @param var1 prvni promenna podminky
      * @param op operator podminky
      * @param var2 druha promenna podminky
      */
    Constraint(QString var1, int op, QString var2);
    /**
      * Konstruktor podminky, ktery nastavy promennou a konstantu.
      *
      * @param var1 prvni promenna podminky
      * @param op operator podminky
      * @param cons konstanta podminky
      */
    Constraint(QString var1, int op, int cons);
    /**
      * Konstruktor podminky, nastavy prvni promennou a typ.
      *
      * @param variable prvni promenna
      * @param type typ podminky
      */
    Constraint(QString variable, ConditionType type)
        :type(type),first(variable) {}
    /**
      * Zjisti, zda zadane hodnoty odpovidaji podmince.
      *
      * @param values slovnik promenna:hodnota, pro ktere podminku kontroluji
      *
      * @return true pokud odpovidaji vstupni hodnoty podmince, jinak false
      */
    bool conditionAccepts(StringToPntypeMap values);
};

#endif // CONSTRAINT_H
