#include "loadsimdialog.h"
#include "ui_loadsimdialog.h"
#include <QTableWidgetItem>
#include <QString>
#include <QDebug>
#include <QRegExp>

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

void loadSimDialog::pushSim(QString sim){

    items.push_back(sim);
}

void loadSimDialog::updateList(){
    ui->listWidget->clear();
    QStringList filterList = ui->filterEdit->text().split(QRegExp("\\s*,\\s*"));
    foreach(QString item, items){
        foreach(QString filter, filterList){
            qDebug() << filter;
            if(item.contains(filter,Qt::CaseInsensitive)){
                ui->listWidget->addItem(item);
                break;
            }
        }
    }
}
