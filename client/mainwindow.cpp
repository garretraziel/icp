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
#include "communicator.h"

MainWindow * mw;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);
    ad = new aboutDialog(this);
    editor = new editDialog(this);
    mw = this;
    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),this, SLOT(showConnectDialog()) );
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(addItem()));
    QObject::connect(ui->pushButtonRect, SIGNAL(clicked()),this,SLOT(addItemRect()));
    QObject::connect(ui->actionAbout, SIGNAL(activated()),this,SLOT(showAboutDialog()));

    QObject::connect(ui->actionNew_Simulation, SIGNAL(activated()),this,SLOT(newTab()));
    QObject::connect(ui->actionLoad_Simulation, SIGNAL(activated()),this, SLOT(loadSim()));
    QObject::connect(ui->actionSave_Simulation, SIGNAL(activated()),this, SLOT(saveSim()));

    QObject::connect(ui->deleter, SIGNAL(clicked()),this,SLOT(checkErase()));

    QObject::connect(ui->zoomOut, SIGNAL(clicked()),this,SLOT(zoomOut()));
    QObject::connect(ui->zoomIn, SIGNAL(clicked()),this,SLOT(zoomIn()));
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

    simVect.push_back(new SimState());
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
        guard += "??"; //TODO! jeste nefunguje jinde
    guard += " && ";

    return guard;
}

QString fromOperation(OneOut oper){
    QString func = "";
    func += oper.output + "= ";
    foreach(Operation op, oper.operations){
        func+=((op.op==SUB)? "-":"+")+op.var+" ";
    }
    func += "; ";

    return func;
}

#define loadLines(direction)

void MainWindow::loadSim(){
    newTab();
    SimState * currentSim = getCurrentSim();
    //TOO DOO
    //(((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scale(0.5,0.5));

    QFile xmlfile("semafor.xml");
    xmlfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray xml = xmlfile.readAll();
    xmlfile.close();
    currentSim->setState(xml.data());
    QPointF center(0,0);
    int centerCnt = 1;
    std::map<PNPlace *, pnItem *> placeToGui;
    foreach(PNPlace * place, currentSim->places){
        pnItem * item = __addItem(place);
        placeToGui[place] = item;
        QString id = "";
        foreach(pntype token, place->getTokens()){
            id += QString::number(token) + ", ";
        }
        id = id.mid(0,id.length()-2);
        item->label->setPlainText(id);
        item->setPosition(place->x.toInt(),place->y.toInt());
        center += QPointF(item->pos());
        centerCnt++;
    }


    foreach(PNTrans * transit, currentSim->transits){
        pnItem * item = __addItemRect(transit);
        QString guard = "";
        foreach(Constraint * cons, transit->constraints){
            guard += fromConstraint(cons);
        }
        guard = guard.mid(0,guard.length()-4);
        item->label->setPlainText(guard);
        QString func = "";
        foreach(OneOut oper, transit->operations){
            func += fromOperation(oper);
        }
        func = func.mid(0,func.length()-2);
        item->funcLabel->setPlainText(func);
        item->setPosition(transit->x.toInt(),transit->y.toInt());

        center += QPointF(item->pos());
        centerCnt++;

        StringToPnplaceMap::iterator it;
        for(it=transit->in_names.begin(); it!=transit->in_names.end(); it++){
            pnLine * newLine = new pnLine(placeToGui[(*it).second],item, item->canvas);
            newLine->label->setPlainText((*it).first);
        }
        for(it=transit->out_names.begin(); it!=transit->out_names.end(); it++){
            pnLine * newLine = new pnLine(item,placeToGui[(*it).second], item->canvas);
            newLine->label->setPlainText((*it).first);
        }

    }
    ((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->centerOn(center/centerCnt);
    return;

}

void MainWindow::saveSim(){
    if(!getCurrentSim()->checkConfiguration())
        QMessageBox::critical(this, "Wrong configuration", "Check the vars on edges and vars in transits");
    else{
        std::cout << getCurrentSim()->getState().toStdString();
        communicator.sendSimState(getCurrentSim()->getState());
    }
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
    SimState * currentSim = getCurrentSim();
    if(!currentSim)
        return NULL;

    currentSim->places.push_back(new PNPlace());

#define simPlace ((PNPlace *)(currentSim->places.back()))
    simPlace->id = QString::number((long long)simPlace); //FIXME!
    simPlace->x = QString::number(this->x());
    simPlace->y = QString::number(this->y());
    return __addItem(simPlace);
#undef simPlace
}

pnItem * MainWindow::__addItemRect(PNTrans *simTrans){
    noTabReturn;
    pnItem * item = new pnRect(currentTabScene, simTrans);
    return item;
}

pnItem * MainWindow::addItemRect(){
    SimState * currentSim = getCurrentSim();
    if(!currentSim)
        return NULL;

    currentSim->transits.push_back(new PNTrans());
#define simTrans ((PNTrans *)(currentSim->transits.back()))
    simTrans->id = QString::number((long long)this);
    simTrans->x = QString::number(this->x());
    simTrans->y = QString::number(this->y());
    return __addItemRect(simTrans);
#undef simTrans
}


void MainWindow::checkErase(){
    erase = ui->deleter->isChecked();
}

void MainWindow::zoomOut(){
    ((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scale(0.8,0.8);
}

void MainWindow::zoomIn(){
    ((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]))->scale(1/0.8,1/0.8);
}


SimState * MainWindow::getCurrentSim(){
    int cnt = ui->tabWidget->count();
    if(!cnt)
        return NULL;
    else
        return simVect[ui->tabWidget->currentIndex()];
}

editDialog * MainWindow::getEditor(){
    return editor;
}
