/**
 * @file aboutdialog.h
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

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(registerUser()));
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::accept(){
    //toto je fakt moc pekny, priste asi nejakej qstream nebo co


    /*connect(&commSock, SIGNAL(connected()), this, SLOT(login()));

    commSock.connectToHost(ui->server->text(), ui->port->text().toUInt(),QIODevice::ReadWrite);*/

    communicator.connect(ui->server->text(),ui->port->text());
    QString message;
    if (communicator.login(ui->user->text(),ui->password->text(),message))
        this->hide();
    else
        QMessageBox::critical(this, "Error", message);
}

void ConnectDialog::registerUser()
{
    communicator.connect(ui->server->text(),ui->port->text());
    QString message;

    if (communicator.registerUser(ui->user->text(),ui->password->text(),message))
        this->hide();
    else
        QMessageBox::critical(this, "Error", message);
}
