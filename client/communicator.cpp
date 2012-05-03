#include "communicator.h"

Communicator::Communicator()
{
    commSock = new QTcpSocket;
}

Communicator::Communicator(QString hostname, QString port)
{
    commSock = new QTcpSocket;
    commSock->connectToHost(hostname, port.toUInt(),QIODevice::ReadWrite);
}

Communicator::~Communicator()
{
    delete commSock;
}
