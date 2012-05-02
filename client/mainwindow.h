#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include "pngui.h"
#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "aboutdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    ConnectDialog * cd;
    aboutDialog * ad;
    QGraphicsScene * canvas;

    std::vector<QWidget *> tabVect;
    std::vector<QGraphicsView *> viewVect;
    std::vector<QGraphicsScene *> canvasVect;

public slots:
    void showConnectDialog();
    void showAboutDialog();
    void newTab();
    pnItem * addItem();
    pnItem * addItemRect();
    void checkErase();
    void loadSim();
};

extern MainWindow * mw;

#endif // MAINWINDOW_H
