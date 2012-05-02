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

class pnPrimitive {
public:
    editDialog * editor;
    QGraphicsTextItem * label;
    QGraphicsTextItem * funcLabel;
    QGraphicsScene * canvas;
protected:
    QPointF labelPos;
    //QPointF funcPos;
public slots:
    void editorDeleter();
};

class pnItem: public QGraphicsItem, public pnPrimitive
{
private:
    //QGraphicsLineItem * separator;
public:
    QRectF boundingRect() const;
    bool isRect;
    void setPosition(int x, int y);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

class pnCircle: public pnItem{
public:
    PNPlace * simPlace;
    pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace = NULL);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};
class pnRect: public pnItem{    
public:
    PNTrans * simTrans;
    pnRect(QGraphicsScene * _canvas, PNTrans * _simTrans = NULL);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
};

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
