#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

namespace Ui {
class editDialog;
}

class editDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit editDialog(QWidget *parent = 0);
    ~editDialog();
    
private:
    Ui::editDialog *ui;
};

#endif // EDITDIALOG_H
