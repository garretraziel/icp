#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "connectdialog.h"

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

class pnItem: public QGraphicsItem
{
public:
    QRectF boundingRect() const;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //asi mozna lepsi pres globalno promennou, uvidime
    QGraphicsScene * canvas;
};

class pnCircle: public pnItem{
public:
    pnCircle(QGraphicsScene * _canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};
class pnRect: public pnItem{
public:
    pnRect(QGraphicsScene * _canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


#endif // MAINWINDOW_H
