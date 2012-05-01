#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include <QString>
#include <map>
#include "pnplace.h"
#include "constraint.h"

enum Operations {
    ADD,
    SUB
};

struct Operation {
    Operations op;
    QString var;
};

typedef std::vector<PNPlace*> PlaceVector;
typedef std::vector<Constraint*> ConstraintVector;
typedef std::map<QString,PNPlace*> StringToPnplaceMap;
typedef std::map<PNPlace*,QString> PnplaceToStringMap;
typedef std::vector<StringToPntypeMap> StrPntMapVector;
typedef std::vector<struct Operation> OperationVector;

struct OneOut {
    OperationVector operations;
    QString output;
};

typedef std::vector<struct OneOut> OutputOperations;

class PNTrans
{
private:
    StrPntMapVector possible_choices;

    StrPntMapVector chooseValues(StringToTokensMap hash);
public:
    ConstraintVector constraints;
    StringToPnplaceMap in_names;
    StringToPnplaceMap out_names;
    OutputOperations operations;
    QString id;
    QString x;
    QString y;

    PNTrans();
    PNTrans(QString x, QString y, QString id, ConstraintVector constraints,
            StringToPnplaceMap in_names, StringToPnplaceMap out_names, OutputOperations operations)
        :constraints(constraints),in_names(in_names),out_names(out_names),operations(operations),id(id),x(x),y(y) {}
    ~PNTrans();
    bool fire();
    bool doOperations(unsigned int choice);
};

#endif // PNTRANS_H
