#include "simstate.h"
#include <map>
#include <QXmlStreamWriter>

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

    autor = root.attribute("autor");
    name = root.attribute("name");
    version = root.attribute("version").toInt();
    info = root.attribute("info");

    QDomElement xml_places = root.firstChildElement("places");
    QDomElement one_place = xml_places.firstChildElement("place");

    while (!one_place.isNull()) {
        TokenVector tokens;
        QDomElement one_token = one_place.firstChildElement("token");
        while (!one_token.isNull()) {
            pntype token = one_token.text().toInt();
            tokens.push_back(token);
            one_token = one_token.nextSiblingElement("token");
        }
        PNPlace *place = new PNPlace(one_place.attribute("posx"),one_place.attribute("posy")
                                     ,one_place.attribute("id"),tokens);
        places.push_back(place);
        places_id[one_place.attribute("id")] = place;
        one_place = one_place.nextSiblingElement("place");
    }

    QDomElement xml_trans = root.firstChildElement("transitions");
    QDomElement one_trans = xml_trans.firstChildElement("transition");

    while (!one_trans.isNull()) {
        StringToPnplaceMap in_names;
        StringToPnplaceMap out_names;

        QDomElement one_element = one_trans.firstChildElement("inplace");
        while (!one_element.isNull()) {
            in_names[one_element.attribute("name")] = places_id[one_element.text()];
            one_element = one_element.nextSiblingElement("inplace");
        }

        one_element = one_trans.firstChildElement("outplace");
        while (!one_element.isNull()) {
            out_names[one_element.attribute("name")] = places_id[one_element.text()];
            one_element = one_element.nextSiblingElement("outplace");
        }

        ConstraintVector constraints;
        QDomElement one_cond = one_trans.firstChildElement("constraint");
        while (!one_cond.isNull()) {
            Constraint *cond;
            if (one_cond.attribute("type") == "const") {
                cond = new Constraint(one_cond.attribute("var1"),one_cond.attribute("op").toInt(),one_cond.attribute("const").toInt());
            } else {
                cond = new Constraint(one_cond.attribute("var1"),one_cond.attribute("op").toInt(),one_cond.attribute("var2"));
            }
            constraints.push_back(cond);
            one_cond = one_cond.nextSiblingElement("constraint");
        }

        OutputOperations operations;
        QDomElement one_op = one_trans.firstChildElement("operation");
        while (!one_op.isNull()){
            OneOut oneout;
            oneout.output = one_op.attribute("output");
            QDomElement one_operation = one_op.firstChildElement();
            while(!one_operation.isNull()) {
                Operation op;
                if (one_operation.tagName() == "plus") {
                    op.op = ADD;
                } else {
                    op.op = SUB;
                }
                op.var = one_operation.attribute("id");
                oneout.operations.push_back(op);
                one_operation = one_operation.nextSiblingElement();
            }
            operations.push_back(oneout);
            one_op = one_op.nextSiblingElement("operation");
        }

        PNTrans *trans = new PNTrans(one_trans.attribute("posx"), one_trans.attribute("posy"),
                                     one_trans.attribute("id"), constraints, in_names, out_names, operations);
        transits_id[one_trans.attribute("id")] = trans;
        transits.push_back(trans);
        one_trans = one_trans.nextSiblingElement("transition");
    }

    return true;
}

SimState::~SimState()
{
    PlaceVector::iterator pit;
    for (pit = places.begin(); pit < places.end(); pit++) {
        PNPlace *place = (*pit);
        delete place;
    }
    places.clear();
    TransVector::iterator tit;
    for (tit = transits.begin(); tit < transits.end(); tit++) {
        PNTrans *trans = (*tit);
        delete trans;
    }
    transits.clear();
    places_id.clear();
    transits_id.clear();
}

