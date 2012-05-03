#include "pnsimthread.h"
#include <QDebug>
#include <QtNetwork>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

PNSimThread::PNSimThread( int socketDescriptor, QObject *parent) :
    QThread(parent), socketDescriptor(socketDescriptor)
{
}

void PNSimThread::run()
{
    QTcpSocket tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }

    QString login="<login name=\"hurr\"/>";
    QXmlStreamReader client(login);

    while (!client.atEnd()) {
        client.readNext();
        qDebug() << "nacitam";
        qDebug() << client.name();
    }
    if (client.hasError()) {
        qCritical() << "Something went wrong during reading XML";
        return;
    }
}
