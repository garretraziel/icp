#include "petrisim.h"
#include "pnplace.h"
#include "pntrans.h"
#include <QtGlobal>
#include <QTime>

int rand_int(int low, int high)
{
    return qrand() % (high - low) + low;
}

PetriSim::PetriSim()
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
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
    TransVector possible_transits;

    bool was_fired = false;
    for (it = transits.begin(); it != transits.end(); it++) {
        if ((*it)->fire()) {
            possible_transits.push_back((*it));
            was_fired = true;
        }
    }

    if (!was_fired) return;

    int which_transit = rand_int(0,possible_transits.size());
    int which_values = rand_int(0,possible_transits[which_transit]->possibleChoicesCount());

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

        int which_transit = rand_int(0,possible_transits.size());
        int which_values = rand_int(0,possible_transits[which_transit]->possibleChoicesCount());

        possible_transits[which_transit]->doOperations(which_values);
    }
}
