/**
 * @file petrisim.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Obsahuje tridu pro simulaci.
 */

#include "petrisim.h"
#include "pnplace.h"
#include "pntrans.h"
#include <QtGlobal>

int rand_int(int high)
{
    return qrand() % high;
}

PetriSim::PetriSim()
{
}

QString PetriSim::getState()
{
    return state.getState();
}

bool PetriSim::setState(QString xml)
{
    return this -> state.setState(xml);
}

void PetriSim::step()
{
    TransVector transits = state.getTransits();
    TransVector::iterator it;
    TransVector possible_transits;

    bool was_fired = false;
    for (it = transits.begin(); it != transits.end(); it++) {
        if ((*it)->fire()) {
            possible_transits.push_back((*it));
            was_fired = true;
        }
    }

    if (!was_fired) return;

    int which_transit = rand_int(possible_transits.size());
    int which_values = rand_int(possible_transits[which_transit]->possibleChoicesCount());

    possible_transits[which_transit]->doOperations(which_values);
}

void PetriSim::run(int steps)
{
    for (int i = 0; i<steps; i++) {
        TransVector transits = state.getTransits();
        TransVector::iterator it;
        TransVector possible_transits;

        bool was_fired = false;
        for (it = transits.begin(); it != transits.end(); it++) {
            if ((*it)->fire()) {
                possible_transits.push_back((*it));
                was_fired = true;
            }
        }

        if (!was_fired) return;

        int which_transit = rand_int(possible_transits.size());
        int which_values = rand_int(possible_transits[which_transit]->possibleChoicesCount());

        possible_transits[which_transit]->doOperations(which_values);
    }
}
