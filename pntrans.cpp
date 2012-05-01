#include "pntrans.h"
#include <QDebug>
#include <iostream>

typedef std::vector<int> IntVector;

PNTrans::PNTrans()
{
}

PNTrans::~PNTrans()
{
    ConstraintVector::iterator cit;
    for (cit = constraints.begin(); cit < constraints.end(); cit++) {
        delete *cit;
    }
}

bool PNTrans::fire()
{
    StringToPnplaceMap::iterator it;

    StringToTokensMap hash;

    for (it = in_names.begin(); it != in_names.end(); it++) {
        hash[(*it).first] = (*it).second->getTokens();
    }

    possible_choices = chooseValues(hash); //todo: bude vracet vector tohohle, kvuli moznostem

    if (possible_choices.empty()) {
        return false;
    }

    return true;
}

StrPntMapVector PNTrans::chooseValues(StringToTokensMap hash)
{
    StrPntMapVector result;

    int hashsize = hash.size();

    IntVector positions(hashsize+1,0);
    IntVector maximums(hashsize);

    StringToTokensMap::iterator it;
    int i;
    for (i = 0, it = hash.begin(); it != hash.end(); it++, i++) {
        maximums[i] = (*it).second.size();
    }

    while (positions[hashsize] == 0) {
        StringToPntypeMap possibility;

        StringToTokensMap::iterator it;
        int i;
        for (i = 0, it = hash.begin(); it != hash.end(); it++, i++) {
            possibility[(*it).first] = (*it).second[positions[i]];
        }

        //todo: zjistit jestli vyhovuje, kdyztak ho pridat do result
        ConstraintVector::iterator cit;
        bool possible = true;
        for (cit = constraints.begin(); cit != constraints.end(); cit++) {
            if (!(*cit)->conditionAccepts(possibility)) {
                possible = false;
                break;
            }
        }

        if (possible) {
            result.push_back(possibility);
        }

        bool add = true;
        for (int i = 0; i<=hashsize; i++) {
            if (add) {
                positions[i] += 1;
                if (i < hashsize && positions[i] >= maximums[i]) {
                    positions[i] = 0;
                } else {
                    break;
                }
            }
        }
    }

    return result;
}

bool PNTrans::doOperations(unsigned int choice)
{
    if (choice >= possible_choices.size()) return false;
    StringToPntypeMap mapping_choice = possible_choices[choice];

    OutputOperations::iterator outit;
    for (outit = operations.begin(); outit < operations.end(); outit++) {
        pntype result = 0;
        OneOut oneout = (*outit);
        OperationVector::iterator opit;
        for (opit = oneout.operations.begin(); opit < oneout.operations.end(); opit++) {
            Operation op = (*opit);
            pntype value = mapping_choice[op.var];
            if (op.op == ADD) {
                result += value;
            } else if (op.op == SUB){
                result -= value;
            } else {
                qCritical() << "Error: bad operation in transition";
                return false;
            }
            in_names[op.var]->removeToken(value); //todo: kontrolovat spravne smazani
        }
        out_names[oneout.output]->putToken(result);
    }

    return true;
}
