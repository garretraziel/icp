#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>

typedef int pntype;
typedef std::vector<pntype> TokenVector;

class PNPlace
{
private:
    TokenVector tokens;
    int x;
    int y;
    int id;

public:
    PNPlace();
    PNPlace(int x, int y, int id, TokenVector tokens): tokens(tokens), x(x), y(y), id(id) {}
    bool hasTokens();
    TokenVector getTokens();
    void putToken(pntype token);
};

#endif // PNPLACE_H