QString SimState::getState()
{
    QString result;
    QXmlStreamWriter doc(&result);
    doc.setAutoFormatting(true);
    doc.writeStartDocument();

    doc.writeStartElement("petrinet");

    doc.writeAttribute("autor",autor);
    doc.writeAttribute("name",name);
    doc.writeAttribute("version",QString::number(version));
    doc.writeAttribute("info",info);

    doc.writeStartElement("places");

    PlaceVector::iterator pit;

    for (pit = places.begin(); pit != places.end(); pit++) {
        doc.writeStartElement("place");

        PNPlace *place = (*pit);
        doc.writeAttribute("id",place->id);
        doc.writeAttribute("posx",place->x);
        doc.writeAttribute("posy",place->y);

        TokenVector::iterator tit;
        TokenVector tokens = place->getTokens();

        for (tit = tokens.begin(); tit < tokens.end(); tit++) {
            doc.writeTextElement("token",QString::number(*tit));
        }

        doc.writeEndElement();
    }

    doc.writeEndElement();

    doc.writeStartElement("transitions");

    TransVector::iterator trit;

    for (trit = transits.begin(); trit != transits.end(); trit++) {
        doc.writeStartElement("transition");

        PNTrans *transit = (*trit);
        doc.writeAttribute("id",transit->id);
        doc.writeAttribute("posx",transit->x);
        doc.writeAttribute("posy",transit->y);

        ConstraintVector::iterator cvit;
        for (cvit = transit->constraints.begin(); cvit != transit->constraints.end(); cvit++) {
            Constraint *constraint = (*cvit);

            doc.writeEmptyElement("constraint");

            if (constraint -> type == TYPENONE) {
                doc.writeAttribute("type","none");
            } else if (constraint -> type == TYPEANYTHING) {
                doc.writeAttribute("type","anything"); //todo: toto implementovat i v nacitani
            } else {
                doc.writeAttribute("var1",constraint->first);
                doc.writeAttribute("op",QString::number(int(constraint->op)));
                if (constraint -> type == TYPEVAR) {
                    doc.writeAttribute("type","var");
                    doc.writeAttribute("var2",constraint->second_var);
                } else if (constraint -> type == TYPECONST) {
                    doc.writeAttribute("type","const");
                    doc.writeAttribute("const",QString::number(constraint->second_const));
                }
            }

        }

        StringToPnplaceMap::iterator placeit;

        for (placeit = transit->in_names.begin(); placeit != transit->in_names.end(); placeit++) {
            doc.writeStartElement("inplace");
            PNPlace *place = (*placeit).second;
            doc.writeAttribute("name",(*placeit).first);
            doc.writeCharacters(place->id);
            doc.writeEndElement();
        }

        for (placeit = transit->out_names.begin(); placeit != transit->out_names.end(); placeit++) {
            doc.writeStartElement("outplace");
            doc.writeAttribute("name",(*placeit).first);
            PNPlace *place = (*placeit).second;
            doc.writeCharacters(place->id);
            doc.writeEndElement();
        }

        OutputOperations::iterator oit;

        for (oit = transit->operations.begin(); oit != transit->operations.end(); oit++) {
            doc.writeStartElement("operation");
            doc.writeAttribute("output",(*oit).output);
            OperationVector::iterator opit;

            for (opit = (*oit).operations.begin(); opit != (*oit).operations.end(); opit++) {
                Operation operation = (*opit);
                if (operation.op == ADD) doc.writeEmptyElement("plus");
                else if (operation.op == SUB) doc.writeEmptyElement("minus");
                else {
                    qCritical() << "Error: unknown operation during saving XML";
                }
                doc.writeAttribute("id",operation.var);
            }

            doc.writeEndElement();
        }

        doc.writeEndElement();
    }

    doc.writeEndElement();

    doc.writeEndElement();

    doc.writeEndDocument();

    return result;
}


bool SimState::checkConfiguration(){
    foreach(PNTrans * trans, transits){
        foreach(Constraint * constraint, trans->constraints){

            if((trans->in_names.find(constraint->first) == trans->in_names.end()) ||
               (constraint->type == TYPEVAR && trans->in_names.find(constraint->second_var) == trans->in_names.end()))
                /*
                  hurr durr
                  */
                    return false;
        }

        foreach(OneOut operation, trans->operations){
            if(trans->out_names.find(operation.output) == trans->out_names.end())
                return false;

            foreach(Operation op, operation.operations){
                //TODOOO ted to funguje jen pro promenny!!
                if(trans->in_names.find(op.var) == trans->in_names.end())
                    return false;
            }

        }
    }

    return true; //nigga wrote da shit well!
}

void SimState::removePlace(PNPlace * _place){
    PlaceVector::iterator it;
    for(it=places.begin(); it!=places.end(); it++){
        if((*it)==_place){
            places.erase(it);
            return;
        }
    }
}
void SimState::removeTrans(PNTrans * _trans){
    TransVector::iterator it;
    for(it=transits.begin(); it!=transits.end(); it++){
        if((*it)==_trans){
            transits.erase(it);
            return;
        }
    }
}

void SimState::setProperies(QString _name, QString _author, QString _version, QString _info){
    name = _name;
    autor = _author;
    version = _version.toInt();
    info = _info;
}
