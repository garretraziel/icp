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

void PetriSim::step()
{
    TransVector transits = state.getTransits();

    TransVector::iterator it;

    bool was_fired = false;

    for (it = transits.begin(); it<transits.end(); it++) {
        if ((*it)->fire()) {
            was_fired = true;
        }
    }
}
