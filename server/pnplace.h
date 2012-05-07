/**
 * @file pnplace.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Obsahuje tridu pro reprezentaci mista v petriho siti.
 */

#ifndef PNPLACE_H
#define PNPLACE_H

#include <vector>
#include <QString>
#include <QDebug>

typedef int pntype; ///< datovy typ simulace, dle zadani INT
typedef std::vector<pntype> TokenVector; ///< vektor hodnot v miste pnplace

/**
 * Trida pro reprezentaci mista v petriho siti. Uchovava si pouze informace o tokenech, ktere obsahuje.
 */
class PNPlace
{
private:
    TokenVector tokens; /// vektor tokenu typu pntype (in), ktere misto obsahuje

public:
    QString id; ///< id mista
    QString x; ///< x souradnice mista
    QString y; ///< y souradnice mista

    /**
      * Konstruktor tridy. Konstruuje objekt.
      */
    PNPlace();
    /**
      * Konstruktor tridy, ktery nastavi veskere atributy objektu.
      *
      * @param x souradnice x
      * @param y souradnice y
      * @param id id mista
      * @param tokens vektor hodnot, ktere ma misto obsahovat
      */
    PNPlace(QString x, QString y, QString id, TokenVector tokens): tokens(tokens),id(id),x(x),y(y) {}
    /**
      * Vrati, jestli misto obsahuje nejake tokeny.
      *
      * @return true pokud misto obsahuje tokeny, jinak false
      */
    bool hasTokens();
    /**
      * Vrati seznam tokenu.
      *
      * @return vektor tokenu, ktere obsahuje misto
      */
    TokenVector getTokens();
    /**
      * Vlozi token do mista.
      *
      * @param token token, ktery se ma vlozit
      */
    void putToken(pntype token);
    /**
      * Smaze token z mista.
      *
      * @param token token, ktery se ma smazat
      *
      * @return true pokud token mohl byt smazan, jinak false
      */
    bool removeToken(pntype token);
    /**
      * Smaze veskere tokeny mista.
      */
    void removeTokens();
};

#endif // PNPLACE_H
