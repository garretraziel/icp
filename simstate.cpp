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
            one_token = one_token.nextSiblingElement("token");
        }
        int x = one_place.attribute("posx").toInt();
        int y = one_place.attribute("posy").toInt();
        PNPlace *place = new PNPlace(x,y,tokens);
        places.push_back(place);
        placemap[one_place.attribute("id")] = place;
        one_place = one_place.nextSiblingElement("place");
    }

    QDomElement xml_trans = root.firstChildElement("transitions");
    QDomElement one_trans = xml_trans.firstChildElement("transition");

    while (!one_trans.isNull()) {
        PlaceVector ins;
        PlaceVector outs;
        PnplaceToStringMap in_names;
        PnplaceToStringMap out_names;

        QDomElement one_element = one_trans.firstChildElement("inplace");
        while (!one_element.isNull()) {
            ins.push_back(placemap[one_element.text()]);
            in_names[placemap[one_element.text()]] = one_element.attribute("name");
            one_element = one_element.nextSiblingElement("inplace");
        }

        one_element = one_trans.firstChildElement("outplace");
        while (!one_element.isNull()) {
            outs.push_back(placemap[one_element.text()]);
            out_names[placemap[one_element.text()]] = one_element.attribute("name");
            one_element = one_element.nextSiblingElement("outplace");
        }

        //todo: jeste cteni podminek = vymyset zpusob, jak se bude ukladat
        int x = one_trans.attribute("posx").toInt();
        int y = one_trans.attribute("posy").toInt();

        ConstraintVector constraints;
        QDomElement one_cond = one_trans.firstChildElement("constraint");
        while (!one_cond.isNull()) {
            Constraint *cond = new Constraint(one_cond.text());
            constraints.push_back(cond);
            one_cond = one_cond.nextSiblingElement("constraint");
        }

        PNTrans *trans = new PNTrans(ins, outs, x, y, constraints, in_names, out_names);
        transits.push_back(trans);
        one_trans = one_trans.nextSiblingElement("transition");
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
