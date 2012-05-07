/**
 * @file propertiesdialog.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro nastaveni nazvu a informace u siti.
 *
 */

#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

#include "mainwindow.h"
#include "communicator.h"

/**
  * Konstruktor vytvarejici propertiesDialog
  * @param predek pro uklid
  */
propertiesDialog::propertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::propertiesDialog)
{
    ui->setupUi(this);
}

/**
  * Destruktor propertiesDialog
  */
propertiesDialog::~propertiesDialog()
{
    delete ui;
}

/**
  * Nacte data do dialogu
  * @param name jmeno simulace
  * @param author jmeno autora
  * @param version verze
  * @param info informace o simulaci
  */
void propertiesDialog::loadData(QString name, QString author, QString version, QString info){
    ui->nameEdit->setText(name);
    author = (author=="")? communicator.userLoggedIn() : author;
    ui->authorEdit->setText(author);
    ui->versionEdit->setText(version);
    ui->infoEdit->setPlainText(info);
}

/**
  * Slot volany po prijeti dialogu (klik na OK)
  */
void propertiesDialog::accept(){

    mw->getCurrentSim()->setProperies(
                ui->nameEdit->text(),
                ui->authorEdit->text(),
                ui->versionEdit->text(),
                ui->infoEdit->toPlainText()
    );

    mw->setSimName(ui->nameEdit->text());

    this->hide();
}
