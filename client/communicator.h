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
      * @param parent predek pro uklid
      */
    explicit Communicator(QWidget *parent = 0);

    /**
      * Destruktor Communicator
      */
    ~Communicator();

    /**
      * Pripoji k serveru
      * @param hostname hostname nebo IP
      * @param port port, na kterem server nasloucha
      * @return true pri uspechu
      */
    bool connect(QString hostname, QString port);

    /**
      * Zjisteni, zda je klient pripojen k serveru
      * @return true pokud je pripojen k serveru
      */
    bool connected();

    /**
      * Zaloguje uzivatele
      * @param name uzivatelske jmeno
      * @param password heslo
      * @param message navratova informacni zprava
      * @return true pri uspechu
      */
    bool login(QString name, QString password, QString &message);

    /**
      * Zaregistruje uzivatele
      * @param name uzivatelske jmeno
      * @param password heslo
      * @param message navratova informacni zprava
      * @return true pri uspechu
      */
    bool registerUser(QString name, QString password, QString &message);

    /**
      * Ulozi simulaci na server
      * @param XmlSimState retezec obsahujici XML stav simulace
      * @param message navratova informacni zprava
      * @return true pri uspechu
      */
    bool saveSimState(QString xmlSimState, QString &message);

    /**
      * Zjisteni jmena zalogovaneho uzivatele
      * @return Username zalogovaneho uzivatele, jinak prazdny retezec
      */
    QString userLoggedIn() {return loginName;}

    /**
      * Ziskani simulaci
      * @param sims reference na seznam simulaci
      * @return true pri uspechu
      */
    bool getSimulations(simList & sims);

    /**
      * Nacita ze serveru danou simulaci
      * @param name nazev simulace
      * @param version verze simulace
      * @return true pri uspechu
      */
    bool loadThis(QString name, QString version);

    /**
      * Spusti simulaci
      * @param id unikatni ID (ziskane od serveru)
      * @param run_or_step true pokud run, false pokud step
      * @return true pri uspechu
      */
    bool runSimulation(QString id, bool run_or_step);

    QString sim;        /// specialni navratova hodnota
    QString errorMsg;   /// specialni navratova hodnota (chybova zprava)
    QString simID;      /// specialni navratova hodnota (id simulace)

    /**
      * Blokuje signaly se socketu
      * @param b true zapne blokovani, false vypne
      */
    void blockSocket(bool b);

signals:
    /**
      * Error signal pri chybe
      * @param typ socket erroru
      */
    void error(QTcpSocket::SocketError socketError);

    /**
      * Error simulace (pri komunikaci se serverem)
      */
    void simError();

    /**
      * Simulace OK (pri komunikaci se serverem)
      */
    void simOk();

private:

    QTcpSocket *commSock; /// komunikacni socket

    /**
      * Posle prikaz serveru
      * @param command prikaz
      * @return true pri uspechu
      */
    bool sendCommand(QString command);

    /**
      * Ziska prikaz poslany ze serveru
      * @param command reference na ziskany prikaz
      * @return true pri uspechu
      */
    bool recvCommand(QString &command);

    /**
      * Zaregistrue nebo zaloguje uzivatele
      * @param what kterou akci provest ("login", "register")
      * @param name jmeno uzivatele
      * @param password heslo
      * @param message reference na informacni zpravu
      * @return true pri uspechu
      */
    inline bool login_or_register(QString what, QString name, QString password, QString &message);

    quint32 block; /// velikost nacitane zpravy od serveru

    /**
      * Zkontroluje, zda zprava od serveru neni error
      * @param recMessage reference na zpravu ziskanou od serveru
      * @param message reference na informacni zpravu
      * @return true, pokud neni error
      */
    bool isNotError(QString & recMessage, QString & message);

    /**
      * Zpracuje prikaz prijaty od serveru
      * @param command prikaz
      * @return true pri uspechu
      */
    bool handleCommand(QString command);

    QString loginName; /// jmeno zalogovaneho uzivatele


private slots:
    /**
      * Slot na zobrazeni chybove hlasky pri chybe socketu
      * @param socketError typ chyby socketu
      */
    void displayError(QAbstractSocket::SocketError socketError);

    /**
      * Slot na zpracovani prichozi zpravy
      */
    void handleIncomming();

    /**
      * Slot na nastaveni offline hlasky v statusbaru mainwindow
      */
    void setOffline();
};

extern Communicator communicator; /// staticka instance tridy Communicator

#endif // COMMUNICATOR_H
