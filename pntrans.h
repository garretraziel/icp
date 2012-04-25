#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include <QString>
#include <map>
#include "pnplace.h"
#include "constraint.h"

enum Operations {
    ADD,
    SUB
};

typedef std::vector<PNPlace*> PlaceVector;
typedef std::vector<Constraint*> ConstraintVector;
typedef std::map<QString,PNPlace*> StringToPnplaceMap;
typedef std::map<PNPlace*,QString> PnplaceToStringMap;

class PNTrans
{
private:
    ConstraintVector constraints;
    QString operation;
    StringToPnplaceMap in_names;
    StringToPnplaceMap out_names;
    int x;
    int y;

    StringToPntypeMap chooseValues(StringToTokensMap hash);
public:
    PNTrans();
    PNTrans(int x, int y, ConstraintVector constraints,
            StringToPnplaceMap in_names, StringToPnplaceMap out_names)
        :constraints(constraints),in_names(in_names),out_names(out_names),x(x),y(y) {}
    ~PNTrans();
    bool fire();
};

#endif // PNTRANS_H
