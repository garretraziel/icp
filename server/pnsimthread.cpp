#include "pnsimthread.h"

PNSimThread::PNSimThread( int socketDescriptor, QObject *parent) :
    QThread(parent), socketDescriptor(socketDescriptor)
{
}

void PNSimThread::run()
{

}
