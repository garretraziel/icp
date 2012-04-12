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
            QString string_val =  one_token.text();
            pntype token = string_val.toInt();
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
