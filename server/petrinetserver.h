#ifndef PETRINETSERVER_H
#define PETRINETSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "pnsimthread.h"

class QTcpServer;
class QTcpSocket;

class PetriNetServer : public QTcpServer
{
    Q_OBJECT
private:
    QHostAddress my_ip;
    int port;

public:
    explicit PetriNetServer(QObject *parent = 0, int maxconnections = 50, int port = 0);
    ~PetriNetServer();
    bool start();

protected:
    void incomingConnection(int socketDescriptor);
};

#endif // PETRINETSERVER_H
