#include "communicator.h"
#include <QDataStream>
#include <QByteArray>

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

bool Communicator::login(QString name, QString password)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    QString message = "<login name=\"";
    message += name;
    message += "\" password=\"";
    message += password;
    message += "\"/>";
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    qDebug() << "velikost: " << (quint16)(block.size() - sizeof(quint16));
    commSock->write(block);
    return true;
}
