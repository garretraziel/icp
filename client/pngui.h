/**
 * @file pngui.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Tridy pro graficke prvky ve scene.
 *
 */

#ifndef PNGUI_H
#define PNGUI_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QtGui>
#include <vector>
#include <QPointF>
#include "editdialog.h"
#include "../server/pnplace.h"
#include "../server/pntrans.h"

#define PLACE 0
#define TRANS 1
#define EDGE 3

/**
 *
 * Zakladni trida grafickych prvku
 *
 */
class pnPrimitive {
public:
    editDialog * editor;            ///< ukazatel na edit dialog
    QGraphicsTextItem * label;      ///< popisek (tokeny, promenne, nebo straz)
    QGraphicsTextItem * funcLabel;  ///< popisek (vytupni funkce)
    QGraphicsScene * canvas;        ///< scena, na ktere je prvek
    int primType;                   ///< typ prvku
protected:
    QPointF labelPos;               ///< pozice popisku
};

/**
 *
 * Trida odvozena od grafickych prvku Qt a zakladni tridy
 *
 */
class pnItem: public QGraphicsItem, public pnPrimitive
{
private:
    //QGraphicsLineItem * separator;
public:
    /**
      * Ziskani ohranicujiciho obdelnika
      * @return ohranicujici obdelnik
      */
    QRectF boundingRect() const;

    /**
      * Nastaveni pozice na x y
      * @param x X pozice
      * @param y Y pozice
      */
    void setPosition(int x, int y);
protected:
    /**
      * Obslouzeni kliknuti na prvek
      * @param event udalost spojena s mysi
      * @return
      */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /**
      * Obslouzeni posunuti prvku
      * @param event udalost spojena s mysi
      */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
      * Obslouzeni pusteni zmacknuteho tlacitka po kliknuti na prvek
      * @param event udalost spojena s mysi
      */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /**
      * Obslouzeni dvojkliku na prvek
      * @param event udalost spojena s mysi
      */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

/**
 *
 * Trida zastitujici vykresleni a praci s "misty"
 *
 */
class pnCircle: public pnItem{
public:
    PNPlace * simPlace; ///< ukazatel na misto do SimState

    /**
      * Konstruktor vytvarejici misto ve scene
      * @param _canvas ukazatel na scena, kam se vykresluje
      * @param _simPlace ukazatel na misto do SimState
      */
    pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace = NULL);

    /**
      * Obslozeni vykresleni mista do sceny
      * @param painter vykreslovac (vnitrne obsluhuje scena)
      * @param option vykreslovaci volba
      * @param widget uakaztel na vykreslovaci widget
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

/**
 *
 * Trida zastitujici vykresleni a praci s "prechody"
 *
 */
class pnRect: public pnItem{    
public:
    PNTrans * simTrans; ///< ukazatel na prechod do SimState

    /**
      * Konstruktor vytvarejici prechod ve scene
      * @param _canvas ukazatel na scena, kam se vykresluje
      * @param _simTrans ukazatel na prechod do SimState
      */
    pnRect(QGraphicsScene * _canvas, PNTrans * _simTrans = NULL);

    /**
      * Obslouzeni vykresleni prechodu do sceny
      * @param painter vykreslovac (vnitrne obsluhuje scena)
      * @param option vykreslovaci volba
      * @param widget uakaztel na vykreslovaci widget
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
      * Ziskani ohranicujiciho obdelnika
      * @return ohranicujici obdelnik
      */
    QRectF boundingRect() const;
};

/**
 *
 * Trida zastitujici vykresleni a praci se spojovacimi hranami
 *
 */
class pnLine: public pnPrimitive{
private:
    QGraphicsLineItem * line;   ///< hrana spojici misto a prechod
public:
    /**
      * Konstruktor vytvarejici pnLine
      * @param _start pocatecni prvek
      * @param _end koncovy prvek
      * @param _canvas scena kam se kresli
      */
    pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas);

    /**
      * Destruktor pnLine
      */
    ~pnLine();

    /**
      * Obslozeni prekresleni hrany po presunu mista nebo prechodu
      */
    void update();

    pnItem * start; ///< ukazatel na pocatecni misto/prechod
    pnItem * end;   ///< ukazatel na pocatecni prechod/misto
};


extern pnItem * startpos;               ///< ukazatel na pocatecni misto/prechod, ze ktereho je vedena hrana
extern bool line;                       ///< globalni prepinac, urcuje zda se prave spojuji prvky ve scene
extern std::vector<pnLine*> lineVect;   ///< vektor hran, uzit pro iterovani a opraveni pozice metodou update()
extern bool erase;                      ///< globalni prepinas, urcuje zda se prave mazou prvky ve scene
#endif // PNGUI_H
