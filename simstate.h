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
private:
    PlaceVector places;
    TransVector transits;
    StringToPnplaceMap places_id;
    StringToPntransMap transits_id;

public:
    SimState();
    ~SimState();
    bool setState(QString xml);
    TransVector getTransits() {return transits;}
    //todo: a taky zde bude funkce vracejici XML, neco jako getStaticState a getDynamicState
};

#endif // SIMSTATE_H
