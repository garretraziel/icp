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
    bool sendCommand(QString command);
    bool login(QString name, QString password);

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    QTcpSocket *commSock;

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
};

extern Communicator communicator;

#endif // COMMUNICATOR_H
