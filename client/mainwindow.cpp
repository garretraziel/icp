#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QGraphicsScene>
#include <iostream>
#include <vector>
#include <QSizePolicy>
#include <QPointF>
#include "pngui.h"
#include <QtXml>
#include <QDomDocument>
#include <QFile>
#include <map>
#include "../server/simstate.h"

MainWindow * mw;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);
    ad = new aboutDialog(this);
    mw = this;
    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),this, SLOT(showConnectDialog()) );
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(addItem()));
    QObject::connect(ui->pushButtonRect, SIGNAL(clicked()),this,SLOT(addItemRect()));
    QObject::connect(ui->actionAbout, SIGNAL(activated()),this,SLOT(showAboutDialog()));
    QObject::connect(ui->actionNew_Simulation, SIGNAL(activated()),this,SLOT(newTab()));
    QObject::connect(ui->actionLoad_Simulation, SIGNAL(activated()),this, SLOT(loadSim()));
    QObject::connect(ui->deleter, SIGNAL(clicked()),this,SLOT(checkErase()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cd;
    delete ad;

    foreach(QGraphicsScene * canvas, canvasVect){
        delete canvas;
    }
    foreach(QGraphicsView * view, viewVect){
        delete view;
    }
    //TODO ja kurva nevim jestli to uklizi samo nejak nebo ne
}

void MainWindow::newTab(){
    //TODO oddelat ty dementni back();
    tabVect.push_back(new QWidget(this));

    //~~~~~ tyto dva musi byt v tomto poradi
    viewVect.push_back(new QGraphicsView(tabVect.back()));
    QLayout * layout = new QVBoxLayout(tabVect.back()); //TODO zkontrolovat jestli se uklidi
    //~~~~~

    canvasVect.push_back(new QGraphicsScene(viewVect.back()));
    viewVect.back()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    viewVect.back()->setRenderHint(QPainter::Antialiasing);
    viewVect.back()->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    viewVect.back()->setScene(canvasVect.back());

    canvasVect.back()->setSceneRect(0,0,2000,2000);
    viewVect.back()->centerOn(1000,1000);

    layout->addWidget(viewVect.back());

    ui->tabWidget->addTab(tabVect.back(),QString("Unnamed simulation"));
    ui->tabWidget->setCurrentWidget(tabVect.back());

}

QString fromConstraint(Constraint * cons){
    QString guard = "";
    QString ops[] = {"<","<=",">=",">","==","!="};
    guard += cons->first;
    guard += ops[cons->op];
    if(cons->type == TYPEVAR)
        guard += cons->second_var;
    else
    if(cons->type == TYPECONST)
        guard += QString::number(cons->second_const);
    else
        guard += "??";
    guard += " && ";

    return guard;
}

QString fromOperation(OneOut oper){
    QString func = "";
    func += oper.output + "= 0";
    foreach(Operation op, oper.operations){
        func+=((op.op==SUB)? "-":"+")+op.var;
    }
    func += " | ";

    return func;
}

#define loadLines(direction)

void MainWindow::loadSim(){
    newTab();

    //TOO DOO
    //(((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scale(0.5,0.5));

    simVect.push_back(new SimState());
    QFile xmlfile("test.xml");
    xmlfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray xml = xmlfile.readAll();
    xmlfile.close();
    simVect.back()->setState(xml.data());

    std::map<PNPlace *, pnItem *> placeToGui;
    foreach(PNPlace * place, simVect.back()->places){
        pnItem * item = __addItem(place);
        placeToGui[place] = item;
        QString id = place->id + ": ";
        foreach(pntype token, place->getTokens()){
            id += QString::number(token) + ", ";
        }
        item->label->setPlainText(id);
        item->setPosition(place->x.toInt(),place->y.toInt());
    }


    foreach(PNTrans * transit, simVect.back()->transits){
        pnItem * item = __addItemRect(transit);
        QString guard = "";
        foreach(Constraint * cons, transit->constraints){
            guard += fromConstraint(cons);
        }
        item->label->setPlainText(guard);
        QString func = "";
        foreach(OneOut oper, transit->operations){
            func += fromOperation(oper);
        }
        item->funcLabel->setPlainText(func);
        item->setPosition(transit->x.toInt(),transit->y.toInt());

        StringToPnplaceMap::iterator it;
        for(it=transit->in_names.begin(); it!=transit->in_names.end(); it++){
            pnLine * newLine = new pnLine(placeToGui[(*it).second],item, item->canvas);
            newLine->label->setPlainText((*it).first);
        }
        for(it=transit->out_names.begin(); it!=transit->out_names.end(); it++){
            pnLine * newLine = new pnLine(item,placeToGui[(*it).second], item->canvas);
            newLine->label->setPlainText((*it).first);
        }

        item->canvas->views()[0]->centerOn(item);
    }

    return;

}

void MainWindow::showConnectDialog()
{
    cd->show();
}

void MainWindow::showAboutDialog()
{
    ad->show();
}

//TOTO KURVA, TOTO JE MOC MOC MOC MOC KREHKY!!!!!!!!!! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ index !!
#define currentTabScene (((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scene())
#define noTabReturn if(ui->tabWidget->currentWidget() == NULL) return NULL


pnItem * MainWindow::__addItem(PNPlace * simPlace){
    noTabReturn;

    pnItem * item = new pnCircle(currentTabScene, simPlace);
    return item;
}

pnItem * MainWindow::addItem(){
    //FIXME!!!!! UPRAVIT PRO VICE SCEN
    simVect.back()->places.push_back(new PNPlace());
    return __addItem(simVect.back()->places.back());
}

pnItem * MainWindow::__addItemRect(PNTrans *simTrans){
    noTabReturn;

    pnItem * item = new pnRect(currentTabScene, simTrans);
    return item;
}

pnItem * MainWindow::addItemRect(){
    simVect.back()->transits.push_back(new PNTrans());
    return __addItemRect(simVect.back()->transits.back());
}


void MainWindow::checkErase(){
    erase = ui->deleter->isChecked();
}

