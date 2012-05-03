#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <vector>
#include "petrisim.h"

typedef std::vector<PetriSim> SimVector;

class PNSimThread : public QThread
{
    Q_OBJECT
public:
    explicit PNSimThread(int socketDescriptor, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    int socketDescriptor;
    int isLogged;

    SimVector simulations;

    void handleCommand(QString command);
};

#endif // PNSIMTHREAD_H
