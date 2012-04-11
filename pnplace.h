#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>

typedef int pntype;
typedef std::vector<pntype> TokenVector;

class PNPlace
{
private:
    TokenVector tokens;
public:
    PNPlace();
};

#endif // PNPLACE_H
