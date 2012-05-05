#include "loadsimdialog.h"
#include "ui_loadsimdialog.h"
#include <QTableWidgetItem>
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>

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

void loadSimDialog::pushSim(StringVector sim){

    items.push_back(sim);
}

void loadSimDialog::updateList(){
    ui->listWidget->clear();
    QStringList filterList = ui->filterEdit->text().split(QRegExp("\\s*,\\s*"));
    foreach(StringVector item, items){
        foreach(QString filter, filterList){
            qDebug() << filter;
            if((item[0]+item[1]+item[2]+item[3]).contains(filter,Qt::CaseInsensitive)){
                ui->listWidget->addItem(item[0]+"\tv"+item[1]+"\t by"+item[2]+"\t("+item[3]+")");
                break;
            }
        }
    }
}

void loadSimDialog::accept(){
    QString selected = ui->listWidget->item(ui->listWidget->currentRow())->text();
    foreach(StringVector item, items){
        if(selected == item[0]+"\tv"+item[1]+"\t by"+item[2]+"\t("+item[3]+")"){
            if(!communicator.loadThis(item[0],item[1]))
                QMessageBox::critical(this,"Error","Server didn't response");
            else {
                this->hide();
            }
            return;
        }
    }
}
