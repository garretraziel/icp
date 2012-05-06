/**
 * @file aboutdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro nastaveni nazvu a informace u siti.
 *
 */

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class propertiesDialog;
}

/**
 *
 * Trida zastitujici zobrazeni a praci s "Properties dialogem"
 *
 */
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
