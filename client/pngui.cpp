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

#include <QDebug>

pnItem * startpos;               ///< ukazatel na pocatecni misto/prechod, ze ktereho je vedena hrana
bool line;                       ///< globalni prepinac, urcuje zda se prave spojuji prvky ve scene
std::vector<pnLine*> lineVect;   ///< vektor hran, uzit pro iterovani a opraveni pozice metodou update()
bool erase;                      ///< globalni prepinas, urcuje zda se prave mazou prvky ve scene

#define PI 3.1415927537

/**
  * Konstruktor vytvarejici misto ve scene
  * @param _canvas ukazatel na scena, kam se vykresluje
  * @param _simPlace ukazatel na misto do SimState
  */
pnCircle::pnCircle(QGraphicsScene * _canvas, PNPlace * _simPlace){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    //nastavi si canvas
    canvas = _canvas;
    canvas->addItem(this);
    //a napozicuje doprostred sceny
    this->setPos(mw->currentTabViewCenter());

    label = canvas->addText("?? P");
    labelPos = QPointF(-15, -10);
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    //mame jen tokeny, zadnou funcki
    funcLabel = NULL;
    //ziska spolecny editor
    editor = mw->getEditor();
    simPlace = _simPlace;
    primType = PLACE;
}

/**
  * Konstruktor vytvarejici prechod ve scene
  * @param _canvas ukazatel na scena, kam se vykresluje
  * @param _simTrans ukazatel na prechod do SimState
  */
pnRect::pnRect(QGraphicsScene * _canvas, PNTrans * _simTrans){
    //podobne jako pnCircle
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);
    this->setPos(mw->currentTabViewCenter());
    label = canvas->addText("?? T");
    labelPos = QPointF(-60,-20);
    label->setPos(this->x()+labelPos.x(),this->y()+labelPos.y());
    //funkci ale mame, tak ji nastavime
    funcLabel = canvas->addText("output func = ??");
    funcLabel->setPos(this->x()+labelPos.x(),this->y()+labelPos.y() +15);
    //ziska editor
    editor = mw->getEditor();
    simTrans = _simTrans;
    primType = TRANS;
}

/**
  * Nastaveni pozice na x y
  * @param x X pozice
  * @param y Y pozice
  */
void pnItem::setPosition(int x, int y){
    this->setPos(x,y);
    label->setPos(x+labelPos.x(),y+labelPos.y());
    if(funcLabel) funcLabel->setPos(x+labelPos.x(),y+labelPos.y()+15);
}

/**
  * Obslouzeni kliknuti na prvek
  * @param event udalost spojena s mysi
  * @return
  */
void pnItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
 {
    if(erase){
        //simulace neni aktualni
        mw->getCurrentSim()->isAct = false;
        mw->actAct(mw->getCurrentIndex());
        std::vector<pnLine *> tmp;
        //zkontroluje pripojene hrany a smaze je
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
        //smaze popisky
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
        return;
    }

    //klik, ale nic nezmeneni, simulaci zustava jeji aktualnost
    if(event->button()==Qt::LeftButton){
        setCursor(Qt::ClosedHandCursor);
        line = false;
    }


    else{
        //simulace neni aktualni
        mw->getCurrentSim()->isAct = false;
        mw->actAct(mw->getCurrentIndex());

        //propojuje, pokud nebylo kliknuto na ten jeden a samy, nebo na stejny typ
        if(line){
            line = false;
            if(startpos->primType== this->primType || startpos == this)
                return;
            foreach(pnLine * l, lineVect){
                if(l->start == startpos && l->end == this){
                    QMessageBox::critical(mw, "Double edge error", "There can't be two same edges");
                    return;
                }
            }

            pnLine * new_line = new pnLine(startpos,this,canvas);
            Q_UNUSED(new_line);

        }else{
            startpos = this;
            line = true;
        }

    }
 }

/**
  * Obslouzeni posunuti prvku
  * @param event udalost spojena s mysi
  */
void pnItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    // pri pohnuti sit neni aktualni
    // server by zaslal puvodni pozice prvku
    // a grafika by poskakovala
    mw->getCurrentSim()->isAct = false;
    mw->actAct(mw->getCurrentIndex());

    setPosition(event->scenePos().x(),event->scenePos().y());

    //nastavi pozice v simulaci
    if(primType == PLACE){
        ((pnCircle *)this)->simPlace->x = QString::number(int(this->x()));
        ((pnCircle *)this)->simPlace->y = QString::number(int(this->y()));
    } else
    if(primType == TRANS){
        ((pnRect *)this)->simTrans->x = QString::number(int(this->x()));
        ((pnRect *)this)->simTrans->y = QString::number(int(this->y()));
    }

    // a upravi hrany
    foreach(pnLine * l,lineVect){
        l->update();
    }
    setCursor(Qt::OpenHandCursor);
}

/**
  * Obslouzeni pusteni zmacknuteho tlacitka po kliknuti na prvek
  * @param event udalost spojena s mysi
  */
void pnItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
 {
     if(event->button()==Qt::LeftButton)
         setCursor(Qt::OpenHandCursor);
     else{
     }
 }

/**
  * Obslouzeni dvojkliku na prvek
  * @param event udalost spojena s mysi
  */
void pnItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    editor->loadData(this);
    editor->show();
}

/**
  * Ziskani ohranicujiciho obdelnika
  * @return ohranicujici obdelnik
  */
QRectF pnItem::boundingRect() const{
    return QRectF(-20.5, -20.5, 41, 41);
}

/**
  * Obslouzeni vykresleni mista do sceny
  * @param painter vykreslovac (vnitrne obsluhuje scena)
  * @param option vykreslovaci volba
  * @param widget uakaztel na vykreslovaci widget
  */
void pnCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawEllipse(-20, -20, 40, 40);
}


/**
  * Ziskani ohranicujiciho obdelnika
  * @return ohranicujici obdelnik
  */
QRectF pnRect::boundingRect() const{
    //TODO
    int textLen = label->toPlainText().length()*6;
    int funcLen = funcLabel->toPlainText().length()*6;
    return QRectF(-66, -26, 66 + ((textLen > funcLen)? textLen : funcLen), 51);
}

/**
  * Obslouzeni vykresleni prechodu do sceny
  * @param painter vykreslovac (vnitrne obsluhuje scena)
  * @param option vykreslovaci volba
  * @param widget uakaztel na vykreslovaci widget
  */
void pnRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(QColor(255,255,255)));
    int textLen = label->toPlainText().length()*6;
    int funcLen = funcLabel->toPlainText().length()*6;
    painter->drawRect(-65, -25, 65 + ((textLen > funcLen)? textLen : funcLen), 50);
}

/**
  *
  * Trida pro popisek reagujici na dvojklik
  *
  */
class dClickLabel: public QGraphicsTextItem {
private:
    pnLine * sender; ///< ukazatel na prvek, ktery tento label popisuje
public:

    /**
      * Konstruktor dClickLabel
      * @param _sender ukazatel na prvek, ktery tento label popisuje
      */
    dClickLabel(pnLine * _sender){
        sender = _sender;
    }
protected:

    /**
      * Obslouzeni dvojkliku na prvek
      * @param event udalost spojena s mysi
      */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
        Q_UNUSED(event);
        sender->editor->loadData(sender);
        sender->editor->show();
    }

    /**
      * Obslouzeni kliknuti na prvek
      * @param event udalost spojena s mysi
      * @return
      */
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

    /**
      * Obslouzeni vykresleni popisku do sceny
      * @param painter vykreslovac (vnitrne obsluhuje scena)
      * @param option vykreslovaci volba
      * @param widget uakaztel na vykreslovaci widget
      */
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

/**
  *
  * Trida reprezentujici sipku ve scene
  *
  */
class arrow: public QGraphicsLineItem {
    pnItem * colider; ///< ukazatel na koncovy prvek
public:
    /**
      * Nastavi ukazatel na koncovy prvek
      * @param _colider ukazatel na koncovy prvek
      */
    void setColider(pnItem * _colider){
        colider = _colider;
    }

    /**
      * Obslouzeni vykresleni sipky do sceny
      * @param painter vykreslovac (vnitrne obsluhuje scena)
      * @param option vykreslovaci volba
      * @param widget uakaztel na vykreslovaci widget
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
        Q_UNUSED(widget);
        Q_UNUSED(option);

        //pres vsechny stehny ohranicujicho obdelnika
        std::vector<QLineF> boundingLines;
#define bndRct colider->boundingRect()
#define bPos colider->pos()
        boundingLines.push_back(QLineF(bndRct.topLeft()+bPos, bndRct.topRight()+bPos));
        boundingLines.push_back(QLineF(bndRct.bottomLeft()+bPos, bndRct.bottomRight()+bPos));
        boundingLines.push_back(QLineF(bndRct.topLeft()+bPos, bndRct.bottomLeft()+bPos));
        boundingLines.push_back(QLineF(bndRct.topRight()+bPos, bndRct.bottomRight()+bPos));
#undef bndRct

        painter->setPen(QPen(Qt::black, 2));
        //zjistit se kterou se protina
        QPointF intersectPoint;
        foreach(QLineF boundingLine, boundingLines){
            QLineF::IntersectType intersectType =
                    this->line().intersect(boundingLine, &intersectPoint);
            if (intersectType == QLineF::BoundedIntersection) break;
        }
        //udelat caru az k bodu pruniku
        painter->drawLine(this->line().p1(),intersectPoint);

        double alfa = ::acos(this->line().dx()/this->line().length());
        if(this->line().dy() >= 0) alfa = (2*PI)-alfa;

        //a za pomoci jednotkove kruznice vykresli sipku
        painter->drawLine(QLineF(intersectPoint, intersectPoint+QPointF(sin(alfa-PI/3)*10,cos(alfa-PI/3)*10)));
        painter->drawLine(QLineF(intersectPoint, intersectPoint+QPointF(sin(alfa+PI+PI/3)*10,cos(alfa+PI+PI/3)*10)));

    }
};

/**
  * Konstruktor vytvarejici pnLine
  * @param _start pocatecni prvek
  * @param _end koncovy prvek
  * @param _canvas scena kam se kresli
  */
pnLine::pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas){
   start = _start;
   end = _end;
   canvas = _canvas;
   line = new arrow;

   //nastavi se koncovy bod pro detekci pruniku
   ((arrow *)line)->setColider(end);

   //nastavice pozice
   line->setLine(start->x(),start->y(),end->x(),end->y());
   canvas->addItem(line);

   //cary budou pod prvky
   line->setZValue(-1);
   if(start->primType == PLACE)
       line->setPen(QPen(Qt::green, 2));
   else
       line->setPen(QPen(Qt::red, 2));
   lineVect.push_back(this);

   //a popisek bude prijimat dvojkliky
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

/**
  * Destruktor pnLine
  */
pnLine::~pnLine(){

    delete line;
    delete label;
}

/**
  * Obslozeni prekresleni hrany po presunu mista nebo prechodu
  */
void pnLine::update(){
    line->setLine(start->x(),start->y(),end->x(),end->y());
    label->setPos((start->x()+end->x())/2,(start->y()+end->y())/2);    
}
