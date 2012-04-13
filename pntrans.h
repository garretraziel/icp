#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include <QString>
#include "pnplace.h"
#include "constraint.h"

enum Operations {
    ADD,
    SUB
};

typedef std::vector<PNPlace*> PlaceVector;
typedef std::vector<Constraint*> ConstraintVector;

class PNTrans
{
private:
    PlaceVector ins;
    PlaceVector outs;
    ConstraintVector constraints;
    QString operation;
    int x;
    int y;
public:
    PNTrans();
    PNTrans(PlaceVector ins, PlaceVector outs, int x, int y, ConstraintVector constraints)
        :ins(ins),outs(outs),constraints(constraints),x(x),y(y) {}
    ~PNTrans();
};

#endif // PNTRANS_H
