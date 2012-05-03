#ifndef PNSIMTHREAD_H
#define PNSIMTHREAD_H

#include <QThread>
#include <QTcpSocket>

class PNSimThread : public QThread
{
    Q_OBJECT
public:
    explicit PNSimThread(int socketDescriptor, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    int socketDescriptor;
    quint16 blockSize;
    QTcpSocket *commSock;

    void handleCommand(QString command);
private slots:
    void readCommand();
};

#endif // PNSIMTHREAD_H
