#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>
#include <QString>

typedef int pntype;
typedef std::vector<pntype> TokenVector;

class PNPlace
{
private:
    TokenVector tokens;
    int x;
    int y;

public:
    QString id;

    PNPlace();
    PNPlace(int x, int y, QString id, TokenVector tokens): tokens(tokens), x(x), y(y), id(id) {}
    bool hasTokens();
    TokenVector getTokens();
    void putToken(pntype token);
};

#endif // PNPLACE_H
