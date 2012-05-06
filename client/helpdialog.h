/**
 * @file helpdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog zobrazujici napovedu.
 *
 */

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
class helpDialog;
}


/**
 *
 * Trida zastitujici zobrazeni a praci s "Help dialogem"
 *
 */
class helpDialog : public QDialog
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici helpDialog
      * @param predek pro uklid
      */
    explicit helpDialog(QWidget *parent = 0);

    /**
      * Destruktor helpDialog
      */
    ~helpDialog();
    
private:
    Ui::helpDialog *ui;///user interface
};

#endif // HELPDIALOG_H
