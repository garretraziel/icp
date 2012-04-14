#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QtGui>
#include <QGraphicsScene>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);

    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),
                     this, SLOT(showConnectDialog()) );
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(addItem()));

    canvas = new QGraphicsScene(this);

    ui->view->setScene(canvas);
    ui->view->setRenderHint(QPainter::Antialiasing);
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

void MainWindow::addItem(){
    pnItem * item = new pnItem();
    canvas->addItem(item);
}


pnItem::pnItem(){
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void pnItem::mousePressEvent(QGraphicsSceneMouseEvent *)
 {
    setCursor(Qt::ClosedHandCursor);
 }

void pnItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
             .length() < QApplication::startDragDistance()) {
             return;
    } //netusim k cemu to je, copypasta

    this->setPos(event->scenePos().x(),event->scenePos().y());
    setCursor(Qt::OpenHandCursor);
}

void pnItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
 {
     setCursor(Qt::OpenHandCursor);
 }

QRectF pnItem::boundingRect() const{
    return QRectF(-15.5, -15.5, 34, 34);
}

void pnItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(255,255,255)));
    painter->drawEllipse(-15, -15, 30, 30);
}
