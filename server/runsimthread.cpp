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
    //qsrand(time(NULL));
    qsrand(QTime::currentTime().msec());
    qDebug() << "[info] starting: " << myid;
    if (run_or_step) {
        sim->run();
    } else {
        sim->step();
    }
    outmutex->lock();
    (*outid) = myid;
}
