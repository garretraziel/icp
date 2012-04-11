#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>

enum Operators {
    OP_LESS,
    OP_LESSEQ,
    OP_MOREEQ,
    OP_MORE,
    OP_EQ,
    OP_NOTEQ
};

typedef int pntype;
typedef std::vector<pntype> TokenVector;

class PNPlace
{
private:
    TokenVector tokens;
public:
    PNPlace();
    bool hasTokens();
};

#endif // PNPLACE_H
