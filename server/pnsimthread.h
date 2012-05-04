#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <vector>
#include <map>
#include "petrisim.h"

typedef std::vector<PetriSim> SimVector;
typedef std::map<QString, QString> StrToStrMap;

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
    QString usersFile;
    QString logFile;
    QString simDirectory;

    SimVector simulations;

    bool handleCommand(QString command,QString &message);
    int logUser(QString login, QString password);
    QByteArray createMessage(QString message);
    int registerUser(QString login, QString password);
    QString getSimulations();
    bool getCommand(QString xml, QString &result, StrToStrMap &args);
};

#endif // PNSIMTHREAD_H
