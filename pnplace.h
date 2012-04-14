#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>

typedef int pntype;
typedef std::vector<pntype> TokenVector;

class PNPlace
{
private:
    TokenVector ready_tokens;
    TokenVector new_tokens;
    int x;
    int y;
public:
    PNPlace();
    PNPlace(int x, int y, TokenVector tokens): ready_tokens(tokens), x(x), y(y) {}
    bool hasTokens();
    TokenVector getTokens();
    void updateTokens();
};

#endif // PNPLACE_H
