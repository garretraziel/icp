#ifndef LOADSIMDIALOG_H
#define LOADSIMDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class loadSimDialog;
}

class loadSimDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit loadSimDialog(QWidget *parent = 0);
    ~loadSimDialog();
    void pushSim(QString sim);
    
private:
    Ui::loadSimDialog *ui;
    int index;
    QStringList items;

public slots:
    void updateList();
};

#endif // LOADSIMDIALOG_H
