#include "runsimthread.h"
#include <QDebug>

RunSimThread::RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, bool run_or_step, QObject *parent) :
    QThread(parent),myid(myid), sim(sim), outmutex(outmutex), outid(outid), run_or_step(run_or_step)
{
}

void RunSimThread::run()
{
    qDebug() << "[info] starting: " << myid;
    if (run_or_step) {
        sim->run();
    } else {
        sim->step();
    }
    outmutex->lock();
    (*outid) = myid;
}
