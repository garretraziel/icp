#include "pntrans.h"

PNTrans::PNTrans()
{
}

PNTrans::~PNTrans()
{
    ConstraintVector::iterator cit;
    for (cit = constraints.begin(); cit < constraints.end(); cit++) {
        delete *cit;
    }
}

bool PNTrans::fire()
{
    bool was_fired = false;

    PlaceVector::iterator it;

    StringToTokensMap hash;

    for (it = ins.begin(); it < ins.end(); it++) {

    }

    return was_fired;
}
