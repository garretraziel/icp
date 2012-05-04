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
std::vector<pnLine*> lineVect; //FIXME!
bool erase;


pnCircle::pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);
    //jojo, moc pekne, indexace do "neznama"
    QGraphicsView * thisView = canvas->views()[0];
    QPointF center = thisView->mapToScene(thisView->viewport()->rect().center());
    this->setPos(center.x(),center.y());
    label = canvas->addText("?? P");
    labelPos = QPointF(-15, -10);
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    funcLabel = NULL;
    //editor = new editDialog; //toto se asi neuklizi
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

    funcLabel = canvas->addText("a = x + y + z");
    funcLabel->setPos(this->x()+labelPos.x(),this->y()+labelPos.y() +15);
    //toto by slo mozna lip, takto bude mit kazda bunka svuj editor...
    //editor = new editDialog;
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
        // toto je ovsem ohavne, ale mazat pri iterovani pres vektor se mi nepodarilo
        std::vector<pnLine *> tmp;
        foreach(pnLine * l, lineVect){
            if(l->start==this || l->end == this){
                if(l->start->primType == TRANS){
                    //smazat z prechodu toto out name
                    //->removeOutName();
                    ((pnRect *)l->start)->simTrans->removeConnectedPlace(((pnCircle *)l->end)->simPlace, false);
                } else {
                    //smazat z prechodu toto in name
                    //->removeInName();
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


        delete editor;
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
    this->setPos(event->scenePos().x(),event->scenePos().y());
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    if(primType == PLACE){
        ((pnCircle *)this)->simPlace->x = QString::number(this->x());
        ((pnCircle *)this)->simPlace->y = QString::number(this->y());
    } else
    if(primType == TRANS){
        ((pnRect *)this)->simTrans->x = QString::number(this->x());
        ((pnRect *)this)->simTrans->y = QString::number(this->y());
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

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawEllipse(-20, -20, 40, 40);
}

QRectF pnRect::boundingRect() const{
    //TODO
    int textLen = label->toPlainText().length()*6;
    int funcLen = funcLabel->toPlainText().length()*6;
    return QRectF(-65, -25, 65 + ((textLen > funcLen)? textLen : funcLen), 50);
}

void pnRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 1));
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
};


pnLine::pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas){
   start = _start;
   end = _end;
   canvas = _canvas;

   line = canvas->addLine(start->x(),start->y(),end->x(),end->y(),QPen(Qt::black, 1));
   line->setZValue(-1);
   lineVect.push_back(this);

   label = new dClickLabel(this);
   label->setPlainText("?? L");
   label->setPos((start->x()+end->x())/2,(start->y()+end->y())/2);
   label->setAcceptedMouseButtons(Qt::LeftButton);   
   canvas->addItem(label);
   primType = EDGE;
   //editor = new editDialog;
   editor = mw->getEditor();
   funcLabel = NULL;
}

pnLine::~pnLine(){
    delete editor;
    delete line;
    delete label;
}

void pnLine::update(){
    line->setLine(start->x(),start->y(),end->x(),end->y());
    label->setPos((start->x()+end->x())/2,(start->y()+end->y())/2);    
}
