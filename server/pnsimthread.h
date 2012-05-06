/**
 * @file pnsimthread.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida pro vlakno, ktere komunikuje s uzivatelem a spousti simulace.
 */

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

/**
 * Vlakno, ve kterem probiha veskera komunikace s uzivatelem, uchovava simulace
 * a spousti je.
 */
class PNSimThread : public QThread
{
    Q_OBJECT
public:
    /**
      * Konstruktor tridy thread, naplnuje implicitnimi hodnotami.
      *
      * @param socketDescriptor popisovac socketu, kterym bude thread komunikovat
      * @param iomutex semafor pro pristup k souborum
      * @param parent ukazatel na predka typu qobject
      */
    explicit PNSimThread(int socketDescriptor, QMutex *iomutex, QObject *parent = 0);
    /**
      * Destruktor vlakna, smaze simulace.
      */
    ~PNSimThread();

    /**
      * Metoda, ktera se vola automaticky po spusteni vlakna pomoci start().
      */
    void run();

    QMutex *simmutex; ///< semafor pro pristup k simulacim
    QString *outid; ///< id ktere vraci vlakno po dokonceni simulace
    QMutex *iomutex; ///< semafor pro pristup k souborum


signals:
    /**
      * Signal, ktery se emituje pri chybe na socketu.
      *
      * @param socketError typ erroru socketu
      */
    void error(QTcpSocket::SocketError socketError);

private slots:
    /**
      * Slot, ktery se spusti se signalem readyRead() socketu
      */
    void readIncoming();
    /**
      * Slot, ktery se spusti se signalem odpojeni socketu.
      */
    void handleDisconnection();
    /**
      * Slot, ktery se spusti se signalem dokonce threadu, ktery simuluje simulaci.
      */
    void handleSimuled();

private:
    int socketDescriptor; ///< descriptor komunikacniho socketu
    int isLogged; ///< udava, zda je uzivatel prihlaseny
    QString userName; ///< uklada jmeno uzivatele
    QString usersFile; ///< soubor s prihlasovacimy udaji uzivatelu
    QString logFile; ///< soubor s informacemi o spustenych simulacich
    QString simDirectory; ///< adresar s ulozenymi simulacemi
    QTcpSocket *commSock; ///< komunikacni socket
    quint32 block; ///< blok dat, co se maji nacist
    SimVector simulations; ///< vektor otevrenych simulaci
    QStringVector idsToSend; ///< vektor simulaci, co jsou dokoncene a musi se odeslat
    int maxid; ///< nejvyssi ID pro identifikaci siti

    /**
      * Postara se o prijaty prikaz.
      *
      * @param command prijaty prikaz
      * @param message zprava, ktera se zapise zpet uzivateli
      *
      * @return true, pokud smi zustat klient pripojen, jinak false
      */
    bool handleCommand(QString command,QString &message);
    /**
      * Pokusi se prihlasit uzivatele.
      *
      * @param login jmeno uzivatele
      * @param password heslo uzivatele
      *
      * @return 0 pri spravnem prihlaseni, jine pri chybe
      */
    int logUser(QString login, QString password);
    /**
      * Vytvori zpravu vhodnout pro poslani.
      *
      * @param message zprava, co se ma poslat
      *
      * @return bytove pole vhodne k poslani
      */
    QByteArray createMessage(QString message);
    /**
      * Zaregistruje noveho uzivatele.
      *
      * @param login jmeno uzivatele
      * @param password heslo uzivatele
      *
      * @return 0 pri spravnem zaregistrovano, jine pri chybe
      */
    int registerUser(QString login, QString password);
    /**
      * Najde ve slozce vsechny simulace a vrati je v XML.
      *
      * @return XML se simulacemi
      */
    QString getSimulations();
    /**
      * Proparsuje vstupni XML prikaz, vysledek ulozi do result, argumenty do args.
      *
      * @param xml vstupni XML prikaz
      * @param result zde se ulozi prikaz
      * @param args slovnik argumentu a jejich hodnot
      *
      * @return false pri chybe, jinak true
      */
    bool getCommand(QString xml, QString &result, StrToStrMap &args);
    /**
      * Nacte simulaci, ulozi do seznamu simulaci.
      *
      * @param name jmeno otevirane simulace
      * @param version verze otevirane simulace
      *
      * @return XML se simulaci
      */
    QString loadSim(QString name, QString version);
    /**
      * Ulozi zadanou simulaci.
      *
      * @param xml XML se simulaci k ulozeni
      *
      * @return true pokud se vse ulozilo v poradku, jinak false
      */
    bool saveSimulation(QString xml);
    /**
      * Spusti zadanou simulaci v novem threadu.
      *
      * @param id ID simulace, ktera se ma spustit
      * @param run_or_step true pokud se ma dat run, jinak false
      */
    void runSimulation(QString id, bool run_or_step);
    /**
      * Zapise do souboru s logem informaci o nacteni simulace.
      *
      * @param name jmeno simulace
      * @param version verze simulace
      * @param user jmeno uzivatele, ktery simulaci spustil
      */
    void logRun(QString name, QString version, QString user);
};

#endif // PNSIMTHREAD_H
