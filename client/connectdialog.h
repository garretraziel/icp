#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();
    QTcpSocket commSock;
    
private:
    Ui::ConnectDialog *ui;

public slots:
    virtual void accept();
    void login();
};

#endif // CONNECTDIALOG_H
