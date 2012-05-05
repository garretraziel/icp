#ifndef RUNSIMTHREAD_H
#define RUNSIMTHREAD_H

#include <QThread>
#include <QMutex>
#include <QString>
#include "petrisim.h"

class RunSimThread : public QThread
{
    Q_OBJECT
public:
    explicit RunSimThread(QString myid, PetriSim *sim, QMutex *outmutex, QString *outid, QObject *parent = 0);
    void run();

    QString myid;
    PetriSim *sim;
    QMutex *outmutex;
    QString *outid;
signals:
    
public slots:
    
};

#endif // RUNSIMTHREAD_H
