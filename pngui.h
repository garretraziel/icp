#ifndef PNGUI_H
#define PNGUI_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QtGui>
#include <vector>

#include "editdialog.h"

class pnPrimitive {
public:
    editDialog * editor;
    QGraphicsTextItem * label;
protected:
    QGraphicsScene * canvas;
public slots:
    void editorDeleter();
};

class pnItem: public QGraphicsItem, public pnPrimitive
{
public:
    QRectF boundingRect() const;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
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
