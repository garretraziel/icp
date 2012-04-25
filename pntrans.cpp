#include "pntrans.h"

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

    StringToPntypeMap result = chooseValues(hash); //todo: bude vracet vector tohohle, kvuli moznostem

    if (!result.empty()) {
        was_fired = true;
        // a tady odeberu prvky a provedu operaci
    }

    return was_fired;
}

StringToPntypeMap PNTrans::chooseValues(StringToTokensMap hash)
{
    StringToPntypeMap result;


}
