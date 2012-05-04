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

private:
    Ui::MainWindow *ui;
    ConnectDialog * cd;
    aboutDialog * ad;
    QGraphicsScene * canvas;
    pnItem * __addItem(PNPlace * simPlace);
    pnItem * __addItemRect(PNTrans * simTrans);

    std::vector<QWidget *> tabVect;
    std::vector<QGraphicsView *> viewVect;
    std::vector<QGraphicsScene *> canvasVect;

    std::vector<SimState *> simVect;

    editDialog * editor;

public slots:
    void showConnectDialog();
    void showAboutDialog();
    void newTab();
    pnItem * addItem();
    pnItem * addItemRect();
    void checkErase();
    void loadSim();
    void saveSim();

    void zoomOut();
    void zoomIn();
};

extern MainWindow * mw;

#endif // MAINWINDOW_H
