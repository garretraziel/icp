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
    bool connected();
    bool login(QString name, QString password, QString &message);
    bool registerUser(QString name, QString password, QString &message);

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QTcpSocket *commSock;
    bool sendCommand(QString command);
    bool recvCommand(QString &command);

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
};

extern Communicator communicator;

#endif // COMMUNICATOR_H
