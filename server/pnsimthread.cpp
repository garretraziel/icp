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
        }
        in >> block;
        qDebug() << "velikost: " << block;
        while (commSock.bytesAvailable() < block) {
            commSock.waitForReadyRead(-1);
        }
        QString command;
        in >> command;
        handleCommand(command);
    }
}

void PNSimThread::handleCommand(QString command)
{
    qDebug() << command;
}
