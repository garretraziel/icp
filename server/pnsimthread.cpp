#include "pnsimthread.h"
#include <QDebug>
#include <QtNetwork>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>

PNSimThread::PNSimThread( int socketDescriptor, QObject *parent) :
    QThread(parent), socketDescriptor(socketDescriptor)
{
    commSock = new QTcpSocket(this);
    connect(commSock,SIGNAL(readyRead()),this,SLOT(readCommand()));
    connect(commSock,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void PNSimThread::run()
{
    if (!commSock->setSocketDescriptor(socketDescriptor)) {
        emit error(commSock->error());
        return;
    }
    blockSize = 0;
}

void PNSimThread::readCommand()
{
    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);
    if (blockSize == 0) {
        if (commSock->bytesAvailable() < (int)sizeof(quint16)) return;
        in >> blockSize;
    }

    if (commSock->bytesAvailable() < blockSize) return;

    QString command;
    in >> command;

    handleCommand(command);
}

void PNSimThread::handleCommand(QString command)
{
    qDebug() << command;
}
