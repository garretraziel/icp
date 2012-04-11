#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include "pnplace.h"

class PNPlace;

typedef std::vector<PNPlace> PlaceVector;

class PNTrans
{
private:
    PlaceVector ins;
    PlaceVector outs;
public:
    PNTrans();
};

#endif // PNTRANS_H
