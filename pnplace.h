#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>
#include "pntrans.h"

class PNTrans;

typedef int pntype;
typedef std::vector<pntype> TokenVector;
typedef std::vector<PNTrans> TransVector;

class PNPlace
{
private:
    TokenVector tokens;
    TransVector inputs;
    TransVector outputs;
public:
    PNPlace();
};

#endif // PNPLACE_H
