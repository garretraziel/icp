/**
 * @file mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include "editdialog.h"
#include "pngui.h"
#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "aboutdialog.h"
#include "../server/simstate.h"
#include "../server/pnplace.h"
#include "../server/pntrans.h"
#include <QFileDialog>
#include "propertiesdialog.h"
#include "loadsimdialog.h"
#include "helpdialog.h"

namespace Ui {
class MainWindow;
}

/**
 *
 * Trida zastitujici zobrazeni a praci s hlavnim oknem
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici MainWindow
      * @param predek pro uklid
      */
    explicit MainWindow(QWidget *parent = 0);

    /**
      * Destruktor MainWindow
      */
    ~MainWindow();

    editDialog * getEditor();
    SimState * getCurrentSim();
    int getCurrentIndex();

    void setSimName(QString name);

    void setID(QString id);
    QString getID();

    int findID(QString _id);
    void simReload(QString _id, QString newSimState);

    void setStatusLabel(QString status, QString color);

private:
    Ui::MainWindow *ui;///user interface

    ConnectDialog * cd;     ///connect dialog pointer
    aboutDialog * ad;       ///about dialog pointer
    propertiesDialog * pd;  ///properties dialog pointer
    loadSimDialog * ld;     ///loadsim dialog pointer
    helpDialog * hd;        ///help dialog pointer

    QLabel * statusLabel;   ///status label (online/offline status)

    void __loadSimString(QString simString);
    void __loadSimStringNoNewTab(QString simString);

    pnItem * __addItem(PNPlace * simPlace);
    pnItem * __addItemRect(PNTrans * simTrans);

    std::vector<QWidget *> tabVect;
    std::vector<QGraphicsView *> viewVect;
    std::vector<QGraphicsScene *> canvasVect;


    std::vector<SimState *> simVect;
    std::vector<QString> idVect;

    editDialog * editor;

    void __loadSim(QString fileName);

    QGraphicsView * currentTabView();

private slots:
    void preClose();

public slots:
    void actAct(int i);
    void showConnectDialog();
    void showAboutDialog();
    void newTab();
    pnItem * addItem();
    pnItem * addItemRect();
    void checkErase();
    void loadSim();
    void saveSim();

    void loadLocalSim();
    void saveLocalSim();

    void zoomOut();
    void zoomIn();

    void editProperties();

    void simOk();
    void runSim();
    void stepSim();
};

extern MainWindow * mw;

#endif // MAINWINDOW_H
