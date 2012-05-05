#include "loadsimdialog.h"
#include "ui_loadsimdialog.h"
#include <QTableWidgetItem>
#include <QDebug>

loadSimDialog::loadSimDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadSimDialog)
{
    ui->setupUi(this);
    index = 1;

}

loadSimDialog::~loadSimDialog()
{

    delete ui;
}

void loadSimDialog::pushSim(QString name, QString author, QString version, QString info){


    qDebug() << "Hurr durr";
    ui->listWidget->addItem(name+" v"+version+" by "+author+" ("+info+")");
    qDebug() << "Durr hurr";
    index++;
}
