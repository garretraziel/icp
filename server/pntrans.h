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
    Operation(){}
    Operation(Operations _op, QString _var){
        op = _op;
        var = _var;
    }
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
    OneOut(){}
    OneOut(OperationVector _operations, QString _output){
        operations = _operations;
        output = _output;
    }
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
    int possibleChoicesCount();


    //TODO, oddelat, predelat
    void removeOutName(PNPlace * place){
        StringToPnplaceMap::iterator it;
        for(it=out_names.begin(); it!=out_names.end(); it++){
            if((*it).second == place){
                out_names.erase(it);
                return;
            }
        }
    }
    void removeInName(PNPlace * place){
        StringToPnplaceMap::iterator it;
        for(it=in_names.begin(); it!=in_names.end(); it++){
            if((*it).second == place){
                in_names.erase(it);
                return;
            }
        }
    }
};

#endif // PNTRANS_H
