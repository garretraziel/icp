#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <iostream>
#include <QString>
#include <QIODevice>

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
    //toto je fakt moc p?kný, priste asi nejakej qstream nebo co
    std::cout << ui->user->text().toStdString() << ":" << ui->password->text().toStdString() << std::endl;
    std::cout << ui->server->text().toStdString() << ":" << ui->port->text().toStdString() << std::endl;

    connect(&socket, SIGNAL(connected()), this, SLOT(sendSomeShit()));

    //socket.connectToHost(ui->server->text(), ui->port->text().toUInt(),QIODevice::ReadWrite);
    socket.connectToHost("127.0.0.1", 11899,QIODevice::ReadWrite);


    this->hide();
}

void ConnectDialog::sendSomeShit(){
    QString msg = ui->user->text() + ":" + ui->password->text();
    //jojo, to je krasa
    std::cout << socket.write(msg.toStdString().c_str()) << std::endl;
}
