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
    communicator.login(ui->user->text(),ui->password->text());
    if (communicator.connected())
        this->hide();
    else
        QMessageBox::critical(this, "Error", "Error occured during connecting to server.");
}
