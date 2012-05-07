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

/**
  * Konstruktor tridy. Konstruuje objekt.
  */
PNPlace::PNPlace()
{
}

/**
  * Vrati, jestli misto obsahuje nejake tokeny.
  *
  * @return true pokud misto obsahuje tokeny, jinak false
  */
bool PNPlace::hasTokens()
{
    return !tokens.empty(); //pokud je vektor prazdny, v miste nejsou tokeny
}

/**
  * Vrati seznam tokenu.
  *
  * @return vektor tokenu, ktere obsahuje misto
  */
TokenVector PNPlace::getTokens()
{
    return tokens;
}

/**
  * Vlozi token do mista.
  *
  * @param token token, ktery se ma vlozit
  */
void PNPlace::putToken(pntype token)
{
    tokens.push_back(token); //vlozim token nakonec vektoru
}

/**
  * Smaze token z mista.
  *
  * @param token token, ktery se ma smazat
  *
  * @return true pokud token mohl byt smazan, jinak false
  */
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

/**
  * Smaze veskere tokeny mista.
  */
void PNPlace::removeTokens(){
    tokens.clear(); //smazu vsechny tokeny
}
