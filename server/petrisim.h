/**
 * @file petrisim.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Obsahuje tridu pro simulaci.
 */

#ifndef PETRISIM_H
#define PETRISIM_H

#include "simstate.h"

/**
 * Trida, zastitujici jednu simulaci. Uchovava stav simulace SimState a poskytuje metody pro krokovani.
 */
class PetriSim
{
    SimState state; /// stav simulace
public:
    /**
      * Konstruktor simulace. Konstruuje objekt.
      */
    PetriSim();
    /**
      * Metoda pro spusteni jednoho kroku simulace. Odpaleny prechod se vybira nahodne,
      * protoze petriho site jsou ze sve podstaty nedeterministicke.
      */
    void step();
    /**
      * Metoda pro spusteni simulace az do konce. Bezi, dokud je proveditelny prechod, nebo dokud
      * nevyprsi zadany pocet kroku.
      *
      * @param steps pocet kroku, kterej se maji udelat, implicitne 100
      */
    void run(int steps = 100);

    /**
      * Vrati stav ve formatu XML pro ulozeni nebo prenos.
      *
      * @return XML se stavem simulace
      */
    QString getState();
    /**
      * Nastavi pocatecni stav simulace, proparsuje XML.
      *
      * @param xml XML se stavem simulace
      *
      * @return true pokud se podarilo nastavit simulaci, jinak false
      */
    bool setState(QString xml);
};

#endif // PETRISIM_H
