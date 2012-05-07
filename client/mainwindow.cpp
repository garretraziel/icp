/**
 * @file mainwindow.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Trida pro hlavni okno, ktere slouzi jako klientsky editor a
 * prohlizec siti.
 *
 */


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
#include <time.h>

MainWindow * mw;

int UniqID = 0; ///< unikatni id prvku ve scene

/**
  * Konstruktor vytvarejici MainWindow
  * @param predek pro uklid
  */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);
    ad = new aboutDialog(this);
    editor = new editDialog(this);
    ld = new loadSimDialog(this);
    pd = new propertiesDialog(this);
    hd = new helpDialog(this);

    mw = this;
    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),this, SLOT(showConnectDialog()) );
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(addItem()));
    QObject::connect(ui->pushButtonRect, SIGNAL(clicked()),this,SLOT(addItemRect()));
    QObject::connect(ui->actionAbout, SIGNAL(activated()),this,SLOT(showAboutDialog()));

    QObject::connect(ui->actionNew_Simulation, SIGNAL(activated()),this,SLOT(newTab()));
    QObject::connect(ui->actionLoad_Simulation, SIGNAL(activated()),this, SLOT(loadSim()));
    QObject::connect(ui->actionSave_Simulation, SIGNAL(activated()),this, SLOT(saveSim()));
    QObject::connect(ui->sendToServer, SIGNAL(clicked()),this, SLOT(saveSim()));

    QObject::connect(ui->deleter, SIGNAL(clicked()),this,SLOT(checkErase()));

    QObject::connect(ui->zoomOut, SIGNAL(clicked()),this,SLOT(zoomOut()));
    QObject::connect(ui->zoomIn, SIGNAL(clicked()),this,SLOT(zoomIn()));

    QObject::connect(ui->actionLoad_Local_Sim, SIGNAL(activated()), this, SLOT(loadLocalSim()));

    QObject::connect(ui->actionSave_Local_Sim, SIGNAL(activated()), this, SLOT(saveLocalSim()));

    QObject::connect(ui->actionEdit_Properties, SIGNAL(activated()),this, SLOT(editProperties()));

    QObject::connect(&communicator, SIGNAL(simOk()), this, SLOT(simOk()));
    QObject::connect(ui->runButton, SIGNAL(clicked()), this, SLOT(runSim()));
    QObject::connect(ui->stepButton, SIGNAL(clicked()), this, SLOT(stepSim()));

    QObject::connect(ui->actionExit, SIGNAL(activated()), this, SLOT(preClose()));

    QObject::connect(ui->actionHelp, SIGNAL(activated()), this->hd, SLOT(show()));

    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(actAct(int)));

    statusLabel = new QLabel;
    ui->statusBar->addWidget(statusLabel);

    setStatusLabel("Offline", "#ff0000");
}

/**
  * Slot aktualizujici informaci o aktualnosti site
  */
void MainWindow::actAct(int i){
    unsigned int j = i;
    if(simVect.empty() || j >= simVect.size())
        return;

    if(simVect[i]->isAct)
        ui->actLabel->setText("na serveru je aktualni verze");
    else
        ui->actLabel->setText("na serveru neni aktualni verze");
}

/**
  * Slot volany pred zavrenim okna (uklizeni statusbaru)
  */
void MainWindow::preClose(){
    communicator.blockSocket(true);
    delete statusLabel;
    statusLabel = NULL;
    this->close();
}

/**
  * Destruktor MainWindow
  */
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

}

/**
  * Slot otevirajici novy tab
  */
void MainWindow::newTab(){


    tabVect.push_back(new QWidget(this));

    //~~~~~ tyto dva musi byt v tomto poradi
    viewVect.push_back(new QGraphicsView(tabVect.back()));
    QLayout * layout = new QVBoxLayout(tabVect.back());
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

    SimState * newSim = new SimState();
    simVect.push_back(newSim);

    simVect.back()->name = "Unnamed simulation";
    simVect.back()->info = "(Empty info)";
    simVect.back()->author = communicator.userLoggedIn();
    if (simVect.back()->author == "")
        simVect.back()->author = "Offline user";

    idVect.push_back("unkown");

}

/**
  * Zobrazeni z OneOut do QString (vyvotreni textu podminky)
  * @return text operace
  */
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

/**
  * Zobrazeni z OneOut do QString (vyvotreni textu operace)
  * @return text operace
  */
QString fromOperation(OneOut oper){
    QString func = "";
    func += oper.output + "= ";
    foreach(Operation op, oper.operations){
        func+=((op.op==SUB)? "-":"+")+op.var+" ";
    }
    func += "; ";

    return func;
}

/**
  * Nacte simulaci ze souboru
  * @param fileName jmeno souboru
  */
void MainWindow::__loadSim(QString fileName) {
    QFile xmlfile(fileName);
    xmlfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray xml = xmlfile.readAll();
    xmlfile.close();
    __loadSimString(xml.data());
}

/**
  * Nacte simulaci z XML retezce, ulozi do aktualni
  * @param simString XML stav simulace
  */
void MainWindow::__loadSimString(QString simString){
    newTab();
    __loadSimStringNoNewTab(simString);
}

