#include "runsimthread.h"
#include <QDebug>

RunSimThread::RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, bool run, QObject *parent) :
    QThread(parent),myid(myid), sim(sim), outmutex(outmutex), outid(outid), run_or_step(run_or_step)
{
}

void RunSimThread::run()
{
    qDebug() << "spoustim: " << myid;
    if (run_or_step) {
        sim->run();
    } else {
        sim->step();
    }
    outmutex->lock();
    (*outid) = myid;
}
