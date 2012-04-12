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
    TYPEVAR,
    TYPECONST
};

struct condition {
    ConditionType type;
    QString first;
    Operators op;
    int second_const;
    QString second_var;
};

typedef std::vector<condition> ConditionVector;

class Constraint
{
private:
    ConditionVector conditions;
    bool createCondition(QString string_condition);
public:
    Constraint();
    Constraint(StringVector initstrings);
    StringToPntypeMap chooseValues(StringToTokensMap possible_values);
};

#endif // CONSTRAINT_H
