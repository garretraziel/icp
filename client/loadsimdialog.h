#ifndef LOADSIMDIALOG_H
#define LOADSIMDIALOG_H

#include <QDialog>
#include <QStringList>
#include "communicator.h"


namespace Ui {
class loadSimDialog;
}

class loadSimDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit loadSimDialog(QWidget *parent = 0);
    ~loadSimDialog();
    void pushSim(StringVector sim);
    
private:
    Ui::loadSimDialog *ui;
    int index;
    simList items;

public slots:
    void updateList();
    void accept();
};

#endif // LOADSIMDIALOG_H
