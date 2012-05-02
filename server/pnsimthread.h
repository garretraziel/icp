#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>

class PnsimThread : public QThread
{
    Q_OBJECT
public:
    explicit PnsimThread(QObject *parent = 0);

signals:

public slots:

};

#endif // PNSIMTHREAD_H
