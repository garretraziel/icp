/**
 * @file connectiondiloag.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro pripojeni k serveru
 *
 */

#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "communicator.h"
#include <iostream>
#include <QString>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>
#include <QMessageBox>

/**
  * Konstruktor vytvarejici ConnecDialog
  * @param predek pro uklid
  */
ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(registerUser()));
}

/**
  * Destruktor ConnectDialog
  */
ConnectDialog::~ConnectDialog()
{
    delete ui;
}

/**
  * slot pri prijeti dialogu (klik na OK)
  */
void ConnectDialog::accept(){

    // data od uzivatele posle objektu communicator, ktery se zkusi pripojit
    communicator.connect(ui->server->text(),ui->port->text());
    QString message;
    if (communicator.login(ui->user->text(),ui->password->text(),message))
        this->hide();
    //a pokud nejde, vyhodi chybu
    else
        QMessageBox::critical(this, "Error", message);
}

/**
  * slot na registraci klienta
  */
void ConnectDialog::registerUser()
{
    // obdobne jako accept()
    communicator.connect(ui->server->text(),ui->port->text());
    QString message;

    if (communicator.registerUser(ui->user->text(),ui->password->text(),message))
        this->hide();
    else
        QMessageBox::critical(this, "Error", message);
}
