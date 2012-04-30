#include "pnplace.h"

PNPlace::PNPlace()
{
}

bool PNPlace::hasTokens()
{
    return !tokens.empty();
}

TokenVector PNPlace::getTokens()
{
    return tokens;
}

void PNPlace::putToken(pntype token)
{
    tokens.push_back(token);
}
