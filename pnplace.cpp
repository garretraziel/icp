#include "pnplace.h"

PNPlace::PNPlace()
{
}

bool PNPlace::hasTokens()
{
    if (tokens.size())
        return true;
    return false;
}
