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
    bool sendCommand(QString command);
    bool login(QString name, QString password);

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QTcpSocket *commSock;
};

static Communicator communicator; // singleton nefunguje

#endif // COMMUNICATOR_H
