/**
 * @file aboutdialog.h
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
    editDialog * editor;
    QGraphicsTextItem * label;
    QGraphicsTextItem * funcLabel;
    QGraphicsScene * canvas;
    int primType;
protected:
    QPointF labelPos;
    //QPointF funcPos;
public slots:
    void editorDeleter();
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
    QRectF boundingRect() const;
    void setPosition(int x, int y);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

/**
 *
 * Trida zastitujici vykresleni a praci s "misty"
 *
 */
class pnCircle: public pnItem{
public:
    PNPlace * simPlace;
    pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace = NULL);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

/**
 *
 * Trida zastitujici vykresleni a praci s "prechody"
 *
 */
class pnRect: public pnItem{    
public:
    PNTrans * simTrans;
    pnRect(QGraphicsScene * _canvas, PNTrans * _simTrans = NULL);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
};

/**
 *
 * Trida zastitujici vykresleni a praci se spojovacimi hranami
 *
 */
class pnLine: public pnPrimitive{
private:
    QGraphicsLineItem * line;
public:
    pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas);
    ~pnLine();
    void update();
    //to by asi chtelo nejakyho pritele, nez public
    pnItem * start;
    pnItem * end;
};


extern pnItem * startpos;
extern bool line;
extern std::vector<pnLine*> lineVect;
extern bool erase;
#endif // PNGUI_H
