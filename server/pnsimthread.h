#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QMap>
#include "petrisim.h"
#include <QMutex>
#include <vector>

typedef QMap<int, PetriSim *> SimVector;
typedef QMap<QString, QString> StrToStrMap;
typedef std::vector<QString> QStringVector;

class PNSimThread : public QThread
{
    Q_OBJECT
public:
    explicit PNSimThread(int socketDescriptor, QMutex *iomutex, QObject *parent = 0);
    ~PNSimThread();

    void run();

    QMutex *simmutex;
    QString *outid;
    QMutex *iomutex;


signals:
    void error(QTcpSocket::SocketError socketError);

private slots:
    void readIncoming();
    void handleDisconnection();
    void handleSimuled();

private:
    int socketDescriptor;
    int isLogged;
    QString userName;
    QString usersFile;
    QString logFile;
    QString simDirectory;
    QTcpSocket *commSock;
    quint32 block;
    SimVector simulations;
    QStringVector idsToSend;
    int maxid;

    bool handleCommand(QString command,QString &message);
    int logUser(QString login, QString password);
    QByteArray createMessage(QString message);
    int registerUser(QString login, QString password);
    QString getSimulations();
    bool getCommand(QString xml, QString &result, StrToStrMap &args);
    QString loadSim(QString name, QString version);
    bool saveSimulation(QString xml);
    void runSimulation(QString id, bool run_or_step);
    void logRun(QString name, QString version, QString user);
};

#endif // PNSIMTHREAD_H
