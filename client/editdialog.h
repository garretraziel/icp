/**
 * @file aboutdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog na editaci vlastnosti casti site.
 *
 */

#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class editDialog;
}

/**
 *
 * Forward definice
 *
 */
class pnPrimitive;


/**
 *
 * Trida zastitujici zobrazeni a praci s "Edit dialogem"
 *
 */
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
