/**
 * @file pngui.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Tridy pro graficke prvky ve scene.
 *
 */

#include "pngui.h"
#include <math.h>
#include <mainwindow.h>
#include <QKeyEvent>
#include <iostream>
#include <QLineF>
#include <QPointF>
#include <typeinfo>
#include <QString>

pnItem * startpos;
bool line;
std::vector<pnLine*> lineVect;
bool erase;

#define PI 3.1415927537

pnCircle::pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);

    QGraphicsView * thisView = canvas->views()[0];
    QPointF center = thisView->mapToScene(thisView->viewport()->rect().center());
    this->setPos(center.x(),center.y());
    label = canvas->addText("?? P");
    labelPos = QPointF(-15, -10);
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    funcLabel = NULL;

    editor = mw->getEditor();
    simPlace = _simPlace;
    primType = PLACE;
}

pnRect::pnRect(QGraphicsScene * _canvas, PNTrans * _simTrans){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);
    QGraphicsView * thisView = canvas->views()[0];
    QPointF center = thisView->mapToScene(thisView->viewport()->rect().center());
    this->setPos(center.x(),center.y());
    label = canvas->addText("?? T");
    labelPos = QPointF(-60,-20);
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());

    funcLabel = canvas->addText("output func = ??");
    funcLabel->setPos(this->x()+labelPos.x(),this->y()+labelPos.y() +15);

    editor = mw->getEditor();
    simTrans = _simTrans;
    primType = TRANS;
}

void pnItem::setPosition(int x, int y){
    this->setPos(x,y);
    label->setPos(x+labelPos.x(),y+labelPos.y());
    if(funcLabel) funcLabel->setPos(x+labelPos.x(),y+labelPos.y()+15);
}

void pnItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
 {
    if(erase){
        mw->getCurrentSim()->isAct = false;
        mw->actAct(mw->getCurrentIndex());
        std::vector<pnLine *> tmp;
        foreach(pnLine * l, lineVect){
            if(l->start==this || l->end == this){
                if(l->start->primType == TRANS){
                    ((pnRect *)l->start)->simTrans->removeConnectedPlace(((pnCircle *)l->end)->simPlace, false);
                } else {
                    ((pnRect *)l->end)->simTrans->removeConnectedPlace(((pnCircle *)l->start)->simPlace, true);
                }
                delete l;
            }
            else
                tmp.push_back(l);
        }
        lineVect.clear();
        foreach(pnLine * l, tmp)
            lineVect.push_back(l);

        delete label;
        if(funcLabel) delete funcLabel;

        switch(primType){
            case PLACE:
                //smazat z vektoru mist v simstate
                mw->getCurrentSim()->removePlace(((pnCircle *)this)->simPlace);
            break;
            case TRANS:
                //smazat z vektoru prechodu v simstate
                mw->getCurrentSim()->removeTrans(((pnRect *)this)->simTrans);
            break;
            case EDGE:
                //tady se nikdy nedostanu
                //nicmene:
                QMessageBox::critical(NULL, "Unhandled fluidum exception", "Something is rotten in the state of Denmark!");
            break;
        }

        delete this;
        return; //!!! lol suicide
    }

    if(event->button()==Qt::LeftButton){
        setCursor(Qt::ClosedHandCursor);
        line = false;
    }
    else{
        mw->getCurrentSim()->isAct = false;
        mw->actAct(mw->getCurrentIndex());
        if(line){
            line = false;
            if(startpos->primType== this->primType || startpos == this)
                return;

            pnLine * new_line = new pnLine(startpos,this,canvas);
            Q_UNUSED(new_line);
        }else{
            startpos = this;
            line = true;
        }

    }
 }

void pnItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    mw->getCurrentSim()->isAct = false;
    mw->actAct(mw->getCurrentIndex());
    this->setPos(event->scenePos().x(),event->scenePos().y());
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    if(primType == PLACE){
        ((pnCircle *)this)->simPlace->x = QString::number(int(this->x()));
        ((pnCircle *)this)->simPlace->y = QString::number(int(this->y()));
    } else
    if(primType == TRANS){
        ((pnRect *)this)->simTrans->x = QString::number(int(this->x()));
        ((pnRect *)this)->simTrans->y = QString::number(int(this->y()));
    }
    if(funcLabel)
        funcLabel->setPos(this->x()+labelPos.x(),this->y()+labelPos.y() +15);
    foreach(pnLine * l,lineVect){
        l->update();
    }
    setCursor(Qt::OpenHandCursor);
}


void pnItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
 {
     if(event->button()==Qt::LeftButton)
         setCursor(Qt::OpenHandCursor);
     else{
     }
 }

void pnItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    editor->loadData(this);
    editor->show();
}

