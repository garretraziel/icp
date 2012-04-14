#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QGraphicsScene>
#include <iostream>
#include <vector>

#include "pngui.h"

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



