/**
 * @file communicator.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Trida, obstaravajici komunikaci se serverem
 *
 */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <vector>

typedef std::vector<QString> StringVector;
typedef std::vector<StringVector> simList;

/**
 *
 * Trida zajistujici komunikaci mezi klientem a serverem
 *
 */
class Communicator : public QObject
{
    Q_OBJECT
public:
    /**
      * Konstruktor vytvarejici Communicator
      * @param predek pro uklid
      */
    explicit Communicator(QWidget *parent = 0);

    /**
      * Destruktor Communicator
      */
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
    bool runSimulation(QString id, bool run_or_step);

    QString sim;
    QString errorMsg;
    QString simID;

    void blockSocket(bool b);

signals:
    void error(QTcpSocket::SocketError socketError);
    void simError();
    void simOk();

private:
    QTcpSocket *commSock;
    bool sendCommand(QString command);
    bool recvCommand(QString &command);
    inline bool login_or_register(QString what, QString name, QString password, QString &message);
    quint32 block;
    bool isNotError(QString & recMessage, QString & message);
    bool handleCommand(QString command);
    QString loginName;
    bool exclusion;

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void handleIncomming();
    void setOffline();
};

extern Communicator communicator;

#endif // COMMUNICATOR_H