QRectF pnItem::boundingRect() const{
    return QRectF(-20.5, -20.5, 41, 41);
}

void pnCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawEllipse(-20, -20, 40, 40);
}

QRectF pnRect::boundingRect() const{
    //TODO
    int textLen = label->toPlainText().length()*6;
    int funcLen = funcLabel->toPlainText().length()*6;
    return QRectF(-66, -26, 66 + ((textLen > funcLen)? textLen : funcLen), 51);
}

void pnRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(QColor(255,255,255)));
    int textLen = label->toPlainText().length()*6;
    int funcLen = funcLabel->toPlainText().length()*6;
    painter->drawRect(-65, -25, 65 + ((textLen > funcLen)? textLen : funcLen), 50);
}

class dClickLabel: public QGraphicsTextItem {
private:
    pnLine * sender;
public:
    dClickLabel(pnLine * _sender){
        sender = _sender;
    }
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
        Q_UNUSED(event);
        sender->editor->loadData(sender);
        sender->editor->show();
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event){
        Q_UNUSED(event);
        if(erase){
            for(std::vector<pnLine *>::iterator it = lineVect.begin(); it!=lineVect.end(); ++it){
                if((*it)==sender){
                    lineVect.erase(it);
                    delete (*it);
                    break;
                }
            }
        }
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
        Q_UNUSED(widget);
        Q_UNUSED(option);
        painter->setBrush(QBrush(QColor(128,128,128)));
        painter->setPen(QPen(QColor(128,128,128), 1));
        painter->drawRoundedRect(QRectF(this->boundingRect().left()+2,this->boundingRect().top()+2,
                                        this->boundingRect().width(),this->boundingRect().height()),6,6);
        painter->setBrush(QBrush(QColor(255,255,255)));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawRoundedRect(this->boundingRect(),6,6);
        painter->drawText(this->boundingRect()," "+this->toPlainText());
    }
};

class arrow: public QGraphicsLineItem {
    pnItem * colider;
public:
    void setColider(pnItem * _colider){
        colider = _colider;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
        Q_UNUSED(widget);
        Q_UNUSED(option);

        std::vector<QLineF> boundingLines;
#define bndRct colider->boundingRect()
#define bPos colider->pos()
        boundingLines.push_back(QLineF(bndRct.topLeft()+bPos, bndRct.topRight()+bPos));
        boundingLines.push_back(QLineF(bndRct.bottomLeft()+bPos, bndRct.bottomRight()+bPos));
        boundingLines.push_back(QLineF(bndRct.topLeft()+bPos, bndRct.bottomLeft()+bPos));
        boundingLines.push_back(QLineF(bndRct.topRight()+bPos, bndRct.bottomRight()+bPos));
#undef bndRct

        painter->setPen(QPen(Qt::black, 2));

        QPointF intersectPoint;
        foreach(QLineF boundingLine, boundingLines){
            QLineF::IntersectType intersectType =
                    this->line().intersect(boundingLine, &intersectPoint);
            if (intersectType == QLineF::BoundedIntersection) break;
        }
        painter->drawLine(this->line().p1(),intersectPoint);

        double alfa = ::acos(this->line().dx()/this->line().length());
        if(this->line().dy() >= 0) alfa = (2*PI)-alfa;

        painter->drawLine(QLineF(intersectPoint, intersectPoint+QPointF(sin(alfa-PI/3)*10,cos(alfa-PI/3)*10)));
        painter->drawLine(QLineF(intersectPoint, intersectPoint+QPointF(sin(alfa+PI+PI/3)*10,cos(alfa+PI+PI/3)*10)));

    }
};

pnLine::pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas){
   start = _start;
   end = _end;
   canvas = _canvas;
   line = new arrow;
   //TODO
   ((arrow *)line)->setColider(end);
   line->setLine(start->x(),start->y(),end->x(),end->y());
   canvas->addItem(line);

   line->setZValue(-1);
   if(start->primType == PLACE)
       line->setPen(QPen(Qt::green, 2));
   else
       line->setPen(QPen(Qt::red, 2));
   lineVect.push_back(this);


   label = new dClickLabel(this);
   label->setPos((start->x()+end->x())/2,(start->y()+end->y())/2);
   label->setPlainText("?? L");
   label->setZValue(1000);
   label->setAcceptedMouseButtons(Qt::LeftButton);

   canvas->addItem(label);
   primType = EDGE;

   editor = mw->getEditor();
   funcLabel = NULL;
}

pnLine::~pnLine(){

    delete line;
    delete label;
}

void pnLine::update(){
    line->setLine(start->x(),start->y(),end->x(),end->y());
    label->setPos((start->x()+end->x())/2,(start->y()+end->y())/2);    
}
