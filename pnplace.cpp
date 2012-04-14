#include "pnplace.h"

PNPlace::PNPlace()
{
}

bool PNPlace::hasTokens()
{
    if (ready_tokens.size())
        return true;
    return false;
}

TokenVector PNPlace::getTokens()
{
    return ready_tokens;
}

void PNPlace::updateTokens()
{
    ready_tokens.insert(ready_tokens.end(),new_tokens.begin(),new_tokens.end());
    new_tokens.clear();
}
