/**
 * @file petrinetserver.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida, ktera poskytuje rozhrani TCP serveru. Pro kazdeho noveho uzivatele vytvori novy thread.
 */

#ifndef PETRINETSERVER_H
#define PETRINETSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include "pnsimthread.h"

class QTcpServer;
class QTcpSocket;

/**
 * Trida, ktera poskytuje rozhrani TCP serveru. Pro kazdeho noveho uzivatele vytvori novy thread. Dedi od QTcpServer.
 */
class PetriNetServer : public QTcpServer
{
    Q_OBJECT
private:
    QHostAddress my_ip; ///< nastavi se na IP adresu spusteneho serveru
    int port; ///< port na kterem server bezi
    QMutex *iomutex; ///< semafor pro pristup k souborum

public:
    /**
      * Konstruktor tridy, vytvori server na zadanem portu, nezacne naslouchat.
      *
      * @param parent ukazatel na rodicovskou tridu typu qobject
      * @param maxconnections kolik soubeznych pripojeni je server schopen zvladnout, velikost fronty
      * @param port port, na kterem bude server naslouchat
      */
    explicit PetriNetServer(QObject *parent = 0, int maxconnections = 50, int port = 0);
    /**
      * Destruktor tridy, smaze semafor.
      */
    ~PetriNetServer();
    /**
      * Server zacne naslouchat na zadanem portu.
      *
      * @return true pokud se poradi naslouchat, false pokud ne
      */
    bool start();

protected:
    /**
      * Funkce ktera je automaticky spustena pri prichozim pripojeni. Vytvori novy thread pro obsluhu.
      *
      * @param socketDescriptor popisovat socketu, na kterem ma thread komunikovat.
      */
    void incomingConnection(int socketDescriptor);
};

#endif // PETRINETSERVER_H
