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

class PNTrans
{
private:
    PlaceVector ins;
    PlaceVector outs;
    ConstraintVector constraints;
    QString operation;
    StringToPnplaceMap in_names;
    StringToPnplaceMap out_names;
    int x;
    int y;
public:
    PNTrans();
    PNTrans(PlaceVector ins, PlaceVector outs, int x, int y, ConstraintVector constraints,
            StringToPnplaceMap in_names, StringToPnplaceMap out_names)
        :ins(ins),outs(outs),constraints(constraints),in_names(in_names),out_names(out_names),x(x),y(y) {}
    ~PNTrans();
    bool fire();
};

#endif // PNTRANS_H
