#ifndef SIMSTATE_H
#define SIMSTATE_H

#include <vector>
#include <map>
#include <QtXml>
#include <QDomDocument>
#include "pnplace.h"
#include "pntrans.h"

typedef std::vector<PNTrans*> TransVector;
typedef std::map<QString,PNTrans*> StringToPntransMap;

// Pro uchovani aktualniho stavu simulace
class SimState
{
//private:
public:
    PlaceVector places;
    TransVector transits;
    StringToPnplaceMap places_id;
    StringToPntransMap transits_id;

    SimState();
    ~SimState();
    bool setState(QString xml);
    QString getState();
    TransVector getTransits() {return transits;}

    bool checkConfiguration();
    //todo: a taky zde bude funkce vracejici XML, neco jako getStaticState a getDynamicState

    //toto pujde jinam, jen ted narychlo...
    //a mozna pujdou sjednotit do jedne
    void removePlace(PNPlace * _place){
        PlaceVector::iterator it;
        for(it=places.begin(); it!=places.end(); it++){
            if((*it)==_place){
                places.erase(it);
                return;
            }
        }
    }
    void removeTrans(PNTrans * _trans){
        TransVector::iterator it;
        for(it=transits.begin(); it!=transits.end(); it++){
            if((*it)==_trans){
                transits.erase(it);
                return;
            }
        }
    }
};

#endif // SIMSTATE_H
