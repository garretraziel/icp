#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    //scene = new QGraphicsScene();
    ui->setupUi(this);
    //ui->graphicsView->setScene(scene);
    //scene->addPixmap(QPixmap("typologo.png"));
    QWidget * img = new QWidget(this);
}

aboutDialog::~aboutDialog()
{
    delete ui;

}
