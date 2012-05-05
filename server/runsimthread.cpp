#include "runsimthread.h"
#include <QDebug>

RunSimThread::RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, QObject *parent) :
    QThread(parent),myid(myid), sim(sim), outmutex(outmutex), outid(outid)
{
}

void RunSimThread::run()
{
    qDebug() << "spoustim: " << myid;
    sim->run();
    outmutex->lock();
    (*outid) = myid;
}