/**
  * Nacte simulaci z XML ..., neotevira novy tab
  * @param simString XML stav simulace
  */
void MainWindow::__loadSimStringNoNewTab(QString simString){

    SimState * currentSim = getCurrentSim();

    currentSim->setState(simString);
    mw->setSimName(getCurrentSim()->name);

    QPointF center(0,0);
    int centerCnt = 0;
    std::map<PNPlace *, pnItem *> placeToGui;
    foreach(PNPlace * place, currentSim->places){
        UniqID = (UniqID <= place->id.toInt())? place->id.toInt()+1 : UniqID;
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
        UniqID = (UniqID <= transit->id.toInt())? transit->id.toInt()+1 : UniqID;
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
    /*if(centerCnt)
        currentTabView()->centerOn(center/centerCnt);
    */
    return;

}

/**
  * Slot nacitajici simulaci
  */
void MainWindow::loadSim(){

    simList simulations;
    if(!communicator.getSimulations(simulations)){
        QMessageBox::critical(this,"Error","Server didn't response");
        return;
    }

    foreach(StringVector sim, simulations)
        ld->pushSim(sim);

    ld->updateList();
    ld->show();
    //__loadSim("");
}

/**
  * Slot ukladajici simulaci
  */
void MainWindow::saveSim(){
    if(!getCurrentSim())
        return;

    if(!getCurrentSim()->checkConfiguration())
        QMessageBox::critical(this, "Wrong configuration", "Check the vars on edges and vars in transits");
    else{

        QString message;
        if(!communicator.saveSimState(getCurrentSim()->getState(), message)){
            QMessageBox::critical(this, "Error", message);
        } else {
            getCurrentSim()->isAct = true;
            mw->actAct(mw->getCurrentIndex());
        }
    }
}

/**
  * Ziska aktualni index
  * @return aktualni index
  */
int MainWindow::getCurrentIndex(){
    return ui->tabWidget->currentIndex();
}

/**
  * Slot ukladajici simulaci lokalne
  */
void MainWindow::saveLocalSim(){
    if(!getCurrentSim())
        return;

    if(!getCurrentSim()->checkConfiguration())
        QMessageBox::critical(this, "Wrong configuration", "Check the vars on edges and vars in transits");
    else{

        QString xmlOut = getCurrentSim()->getState();
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save Local Simulation",
                                                        QDir::currentPath()+"/examples/",
                                                        "XML Sim File (*.xml)");

        if(fileName == "")
            return;

        QFile fileOut(fileName);
        if(!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::critical(this,"Error", "Unable to open file "+fileName+" for writing");
            return;
        }

        //fileOut.write(xmlOut.toStdString().data());
        QTextStream out(&fileOut);
        out << xmlOut;

    }

}

/**
  * Slot zobrazujici connect dialog
  */
void MainWindow::showConnectDialog()
{
    cd->show();
}

/**
  * Slot zobrazujici about dialog
  */
void MainWindow::showAboutDialog()
{
    ad->show();
}

/**
  * Ziskani aktualniho zobrazovace scen
  * @return uakaztel na aktualni zobrazovac scen
  */
QGraphicsView * MainWindow::currentTabView(){
    return ((QGraphicsView *)(ui->tabWidget->currentWidget()->children()[0]));
}

#define returnIfNoTab if(ui->tabWidget->currentWidget() == NULL) return

/**
  * Prida misto do aktualni sceny
  * @param simPlace ukazatel na misto do SimState
  * @return ukazatel na misto
  */
pnItem * MainWindow::__addItem(PNPlace * simPlace){
    returnIfNoTab NULL;
    pnItem * item = new pnCircle(currentTabView()->scene(), simPlace);
    return item;
}

/**
  * Slot pridavajici misto do sceny
  * @return ukazatel na misto (nepouzivan, jen pro uplnost)
  */
pnItem * MainWindow::addItem(){
    SimState * currentSim = getCurrentSim();
    if(!currentSim)
        return NULL;

    currentSim->places.push_back(new PNPlace());
    currentSim->isAct = false;
    actAct(ui->tabWidget->currentIndex());
#define simPlace ((PNPlace *)(currentSim->places.back()))
    simPlace->id = QString::number(++UniqID);
    simPlace->x = QString::number(int(this->x()));
    simPlace->y = QString::number(int(this->y()));
    return __addItem(simPlace);
#undef simPlace
}

/**
  * Prida prechod do aktialni sceny
  * @param simTrans ukazatel na prechod do SimState
  * @return ukazatel na prechod
  */
pnItem * MainWindow::__addItemRect(PNTrans *simTrans){
    returnIfNoTab NULL;
    pnItem * item = new pnRect(currentTabView()->scene(), simTrans);

    return item;
}

/**
  * Slot pridavajici prechod do sceny
  * @return ukazatel na misto (nepouzivan, jen pro uplnost)
  */
