/**
 * @file aboutdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog, ktery zobrazuje informace o nazvu a autorech programu.
 *
 * Vygeneroval automaticky Qt Creator
 *
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class aboutDialog;
}

/**
 *
 * Trida zastitujici zobrazeni a praci s "About dialogem"
 *
 */
class aboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici aboutDialog
      * @param predek pro uklid
      */
    explicit aboutDialog(QWidget *parent = 0);
    /**
      * Destruktor aboutDialog
      */
    ~aboutDialog();
    
private:
    Ui::aboutDialog *ui; ///< user interface
};

#endif // ABOUTDIALOG_H
