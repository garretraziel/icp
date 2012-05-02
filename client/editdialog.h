#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

namespace Ui {
class editDialog;
}

class pnPrimitive;

class editDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit editDialog(QWidget *parent = 0);
    ~editDialog();
    void loadData(pnPrimitive * sender);
private:
    Ui::editDialog *ui;
    pnPrimitive * sender;
public slots:
    virtual void accept();
};

#endif // EDITDIALOG_H
