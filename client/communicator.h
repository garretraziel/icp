#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpSocket>
#include <QString>

class Communicator
{
public:
    Communicator();
    ~Communicator();

    bool connect(QString hostname, QString port);

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QTcpSocket *commSock;
    void login(QString name, QString password);
};

static Communicator communicator; // singleton nefunguje

#endif // COMMUNICATOR_H
