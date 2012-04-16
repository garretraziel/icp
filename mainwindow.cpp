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
    QObject::connect(ui->actionAbout, SIGNAL(activated()),this,SLOT(showAboutDialog()));
    QObject::connect(ui->actionNew_Simulation, SIGNAL(activated()),this,SLOT(newTab()));

    /*
    canvas = new QGraphicsScene(this);
    ui->view->setScene(canvas);
    ui->view->setRenderHint(QPainter::Antialiasing);
    ui->view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    */
    //toto fakt není uplnì nejlep¹í
    //((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->setRenderHint(QPainter::Antialiasing);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete cd;

    foreach(QGraphicsScene * canvas, canvasVect){
        delete canvas;
    }
    foreach(QGraphicsView * view, viewVect){
        delete view;
    }

    //TODO ja kurva nevim jestli to uklizi samo a valgrindu se bojim, co to vyhodi
}

void MainWindow::newTab(){
    //TODO oddelat ty dementni back();
    tabVect.push_back(new QWidget());
    viewVect.push_back(new QGraphicsView(tabVect.back()));
    canvasVect.push_back(new QGraphicsScene(viewVect.back()));
    //TOTO vyresit rozmery
    viewVect.back()->setGeometry(0,0,ui->tabWidget->width(),ui->tabWidget->height());
    viewVect.back()->setRenderHint(QPainter::Antialiasing);
    viewVect.back()->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    viewVect.back()->setScene(canvasVect.back());


    ui->tabWidget->addTab(tabVect.back(),QString("Empty"));
    ui->tabWidget->setCurrentWidget(tabVect.back());
}

void MainWindow::showConnectDialog()
{
    cd->show();
}

void MainWindow::showAboutDialog()
{
    //ad->show();
}


#define currentTabScene (((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scene())

//tyhle dve by snad mohly jit sjednotit do jedne
void MainWindow::addItem(){
    pnItem * item = new pnCircle(currentTabScene);
    Q_UNUSED(item);
    //toto asi neni moc cisty, no
    //canvas->addItem(item);
}

void MainWindow::addItemRect(){
    pnItem * item = new pnRect(currentTabScene);
    Q_UNUSED(item);
}



