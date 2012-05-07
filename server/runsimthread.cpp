/**
 * @file runsimthread.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Vlakno, ve kterem bezi samotna simulace.
 */

#include "runsimthread.h"
#include <QDebug>
#include <time.h>
#include <QtGlobal>
#include <QTime>

RunSimThread::RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, bool run_or_step, QObject *parent) :
    QThread(parent),myid(myid), sim(sim), outmutex(outmutex), outid(outid), run_or_step(run_or_step)
{
}

void RunSimThread::run()
{
    qsrand(QTime::currentTime().msec()); //pro kazde vlakno musim volat srand zvlast
    qDebug() << "[info] starting: " << myid;
    if (run_or_step) {
        sim->run(); //chce spustit vse naraz
    } else {
        sim->step(); //chce krokovat
    }
    outmutex->lock(); //pockam az muzu vratit hodnotu
    (*outid) = myid; //vratim hodnotu, zkoncim
}
