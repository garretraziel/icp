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

void MainWindow::loadSim(){
    newTab();
    QString xml = "test.xml";
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument document;
    if (!document.setContent(xml,&errorStr,&errorLine,&errorColumn)) {
        qCritical() << "Error during p/arsing xml on line: " << errorLine << ", column: " << errorColumn;
        qCritical() << errorStr;
        return;
    }

    QDomElement root = document.documentElement();

    if (root.tagName() != "petrinet") {
        qCritical() << "Error during parsing xml, not valid";
        return;
    }

    QDomElement xml_places = root.firstChildElement("places");
    QDomElement one_place = xml_places.firstChildElement("place");

    while (!one_place.isNull()) {
        pnItem * place = addItem();
        place->label->setPlainText(one_place.attribute("id"));
        place->setPos(one_place.attribute("posx").toInt(), one_place.attribute("posy").toInt());
        /*
        QDomElement one_token = one_place.firstChildElement("token");
        while (!one_token.isNull()) {
            pntype token = one_token.text().toInt();
            tokens.push_back(token);
            one_token = one_token.nextSiblingElement("token");
        }
        */
        one_place = one_place.nextSiblingElement("place");
    }

    QDomElement xml_trans = root.firstChildElement("transitions");
    QDomElement one_trans = xml_trans.firstChildElement("transition");

    while (!one_trans.isNull()) {
        pnItem * place = addItemRect();
        place->label->setPlainText(one_place.attribute("id"));
        place->setPos(one_place.attribute("posx").toInt(), one_place.attribute("posy").toInt());

        QDomElement one_element = one_trans.firstChildElement("inplace");
        while (!one_element.isNull()) {

            one_element = one_element.nextSiblingElement("inplace");
        }

        one_element = one_trans.firstChildElement("outplace");
        while (!one_element.isNull()) {

            one_element = one_element.nextSiblingElement("outplace");
        }

        QDomElement one_cond = one_trans.firstChildElement("constraint");
        while (!one_cond.isNull()) {
            /*
            Constraint *cond;
            if (one_cond.attribute("type") == "const") {
                cond = new Constraint(one_cond.attribute("var1"),one_cond.attribute("op").toInt(),one_cond.attribute("const").toInt());
            } else {
                cond = new Constraint(one_cond.attribute("var1"),one_cond.attribute("op").toInt(),one_cond.attribute("var2"));
            }
            constraints.push_back(cond);
            */
            one_cond = one_cond.nextSiblingElement("constraint");
        }

        QDomElement one_op = one_trans.firstChildElement("operation");
        while (!one_op.isNull()){
            /*
            OneOut oneout;
            oneout.output = one_op.attribute("output");
            QDomElement one_operation = one_op.firstChildElement();
            while(!one_operation.isNull()) {
                Operation op;
                if (one_operation.tagName() == "plus") {
                    op.op = ADD;
                } else {
                    op.op = SUB;
                }
                op.var = one_operation.attribute("id");
                oneout.operations.push_back(op);
                one_operation = one_operation.nextSiblingElement();
            }
            operations.push_back(oneout);
            */
            one_op = one_op.nextSiblingElement("operation");
        }

        one_trans = one_trans.nextSiblingElement("transition");
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
//tyhle dve by snad mohly jit sjednotit do jedne
pnItem * MainWindow::addItem(){
    noTabReturn;

    pnItem * item = new pnCircle(currentTabScene);
    return item;
}

pnItem * MainWindow::addItemRect(){
    noTabReturn;

    pnItem * item = new pnRect(currentTabScene);
    return item;
}

void MainWindow::checkErase(){
    erase = ui->deleter->isChecked();
}

