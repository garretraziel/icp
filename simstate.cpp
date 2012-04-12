#include "simstate.h"
#include <map>

typedef std::map<QString,PNPlace*> PlaceMap;

SimState::SimState()
{
}

bool SimState::setState(QString xml)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument document;
    if (!document.setContent(xml,&errorStr,&errorLine,&errorColumn)) {
        qCritical() << "Error during parsing xml on line: " << errorLine << ", column: " << errorColumn;
        return false;
    }

    QDomElement root = document.documentElement();

    if (root.tagName() != "petrinet") {
        qCritical() << "Error during parsing xml, not valid";
        return false;
    }

    QDomElement xml_places = root.firstChildElement("places");
    QDomElement one_place = xml_places.firstChildElement("place");

    PlaceMap placemap;

    while (!one_place.isNull()) {
        TokenVector tokens;
        QDomElement one_token = one_place.firstChildElement("token");
        while (!one_token.isNull()) {
            pntype token = one_token.text().toInt();
            tokens.push_back(token);
            one_token = one_place.firstChildElement("token");
        }
        int x = one_place.attribute("x").toInt();
        int y = one_place.attribute("y").toInt();
        PNPlace *place = new PNPlace(x,y,tokens);
        places.push_back(place);
        placemap[one_place.attribute("id")] = place;
        one_place = xml_places.firstChildElement("place");
    }

    QDomElement xml_trans = root.firstChildElement("transitions");
    QDomElement one_trans = xml_trans.firstChildElement("transition");

    while (!one_trans.isNull()) {
        PlaceVector ins;
        PlaceVector outs;
        QDomElement one_element = one_trans.firstChildElement("inplace");
        while (!one_element.isNull()) {
            ins.push_back(placemap[one_element.text()]);
            one_element = one_trans.firstChildElement("inplace");
        }
        one_element = one_trans.firstChildElement("outplace");
        while (!one_element.isNull()) {
            outs.push_back(placemap[one_element.text()]);
            one_element = one_trans.firstChildElement("outplace");
        }
        //todo: jeste cteni podminek = vymyset zpusob, jak se bude ukladat
        int x = one_trans.attribute("x").toInt();
        int y = one_trans.attribute("y").toInt();
        PNTrans *trans = new PNTrans(ins, outs, x, y);
        transits.push_back(trans);
        one_trans = xml_trans.firstChildElement("transition");
    }

    return true;
}

SimState::~SimState()
{
    PlaceVector::iterator pit;
    for (pit = places.begin(); pit < places.end(); pit++) {
        delete *pit;
    }
    TransVector::iterator tit;
    for (tit = transits.begin(); tit < transits.end(); tit++) {
        delete *tit;
    }
}
