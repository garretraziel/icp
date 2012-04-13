#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <QString>
#include <vector>
#include <map>
#include "pnplace.h"

typedef std::vector<QString> StringVector;
typedef std::map<QString,TokenVector> StringToTokensMap;
typedef std::map<QString,pntype> StringToPntypeMap;

enum Operators {
    OP_LESS,
    OP_LESSEQ,
    OP_MOREEQ,
    OP_MORE,
    OP_EQ,
    OP_NOTEQ
};

enum ConditionType {
    TYPENONE,
    TYPEVAR,
    TYPECONST
};

class Constraint
{
private:
    ConditionType type;
    QString first;
    Operators op;
    QString second_var;
    int second_const;
public:
    Constraint();
    Constraint(QString string_condition);
    StringToPntypeMap chooseValues(StringToTokensMap possible_values);
    bool conditionAccepts(StringToPntypeMap values);
};

#endif // CONSTRAINT_H
