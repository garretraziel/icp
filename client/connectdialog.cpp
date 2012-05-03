#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <iostream>
#include <QString>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
    socket.close();
}

void ConnectDialog::accept(){
    //toto je fakt moc pekny, priste asi nejakej qstream nebo co

    connect(&socket, SIGNAL(connected()), this, SLOT(sendSomeShit()));

    socket.connectToHost(ui->server->text(), ui->port->text().toUInt(),QIODevice::ReadWrite);

    this->hide();
}

void ConnectDialog::login(){
    /*
    QString msg_hi = "OHAI";

    QString msg_login = "LOGIN/" +      \
            ui->user->text().length() + \
            "/:" + ui->user->text();

    QString msg_password = "PASS/" +        \
            ui->password->text().length() + \
            "/:" + ui->password->text().toStdString();

    socket.write(msg_hi.toStdString().c_str());
    socket.write(msg_login.toStdString().c_str());
    socket.write(msg_password.toStdString().c_str());
*/
}
