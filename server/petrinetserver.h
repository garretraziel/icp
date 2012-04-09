#ifndef PETRINETSERVER_H
#define PETRINETSERVER_H

#include <QTcpServer>

class PetriNetServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PetriNetServer(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // PETRINETSERVER_H
