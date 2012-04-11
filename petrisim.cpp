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

bool PetriSim::setState(SimState state)
{
    this -> state = state;
    //todo: vse co je potreba pro nastaveni stavu
    return true;
}
