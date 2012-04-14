#include "petrisim.h"
#include "pnplace.h"
#include "pntrans.h"

PetriSim::PetriSim()
{
}

SimState PetriSim::getState()
{
    return state;
}

bool PetriSim::setState(QString xml)
{
    //todo: vse co je potreba pro nastaveni stavu
    return this -> state.setState(xml);
}
