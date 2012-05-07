/**
 * @file pnplace.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Obsahuje tridu pro reprezentaci mista v petriho siti.
 */

#include "pnplace.h"
#include <algorithm>

PNPlace::PNPlace()
{
}

bool PNPlace::hasTokens()
{
    return !tokens.empty(); //pokud je vektor prazdny, v miste nejsou tokeny
}

TokenVector PNPlace::getTokens()
{
    return tokens;
}

void PNPlace::putToken(pntype token)
{
    tokens.push_back(token); //vlozim token nakonec vektoru
}

bool PNPlace::removeToken(pntype token)
{
    //projdu vektorem a prvni nalezenou hodnotu token smazu
    TokenVector::iterator position = std::find(tokens.begin(), tokens.end(), token);
    if (position != tokens.end()) {
        tokens.erase(position);
        return true;
    }
    return false;
}

void PNPlace::removeTokens(){
    tokens.clear(); //smazu vsechny tokeny
}
