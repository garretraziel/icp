#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <QString>
#include <vector>
#include <map>
#include "pnplace.h"

typedef std::vector<QString> StringVector;
typedef std::map<QString,TokenVector> StringToTokensMap;
typedef std::map<QString,pntype> StringToPntypeMap;

class Constraint
{
private:
    StringVector conditions;
public:
    Constraint();
    Constraint(StringVector initstrings)
        :conditions(initstrings) {}
    StringToPntypeMap chooseValues(StringToTokensMap possible_values);
};

#endif // CONSTRAINT_H
