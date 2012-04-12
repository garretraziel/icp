#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include <QString>
#include "pnplace.h"

enum Operations {
    ADD,
    SUB
};

typedef std::vector<PNPlace*> PlaceVector;

class PNTrans
{
private:
    PlaceVector ins;
    PlaceVector outs;
    QString constraint;
    QString operation;
    int x;
    int y;
public:
    PNTrans();
    PNTrans(PlaceVector ins, PlaceVector outs, int x, int y)
        :ins(ins),outs(outs),x(x),y(y) {}
};

#endif // PNTRANS_H
