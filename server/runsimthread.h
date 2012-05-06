/**
 * @file runsimthread.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Vlakno, ve kterem bezi samotna simulace.
 */

#ifndef RUNSIMTHREAD_H
#define RUNSIMTHREAD_H

#include <QThread>
#include <QMutex>
#include <QString>
#include "petrisim.h"

/**
 * Trida, vlakno, ve kterem bezi samotna jedna spustena simulace.
 */
class RunSimThread : public QThread
{
    Q_OBJECT
public:
    /**
      * Konstruktor vlakna - nastavi zadane atributy.
      *
      * @param myid id simulace, ktera se zde bude spoustet
      * @param sim ukazatel na simulaci, ktera se ma spustit
      * @param outmutex semafor pro cekani na prebrani hodnoty dokonceni simulace
      * @param outid adresa id, kam se zapise id dokoncene simulace
      * @param run_or_step true pokud run, jinak false
      * @param parent ukazatel na rodice typu qobject
      */
    explicit RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, bool run_or_step, QObject *parent = 0);
    /**
      * Hlavni provadeci metoda vlakna, volana automaticky pri thread->start().
      */
    void run();

    QString myid; ///< id simulace, ktera se zde bude spoustet
    PetriSim *sim; ///< sim ukazatel na simulaci, ktera se ma spustit
    QMutex *outmutex; ///< outmutex semafor pro cekani na prebrani hodnoty dokonceni simulace
    QString *outid; ///< adresa id, kam se zapise id dokoncene simulace
    bool run_or_step; ///< true pokud run, jinak false
signals:
    
public slots:
    
};

#endif // RUNSIMTHREAD_H
