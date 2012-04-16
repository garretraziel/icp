#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include "pngui.h"
#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>

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
    QGraphicsScene * canvas;

    std::vector<QWidget *> tabVect;
    std::vector<QGraphicsView *> viewVect;
    std::vector<QGraphicsScene *> canvasVect;

public slots:
    void showConnectDialog();
    void showAboutDialog();
    void newTab();
    void addItem();
    void addItemRect();
};



#endif // MAINWINDOW_H
