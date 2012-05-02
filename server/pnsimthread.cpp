#include "pnsimthread.h"
#include <QDebug>
#include <QtNetwork>

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

}