pnItem * MainWindow::addItemRect(){
    SimState * currentSim = getCurrentSim();
    if(!currentSim)
        return NULL;
    currentSim->isAct = false;
    actAct(ui->tabWidget->currentIndex());
    currentSim->transits.push_back(new PNTrans());
#define simTrans ((PNTrans *)(currentSim->transits.back()))
    simTrans->id = QString::number(++UniqID);
    simTrans->x = QString::number(int(this->x()));
    simTrans->y = QString::number(int(this->y()));
    return __addItemRect(simTrans);
#undef simTrans

}

/**
  * Slot kontrolujici nastaveni mazani (nastavuje globalni promennou)
  */
void MainWindow::checkErase(){
    erase = ui->deleter->isChecked();
}

/**
  * Slot pro oddaleni sceny
  */
void MainWindow::zoomOut(){
    if(getCurrentSim())
        currentTabView()->scale(0.8,0.8);
}

/**
  * Slot pro priblizeni sceny
  */
void MainWindow::zoomIn(){
    if(getCurrentSim())
        currentTabView()->scale(1/0.8,1/0.8);
}

/**
  * Ziska aktualni simulaci
  * @return ukazatel na aktualni simulaci
  */
SimState * MainWindow::getCurrentSim(){
    int cnt = ui->tabWidget->count();
    if(!cnt)
        return NULL;
    else
        return simVect[ui->tabWidget->currentIndex()];
}

/**
  * Nastavi ID simulace
  * @param id id simulace
  */
void MainWindow::setID(QString id){
    idVect[ui->tabWidget->currentIndex()] = id;
}

/**
  * Ziksa ID simulace
  * @return ID simulace
  */
QString MainWindow::getID(){
    return idVect[ui->tabWidget->currentIndex()];
}

/**
  * Ziska ukazatel na edit dialog
  * @return ukazatel na editDialog
  */
editDialog * MainWindow::getEditor(){
    return editor;
}

/**
  * Slot nacitajici lokalno simulaci
  */
void MainWindow::loadLocalSim(){
    //ld->show();
    QString fileName = QFileDialog::getOpenFileName(this, "Load local simulation",
                                                    QDir::currentPath()+"/examples/",
                                                    "XML Sim File (*.xml)");
    if(fileName == "")
        return;

    __loadSim(fileName);
}

/**
  * Slot pro editaci vlasnosti site
  */
void MainWindow::editProperties(){
    SimState * currentSim = getCurrentSim();
    if(!currentSim) return;

    pd->loadData(currentSim->name,currentSim->author,QString::number(currentSim->version),currentSim->info);
    pd->show();
}

/**
  * Nastavi jmeno simulaci (do tab widgetu)
  * @param name jmeno simulace
  */
void MainWindow::setSimName(QString name){
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), name);
}

/**
  * Slot pro naceni vykomunikovane site od serveru
  */
void MainWindow::simOk(){

    simReload(communicator.simID, communicator.sim);
}

/**
  * Slot spoustejici simulaci na serveru (pres communikator)
  */
void MainWindow::runSim() {
    if(!getCurrentSim()->isAct)
        return;

    QString id = idVect[ui->tabWidget->currentIndex()];
    communicator.runSimulation(id, true);
}

/**
  * Slot krokujici simulaci na serveru (pres komunikator)
  */
void MainWindow::stepSim() {
    if(!getCurrentSim()->isAct)
        return;

    QString id = idVect[ui->tabWidget->currentIndex()];
    communicator.runSimulation(id, false);
}

/**
  * Zobrazi ID simulace na index
  * @param _id ID simulace
  * @return index
  */
int MainWindow::findID(QString _id){
    for(unsigned int i= 0; i< idVect.size(); i++){
        if(idVect[i] == _id){
            return i;
        }
    }
    return -1;
}
/**
  * Znovu nacte simulaci a prepne na jeji tab
  * @param _id ID simulace
  * @param newSimstate XML stav simulace
  */
void MainWindow::simReload(QString _id, QString newSimState){
    int tabIndex = findID(_id);
    if(tabIndex == -1){
        //neni tab
        __loadSimString(newSimState);
        getCurrentSim()->isAct = true;
        actAct(ui->tabWidget->currentIndex());
        //snad to nebude indexovat blbe
        idVect.back() = _id;
    }
    else {
        ui->tabWidget->setCurrentIndex(tabIndex);
        std::vector<pnLine *> tmp;
        foreach(pnLine * l, lineVect){
            if(l->canvas == canvasVect[tabIndex])
                delete l;
            else
                tmp.push_back(l);
        }
        lineVect.clear();
        foreach(pnLine * l, tmp)
            lineVect.push_back(l);
        //smazani obsahu sceny

        //((QGraphicsScene *)canvasVect[tabIndex])->clear();

        currentTabView()->scene()->clear();


        __loadSimStringNoNewTab(newSimState);
        getCurrentSim()->isAct = true;
        actAct(tabIndex);

    }
}

/**
  * Nastavi online/offline status simulace a barvu
  * @param status text statusu
  * @param barava statusu
  */
void MainWindow::setStatusLabel(QString status, QString color){
    if(statusLabel){
        QString styleSheet = "QLabel { color : "+color+";}";
        statusLabel->setText(status);
        statusLabel->setStyleSheet(styleSheet);
    }
}



