#include "pnsimthread.h"
#include <QDebug>
#include <QtNetwork>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>

PNSimThread::PNSimThread(int socketDescriptor, QObject *parent) :
    QThread(parent),socketDescriptor(socketDescriptor)
{
    isLogged = false;
}

void PNSimThread::run()
{
    QTcpSocket commSock;
    //commSock = new QTcpSocket(this);
    if (!commSock.setSocketDescriptor(socketDescriptor)) {
        emit error(commSock.error());
        return;
    }
    commSock.waitForConnected(-1);

    bool connected = true;

    quint16 block = 0;
    QDataStream in(&commSock);
    in.setVersion(QDataStream::Qt_4_0);

    while (connected) {
        while (commSock.bytesAvailable() < (int)sizeof(quint16)) {
            commSock.waitForReadyRead(-1);
            if (commSock.state() == QTcpSocket::UnconnectedState) {
                connected = false;
                break;
            }
        }
        if (!connected) break;
        in >> block;
        qDebug() << "velikost: " << block;
        while (commSock.bytesAvailable() < block) {
            commSock.waitForReadyRead(-1);
        }
        QString command;
        in >> command;
        handleCommand(command);
    }
    qDebug() << "odpojeno";
}

void PNSimThread::handleCommand(QString command)
{
    qDebug() << command;
    QXmlStreamReader com(command);
    if (com.readNext() != QXmlStreamReader::StartDocument) {
        qCritical() << "Error: unknown command from client";
        return;
    }
    com.readNext();
    if (com.atEnd() || com.hasError()) {
        qCritical() << "Error: bad command from client";
        return;
    }
    if (isLogged) {
        qDebug() << "hurr!!";
    }
    if (com.name() == "login") {
        qDebug() << "logged";
        isLogged = true;
    }
}
