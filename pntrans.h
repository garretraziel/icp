#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
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
    int constraint;
    int operation;
public:
    PNTrans();
};

#endif // PNTRANS_H
