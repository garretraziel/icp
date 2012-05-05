#ifndef LOADSIMDIALOG_H
#define LOADSIMDIALOG_H

#include <QDialog>

namespace Ui {
class loadSimDialog;
}

class loadSimDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit loadSimDialog(QWidget *parent = 0);
    ~loadSimDialog();
    void pushSim(QString name, QString author, QString version, QString info);
    
private:
    Ui::loadSimDialog *ui;
    int index;
};

#endif // LOADSIMDIALOG_H
