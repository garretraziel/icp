#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class propertiesDialog;
}

class propertiesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit propertiesDialog(QWidget *parent = 0);
    ~propertiesDialog();
    
private:
    Ui::propertiesDialog *ui;
};

#endif // PROPERTIESDIALOG_H
