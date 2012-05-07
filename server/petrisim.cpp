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

/**
  * Vrati nahodnout hodnotu do zadane hodnoty.
  * Pozor: nikdy nevolat s parametrem 0.
  *
  * @param high max nahodneho cisla
  *
  * @return nahodna hodnota
  */
int rand_int(int high)
{
    if (high == 0) return 0;
    return qrand() % high;
}

/**
  * Konstruktor simulace. Konstruuje objekt.
  */
PetriSim::PetriSim()
{
}

/**
  * Vrati stav ve formatu XML pro ulozeni nebo prenos.
  *
  * @return XML se stavem simulace
  */
QString PetriSim::getState()
{
    return state.getState(); //getter simstate
}

/**
  * Nastavi pocatecni stav simulace, proparsuje XML.
  *
  * @param xml XML se stavem simulace
  *
  * @return true pokud se podarilo nastavit simulaci, jinak false
  */
bool PetriSim::setState(QString xml)
{
    return this -> state.setState(xml); //setter simstate
}

/**
  * Metoda pro spusteni jednoho kroku simulace. Odpaleny prechod se vybira nahodne,
  * protoze petriho site jsou ze sve podstaty nedeterministicke.
  */
void PetriSim::step()
{
    TransVector transits = state.getTransits(); //vytahne si vsechny prechody
    TransVector::iterator it;
    TransVector possible_transits; //prechody, ktere by se mohly odpalit

    bool was_fired = false; //pokud se mohl aspon jeden prechod odpalit
    for (it = transits.begin(); it != transits.end(); it++) {
        if ((*it)->fire()) { //tento prechod se muze odpalit,
            possible_transits.push_back((*it)); //pridam ho do vektoru prechodu k odpaleni
            was_fired = true;
        }
    }

    if (!was_fired) return;

    int which_transit = rand_int(possible_transits.size()); //vyberu nahodny prechod
    int which_values = rand_int(possible_transits[which_transit]->possibleChoicesCount()); //k tomu nahodnou konfiguraci

    possible_transits[which_transit]->doOperations(which_values); //odpalim
}

/**
  * Metoda pro spusteni simulace az do konce. Bezi, dokud je proveditelny prechod, nebo dokud
  * nevyprsi zadany pocet kroku.
  *
  * @param steps pocet kroku, kterej se maji udelat, implicitne 100
  */
void PetriSim::run(int steps)
{
    for (int i = 0; i<steps; i++) { //run je omezeno poctem kroku, ochrana proti zacykleni
        TransVector transits = state.getTransits(); //vytahnu vsechny prechody
        TransVector::iterator it;
        TransVector possible_transits; //prechody k odpaleni

        bool was_fired = false;
        for (it = transits.begin(); it != transits.end(); it++) {
            if ((*it)->fire()) { //prechod se muze odpalit
                possible_transits.push_back((*it)); //pridam ho do vektoru prechodu k odpaleni
                was_fired = true;
            }
        }

        if (!was_fired) return;

        int which_transit = rand_int(possible_transits.size()); //vyberu nahodny prechod
        int which_values = rand_int(possible_transits[which_transit]->possibleChoicesCount()); //k tomu nahodnou konfiguraci

        possible_transits[which_transit]->doOperations(which_values); //odpalim
    }
}
