#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpSocket>
#include <QString>

class Communicator
{
public:
    Communicator();
    Communicator(QString hostname, QString port);
    ~Communicator();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QTcpSocket *commSock;
    void login();
};

#endif // COMMUNICATOR_H
