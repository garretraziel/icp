#include "pngui.h"


pnItem * startpos;
bool line;
std::vector<pnLine*> lineVect;

pnCircle::pnCircle(QGraphicsScene * _canvas){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);
}

pnRect::pnRect(QGraphicsScene * _canvas){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    canvas = _canvas;
    canvas->addItem(this);
}

void pnItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
 {
    if(event->button()==Qt::LeftButton){
        setCursor(Qt::ClosedHandCursor);
        line = false;
    }
    else{
        if(line){
            line = false;
            pnLine * new_line = new pnLine(startpos,this,canvas);
            lineVect.push_back(new_line);
        }else{
            startpos = this;
            line = true;
        }

    }
 }

void pnItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
             .length() < QApplication::startDragDistance()) {
             return;
    } //netusim k cemu to je, copypasta

    this->setPos(event->scenePos().x(),event->scenePos().y());
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

QRectF pnItem::boundingRect() const{
    return QRectF(-15.5, -15.5, 34, 34);
}

void pnCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawEllipse(-15, -15, 30, 30);
}

void pnRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawRect(-15,-5,30,10);
}

pnLine::pnLine(pnItem * _start, pnItem * _end, QGraphicsScene * _canvas){
   start = _start;
   end = _end;
   canvas = _canvas;
   line = canvas->addLine(start->x(),start->y(),end->x(),end->y(),QPen(Qt::black, 1));
   line->setZValue(-1);
   lineVect.push_back(this);
}

void pnLine::update(){
    line->setLine(start->x(),start->y(),end->x(),end->y());
}
