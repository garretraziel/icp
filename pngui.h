#ifndef PNGUI_H
#define PNGUI_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QtGui>
#include <vector>
#include "editdialog.h"

class pnItem: public QGraphicsItem
{
public:
    QRectF boundingRect() const;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    //asi mozna lepsi pres globalno promennou, uvidime
    QGraphicsScene * canvas;
    //toto bude chtit jeste nejak rozumne poresit, zatim pro efekt okay...
    editDialog * editor;
};

class pnCircle: public pnItem{
public:
    pnCircle(QGraphicsScene * _canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};
class pnRect: public pnItem{
public:
    pnRect(QGraphicsScene * _canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class pnLine{
private:
    pnItem * start;
    pnItem * end;
    QGraphicsScene * canvas;
    QGraphicsLineItem * line;
    QGraphicsTextItem * label;
public:
    pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas);
    ~pnLine();
    void update();
    //to by slo snad lip
    editDialog * editor;
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};


extern pnItem * startpos;
extern bool line;
extern std::vector<pnLine*> lineVect;
#endif // PNGUI_H
