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
public:
    PNPlace();
    PNPlace(int x, int y, TokenVector tokens): tokens(tokens), x(x), y(y) {}
    bool hasTokens();
};

#endif // PNPLACE_H
