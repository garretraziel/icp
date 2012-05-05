#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <vector>

typedef std::vector<QString> StringVector;
typedef std::vector<StringVector> simList;

class Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Communicator(QWidget *parent = 0);
    ~Communicator();

    bool connect(QString hostname, QString port);
    bool connected();
    bool login(QString name, QString password, QString &message);
    bool registerUser(QString name, QString password, QString &message);

    bool saveSimState(QString xmlSimState, QString &message);

    QString userLoggedIn() {return loginName;}

    //bool getSimulations(QStringList & sims);
    bool getSimulations(simList & sims);
    bool loadThis(QString name, QString version);
    bool runSimulation(QString id);

    QString sim;
    QString errorMsg;
    QString simID;

signals:
    void error(QTcpSocket::SocketError socketError);
    void simError();
    void simOk();

private:
    QTcpSocket *commSock;
    bool sendCommand(QString command);
    bool recvCommand(QString &command);
    inline bool login_or_register(QString what, QString name, QString password, QString &message);
    qint64 block;
    bool isNotError(QString & recMessage, QString & message);
    bool handleCommand(QString command);
    QString loginName;
    bool exclusion;

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void handleIncomming();
};

extern Communicator communicator;

#endif // COMMUNICATOR_H
