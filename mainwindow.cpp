#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QtGui>
#include <QGraphicsScene>
#include <iostream>
#include <map>

QPointF startpos;
bool line;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);

    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),
                     this, SLOT(showConnectDialog()) );
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(addItem()));
    QObject::connect(ui->pushButtonRect, SIGNAL(clicked()),this,SLOT(addItemRect()));

    canvas = new QGraphicsScene(this);

    ui->view->setScene(canvas);
    //ui->view->setRenderHint(QPainter::Antialiasing);
    ui->view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete cd;
    delete canvas;
}

void MainWindow::showConnectDialog()
{
    cd->show();
}


//tyhle dve by snad mohly jit sjednotit do jedne
void MainWindow::addItem(){
    pnItem * item = new pnCircle(canvas);
    Q_UNUSED(item);
    //toto asi neni moc cisty, no
    //canvas->addItem(item);
}

void MainWindow::addItemRect(){
    pnItem * item = new pnRect(canvas);
    Q_UNUSED(item);
}


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
            canvas->addLine(startpos.x(),startpos.y(),
                            event->scenePos().x(),event->scenePos().y(),
                            QPen(Qt::black, 1));
        }else{
            startpos = event->scenePos();
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
