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

    void loadData(QString name, QString author, QString version, QString info);
    
private:
    Ui::propertiesDialog *ui;

public slots:
    void accept();
};

#endif // PROPERTIESDIALOG_H
