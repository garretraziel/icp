/**
 * @file pntrans.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida pro reprezentaci prechodu simulatoru petriho siti.
 */

#include "pntrans.h"
#include <QDebug>
#include <iostream>

typedef std::vector<int> IntVector;

/**
  * Konstruktor mista.
  */
PNTrans::PNTrans()
{
}

/**
  * Destruktor tridy, uvolni to, co je potreba.
  */
PNTrans::~PNTrans()
{
    ConstraintVector::iterator cit;
    for (cit = constraints.begin(); cit < constraints.end(); cit++) {
        Constraint *c = (*cit); //smazu vsechny podminky
        delete c;
    }
    constraints.clear();
}

/**
  * Zjisti, zda by prechod sel odpalit.
  *
  * @return true pokud odpalit jde, jinak false
  */
bool PNTrans::fire()
{
    StringToPnplaceMap::iterator it;

    StringToTokensMap hash;

    for (it = in_names.begin(); it != in_names.end(); it++) {
        hash[(*it).first] = (*it).second->getTokens(); //ziskam hodnoty ze vstupnich mist
    }

    possible_choices = chooseValues(hash); //najdu veskere kombinace, se kterymi tento prechod muzu odpalit

    if (possible_choices.empty()) { //zadna takova neni, nemohu odpalit
        return false;
    }

    return true;
}

/**
  * Zjisti, ktere z kombinaci vstupnich promennych vyhovuji podminkam, vysledek vrati a ulozi
  *
  * @param hash slovnik poli ve tvaru {"x":[1,2,3],"y":[4,5,6,...]}
  *
  * @return vektor slovniku moznych kombinaci odpaleni
  */
StrPntMapVector PNTrans::chooseValues(StringToTokensMap hash)
{
    StrPntMapVector result; //vektor moznych kombinaci hodnot, co odpovidaji podminkam

    int hashsize = hash.size();

    IntVector positions(hashsize+1,0); //vektor pozic, ze kterych budu brat
    IntVector maximums(hashsize); //vektor velikosti, maximalnich hodnot na dane pozici

    StringToTokensMap::iterator it;
    int i;
    for (i = 0, it = hash.begin(); it != hash.end(); it++, i++) {
        maximums[i] = (*it).second.size(); //ziskam maximalni hodnotu pro danou pozici
        if (maximums[i] == 0) {
            return result; //konec pokud nejake vstupni misto je prazdne
        }
    }

    //toto jsem nazval "hodinarsky algoritmus"
    while (positions[hashsize] == 0) { //dokud neni na posledni nejvyssi pozici jednicka
        StringToPntypeMap possibility; //jedna moznost

        StringToTokensMap::iterator it;
        int i;
        for (i = 0, it = hash.begin(); it != hash.end(); it++, i++) {
            possibility[(*it).first] = (*it).second[positions[i]]; //vyberu hodnoty na mistech podle positions
        }

        ConstraintVector::iterator cit;
        bool possible = true;
        for (cit = constraints.begin(); cit != constraints.end(); cit++) {
            if (!(*cit)->conditionAccepts(possibility)) { //pokud neodpovida nejake podmince
                possible = false; //je tato kombinace spatna
                break;
            }
        }

        if (possible) {
            result.push_back(possibility); //kombinace odpovida vstupnim podminkam, ulozim
        }

        bool add = true; //nakonec prictu zleva jednicku, stejne jako na hodinach
        for (int i = 0; i<=hashsize; i++) {
            if (add) {
                positions[i] += 1; //prictu jedna
                if (i < hashsize && positions[i] >= maximums[i]) {
                    positions[i] = 0; //pokud jsem dosahl max hodnoty, nastavim na nulu a posunu se doleva
                } else {
                    break;
                }
            }
        }
    }

    return result; //vratim kombinace, co odpovidaji
}

/**
  * Provede vybranou operaci podle vektoru moznych konfiguraci.
  *
  * @param choice vyber, ktera kombinace se ma provest
  *
  * @return true pokud se povedl prechod odpalit, jinak false
  */
bool PNTrans::doOperations(unsigned int choice)
{
    if (choice >= possible_choices.size()) return false; //vybrana kombinace neexistuje
    StringToPntypeMap mapping_choice = possible_choices[choice]; //toto je kombinace, se kterou se ma prechod odpalit

    OutputOperations::iterator outit;
    for (outit = operations.begin(); outit < operations.end(); outit++) {
        pntype result = 0; //vysledek je na zacatku nula
        OneOut oneout = (*outit);
        OperationVector::iterator opit; //iteruju pres operace
        for (opit = oneout.operations.begin(); opit < oneout.operations.end(); opit++) {
            Operation op = (*opit);
            bool isNum = false;
            op.var.toInt(&isNum);

            pntype value;

            if (isNum) { //jedna se o konstantu
                value = op.var.toInt();
            } else {
                value = mapping_choice[op.var]; //jedna se o promennou
            }
            if (op.op == ADD) { //pricitam
                result += value;
            } else if (op.op == SUB){ //odecitam
                result -= value;
            } else {
                qCritical() << "Error: bad operation in transition";
                return false;
            }
        }
        out_names[oneout.output]->putToken(result); //na vystup ulozim vysledek
    }

    StringToPntypeMap::iterator remit;

    for (remit = mapping_choice.begin(); remit != mapping_choice.end(); remit++) {
        in_names[(*remit).first]->removeToken((*remit).second); //smazu pouzite tokeny
    }

    return true;
}

/**
  * Vrati pocet moznych konfiguraci k odpaleni.
  *
  * @return pocet konfiguraci
  */
int PNTrans::possibleChoicesCount()
{
    return possible_choices.size(); //vratim velikost moznosti
}

/**
  * Odstrani zadane napojene misto.
  *
  * @param place ukazatel na misto, ktere se ma odebrat
  * @param isIn jestli ma byt vstupni
  */
void PNTrans::removeConnectedPlace(PNPlace * place, bool isIn){
    StringToPnplaceMap * names = (isIn)? &in_names : &out_names;
    StringToPnplaceMap::iterator it;
    for(it=names->begin(); it!=names->end(); it++){
        if((*it).second == place){ //pokud najdu misto co chci smazat
            names->erase(it); //smazu ho
            return;
        }
    }
}
