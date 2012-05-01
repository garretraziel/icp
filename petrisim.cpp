#include "petrisim.h"
#include "pnplace.h"
#include "pntrans.h"

PetriSim::PetriSim()
{
}

QString PetriSim::getState()
{
    return state.getState();
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
            (*it)->doOperations(0);
            was_fired = true;
        }
    }
}
