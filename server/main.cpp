/**
 * @file main.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Soubor s hlavni funkci main aplikace.
 * Vytvori se server, necha se naslouchat na portu 11899 a spusti se event-loop Qt.
 */

#include <QtCore/QCoreApplication>
#include "petrinetserver.h"
#include "petrisim.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define PORTNUMBER 11899

/**
  * Hlavni funkce serveru aplikace.
  *
  * Vytvori se QCoreApplication, spusti server a pote spusti event-loop Qt.
  *
  * @param argc pocet argumentu - automaticky zadan z konzole
  * @param argv pole argumenty - automaticky zadan z konzole
  *
  * @return 0 pri spravnem ukonceni, 1 jinak
  */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    PetriNetServer server(0,50,PORTNUMBER); //spusti server na zadanem portu

    bool is_running = server.start(); //zacne naslouchat

    if (!is_running) {
        qCritical() << "Cannot start server\n";
        return 1;
    }

    return a.exec(); //spusti event-loop
}
