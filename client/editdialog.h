/**
 * @file editdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog na editaci vlastnosti casti site.
 *
 * Vygeneroval automaticky Qt Creator
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
    /**
      * Konstruktor vytvarejici editDialog
      * @param predek pro uklid
      */
    explicit editDialog(QWidget *parent = 0);

    /**
      * Destruktor editDialog
      */
    ~editDialog();

    /**
      * nacte data do dialogu
      * @param sender ukazatel na prvek, ktery nastaveni zavolal
      */
    void loadData(pnPrimitive * sender);

private:
    Ui::editDialog *ui;   /// user interface
    pnPrimitive * sender; /// odesilatel pozadavku na editaci
public slots:
    /**
      * Slot volany pri prijeti dialogu (klik na OK)
      */
    virtual void accept();
};

#endif // EDITDIALOG_H
