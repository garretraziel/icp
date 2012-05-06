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

propertiesDialog::propertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::propertiesDialog)
{
    ui->setupUi(this);
}

propertiesDialog::~propertiesDialog()
{
    delete ui;
}

void propertiesDialog::loadData(QString name, QString author, QString version, QString info){
    ui->nameEdit->setText(name);
    author = (author=="")? communicator.userLoggedIn() : author;
    ui->authorEdit->setText(author);
    ui->versionEdit->setText(version);
    ui->infoEdit->setPlainText(info);
}

void propertiesDialog::accept(){
    //TODO mozna nejaky check...
    mw->getCurrentSim()->setProperies(
                ui->nameEdit->text(),
                ui->authorEdit->text(),
                ui->versionEdit->text(),
                ui->infoEdit->toPlainText()
    );

    mw->setSimName(ui->nameEdit->text());

    this->hide();
}
