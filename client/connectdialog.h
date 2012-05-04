#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();
    
private:
    Ui::ConnectDialog *ui;

public slots:
    virtual void accept();
private slots:
    void registerUser();
};

#endif // CONNECTDIALOG_H
