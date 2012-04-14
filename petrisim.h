#ifndef PETRISIM_H
#define PETRISIM_H

#include "simstate.h"

class PetriSim
{
    SimState state; //stav simulace
public:
    PetriSim();
    void step();
    void run();

    SimState getState(); //vrati stav pro ulozeni nebo prenos
    bool setState(QString xml); // nastavi stav, pro nahrani simulace
};

#endif // PETRISIM_H
