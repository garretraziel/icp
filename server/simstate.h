/**
 * @file simstate.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida popisujici stav simulace.
 */

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

/**
 * Trida, ktera popisuje aktualni stav simulace, uklada vektory hran a prechodu
 * a poskytuje metody pro praci s nimi.
 */
class SimState
{
public:
    PlaceVector places; ///< vektor mist simulace
    TransVector transits; ///< vektor prechodu simulace
    StringToPnplaceMap places_id; ///< slovnik id mist
    StringToPntransMap transits_id; ///< slovnik id prechodu

    QString author; ///< info o autorovi simulace
    QString name; ///< info o nazvu simulace
    int version; ///< info o verzi simulace
    QString info; ///< info o simulaci
    bool isAct; ///< zda je tato simulace aktivni

    /**
      * Konstruktor objektu, nastavi atributy.
      */
    SimState():author(""),name(""),version(1),info(""),isAct(false) {}
    /**
      * Destruktor objektu.
      */
    ~SimState();
    /**
      * Nastavi svuj stav podle vstupniho XML, ktere parsuje.
      *
      * @param xml vstupni XML se stavem simulace
      *
      * @return true pokud se podarilo nastavit stav, jinak false
      */
    bool setState(QString xml);
    /**
      * Vrati XML se stavem simulace.
      *
      * @return XML se stavem simulace
      */
    QString getState();
    /**
      * Vrati vektor prechodu simulace.
      *
      * @return vektor prechodu
      */
    TransVector getTransits() {return transits;}

    /**
      * Zkontroluje, zda je nactena konfigurace validni.
      *
      * @return true pokud je konfigurace validni, jinak false
      */
    bool checkConfiguration();

    /**
      * Smaze zadane misto ze simulace.
      *
      * @param _place ukazatel na misto
      */
    void removePlace(PNPlace * _place);
    /**
      * Smaze zadany prechod ze simulace.
      *
      * @param _trans ukazatel na prechod
      */
    void removeTrans(PNTrans * _trans);
    /**
      * Nastavi informace o prechodu: nazev, autora, verzi, info.
      *
      * @param _name nazev simulace
      * @param _author jmeno autora
      * @param _version verze simulace
      * @param _info dodatecne informace k simulaci
      */
    void setProperies(QString _name, QString _author, QString _version, QString _info);
};

#endif // SIMSTATE_H
