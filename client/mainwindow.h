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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    editDialog * getEditor();
    SimState * getCurrentSim();

    void setSimName(QString name);

    void setID(QString id);
    QString getID();

private:
    Ui::MainWindow *ui;

    // dialogs
    ConnectDialog * cd;
    aboutDialog * ad;   
    propertiesDialog * pd;
    loadSimDialog * ld;
    //

    QGraphicsScene * canvas;


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

public slots:
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
};

extern MainWindow * mw;

#endif // MAINWINDOW_H
