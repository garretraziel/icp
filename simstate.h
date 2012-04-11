#ifndef SIMSTATE_H
#define SIMSTATE_H

#include <vector>
#include <QtXml>
#include "pnplace.h"
#include "pntrans.h"

typedef std::vector<PNTrans*> TransVector;

// Pro uchovani aktualniho stavu simulace
class SimState
{
private:
    PlaceVector places;
    TransVector transits;
public:
    SimState();
    //todo: a taky zde bude funkce vracejici XML, neco jako getStaticState a getDynamicState
};

#endif // SIMSTATE_H
