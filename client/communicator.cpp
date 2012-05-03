#include "communicator.h"

Communicator::Communicator()
{
}

Communicator::~Communicator()
{
    delete commSock;
}

bool Communicator::connect(QString hostname, QString port)
{
    commSock = new QTcpSocket;
    commSock->connectToHost(hostname, port.toUInt(),QIODevice::ReadWrite);
    return true;
}
