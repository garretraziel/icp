#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <vector>
#include <QMap>
#include "petrisim.h"
#include <QMutex>
#include <QMutexLocker>

typedef std::vector<PetriSim *> SimVector;
typedef QMap<QString, QString> StrToStrMap;

class PNSimThread : public QThread
{
    Q_OBJECT
public:
    explicit PNSimThread(int socketDescriptor, QObject *parent = 0);
    ~PNSimThread();

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private slots:
    void readIncoming();
    void handleDisconnection();

private:
    int socketDescriptor;
    int isLogged;
    QString usersFile;
    QString logFile;
    QString simDirectory;
    QTcpSocket *commSock;
    qint64 block;
    SimVector simulations;

    bool handleCommand(QString command,QString &message);
    int logUser(QString login, QString password);
    QByteArray createMessage(QString message);
    int registerUser(QString login, QString password);
    QString getSimulations();
    bool getCommand(QString xml, QString &result, StrToStrMap &args);
    bool loadSim(QString name, int version);
};

#endif // PNSIMTHREAD_H
