#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include "pngui.h"

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

public slots:
    void showConnectDialog();
    void addItem();
    void addItemRect();
};



#endif // MAINWINDOW_H
