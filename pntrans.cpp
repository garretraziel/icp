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
    bool was_fired = false;

    StringToPnplaceMap::iterator it;

    StringToTokensMap hash;

    for (it = in_names.begin(); it != in_names.end(); it++) {
        hash[(*it).first] = (*it).second->getTokens();
    }

    StrPntMapVector result = chooseValues(hash); //todo: bude vracet vector tohohle, kvuli moznostem

    if (!result.empty()) {
        was_fired = true;
        // a tady odeberu prvky a provedu operaci
        StrPntMapVector::iterator it;
        for (it = result.begin(); it != result.end(); it++) {
            using std::cout;
            using std::endl;
            StringToPntypeMap val = (*it);
            StringToPntypeMap::iterator mapiter;
            for (mapiter = val.begin(); mapiter != val.end(); mapiter++) {
                QString name = (*mapiter).first;
                pntype value = (*mapiter).second;
                cout << qPrintable(name) << ": " << value << endl;
            }
            cout << endl;
        }
    }

    return was_fired;
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
