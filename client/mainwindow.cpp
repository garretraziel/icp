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
    //vytvoreni ui a dialogu
    ui->setupUi(this);
    cd = new ConnectDialog(this);
    ad = new aboutDialog(this);
    editor = new editDialog(this);
    ld = new loadSimDialog(this);
    pd = new propertiesDialog(this);
    hd = new helpDialog(this);

    mw = this;

    // propojeni signalu a slotu
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
        ui->actLabel->setText("server has an actual version");
    else
        ui->actLabel->setText("server has NOT an actual version (send it to server)");
}

/**
  * Slot volany pred zavrenim okna (uklizeni statusbaru)
  */
void MainWindow::preClose(){
    // workaround aby se nezasilal signal u vypinani
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

    //~~~~~ tyto dva musi byt v tomto poradi (kvuli ocekavanemu poradi widgetu)
    viewVect.push_back(new QGraphicsView(tabVect.back()));
    QLayout * layout = new QVBoxLayout(tabVect.back());
    //~~~~~

    // na za zasobniky vsechny potrebne informace a nastavi je

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
        guard += "??";
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
    //nastavi prazde simulaci stav
    currentSim->setState(simString);
    mw->setSimName(getCurrentSim()->name);



    //pro vsechny mista v simulaci vytvori jejich obraz ve scene
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

    }

    // pro vsechny prechody v simulaci vytvori jejich obrazy ve scene
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


        // propoji mista a prechody podle vstupu a vystupu v simulaci
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

    return;

}

/**
  * Slot nacitajici simulaci
  */
void MainWindow::loadSim(){

    simList simulations;
    //ziska od serveru seznam simulaci
    if(!communicator.getSimulations(simulations)){
        QMessageBox::critical(this,"Error","Server didn't response");
        return;
    }

    //a nastrka je do dialogu
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
        // zasle komunikatoru stav site a zkontroluje vysledek
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

        // lokalne ulozi do souboru
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
        QTextStream out(&fileOut);
        out << xmlOut;
        // zavre samo v destruktoru
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
    //do simulace vlozi novy prvek a nastavi neaktualno simulace
    currentSim->places.push_back(new PNPlace());
    currentSim->isAct = false;
    actAct(ui->tabWidget->currentIndex());
#define simPlace ((PNPlace *)(currentSim->places.back()))
    //nastavi vlastnosti prvku
    simPlace->id = QString::number(++UniqID);
    simPlace->x = QString::number(int(this->x()));
    simPlace->y = QString::number(int(this->y()));
    //a necha vytvorit grafickou reprezentaci
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

    // obdobne jako v addItem()

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
    getCurrentSim()->isAct = false;
    mw->actAct(mw->getCurrentIndex());
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
    if(simVect.empty())
        return;
    //neaktualni nesimulujeme
    if(!getCurrentSim()->isAct)
        return;
    // ziska ID simulace
    QString id = idVect[ui->tabWidget->currentIndex()];
    communicator.runSimulation(id, true);
}

/**
  * Slot krokujici simulaci na serveru (pres komunikator)
  */
void MainWindow::stepSim() {
    if(simVect.empty())
        return;
    //neaktualni nesimulujeme
    if(!getCurrentSim()->isAct)
        return;
    // ziska ID simulace
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
    //zjisti jestli uz je tab s danym ID
    int tabIndex = findID(_id);

    if(tabIndex == -1){
        //neni tab, vytvori novy a do nej simulaci
        __loadSimString(newSimState);
        getCurrentSim()->isAct = true;
        actAct(ui->tabWidget->currentIndex());
        idVect.back() = _id;
    }
    else {
        //tab je, jen smazeme cary zvlast
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
        // a obsah zvlast
        currentTabView()->scene()->clear();

        // nacteni ze stavu
        __loadSimStringNoNewTab(newSimState);

        //simulace je aktualni
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



