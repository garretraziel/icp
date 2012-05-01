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

public:
    QString id;
    QString x;
    QString y;

    PNPlace();
    PNPlace(QString x, QString y, QString id, TokenVector tokens): tokens(tokens),id(id),x(x),y(y) {}
    bool hasTokens();
    TokenVector getTokens();
    void putToken(pntype token);
    bool removeToken(pntype token);
};

#endif // PNPLACE_H
