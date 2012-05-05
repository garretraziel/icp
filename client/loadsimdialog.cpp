#include "loadsimdialog.h"
#include "ui_loadsimdialog.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QStringList>

loadSimDialog::loadSimDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadSimDialog)
{
    ui->setupUi(this);
    index = 1;

    connect(ui->filterEdit,SIGNAL(textChanged(QString)),this,SLOT(updateList()));

}

loadSimDialog::~loadSimDialog()
{

    delete ui;
}

void loadSimDialog::pushSim(QString name, QString author, QString version, QString info){

    ui->listWidget->addItem(name+" v"+version+" by "+author+" ("+info+")");

    index++;
}

void loadSimDialog::updateList(){
    QStringList filterList = ui->filterEdit->text().split("\\s");
}
