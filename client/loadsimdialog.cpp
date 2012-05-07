/**
 * @file loadsimdialog.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro zobrazeni, vyhledani a nacetni siti ze serveru
 *
 */

#include "loadsimdialog.h"
#include "ui_loadsimdialog.h"
#include <QTableWidgetItem>
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>

/**
  * Konstruktor vytvarejici loadSimDialog
  * @param predek pro uklid
  */
loadSimDialog::loadSimDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadSimDialog)
{
    ui->setupUi(this);
    index = 1;

    connect(ui->filterEdit,SIGNAL(textChanged(QString)),this,SLOT(updateList()));
    connect(this,SIGNAL(accepted()),ui->listWidget,SLOT(clear()));
    connect(this,SIGNAL(rejected()),ui->listWidget,SLOT(clear()));
    connect(this,SIGNAL(accepted()),this,SLOT(clearSims()));
    connect(this,SIGNAL(rejected()),this,SLOT(clearSims()));
}

/**
  * Destruktor loadSimDialog
  */
loadSimDialog::~loadSimDialog()
{

    delete ui;
}

/**
  * zapise informace o simulaci do seznamu
  * @param sim vektor vlatnosti site (jmeno, autor, verze, info)
  */
void loadSimDialog::pushSim(StringVector sim){

    items.push_back(sim);
}

/**
  * slot volany pri zmene filter pole (profiltruje vypsane hodnoty)
  */
void loadSimDialog::updateList(){
    ui->listWidget->clear();
    QStringList filterList = ui->filterEdit->text().split(QRegExp("\\s*,\\s*"));
    foreach(StringVector item, items){
        foreach(QString filter, filterList){            
            if((item[0]+item[1]+item[2]+item[3]).contains(filter,Qt::CaseInsensitive)){
                ui->listWidget->addItem(item[0]+"\tv"+item[1]+"\t by "+item[2]+"\t("+item[3]+")");
                break;
            }
        }
    }
}

/**
  * slot volany pri prijeti dialogu (klik na OK)
  */
void loadSimDialog::accept(){
    QString selected = ui->listWidget->item(ui->listWidget->currentRow())->text();
    foreach(StringVector item, items){
        if(selected == item[0]+"\tv"+item[1]+"\t by "+item[2]+"\t("+item[3]+")"){
            if(!communicator.loadThis(item[0],item[1]))
                QMessageBox::critical(this,"Error","Server didn't response");
            else {
                this->hide();
            }
            items.clear();
            return;
        }
    }
    items.clear();
}

/**
  * slot zajistujici smazani simulaci ze seznamu
  */
void loadSimDialog::clearSims()
{
    items.clear();
}
