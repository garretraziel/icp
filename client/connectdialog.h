/**
 * @file connectdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro pripojeni k serveru
 *
 * Vygeneroval automaticky Qt Creator (nektere metody dopsany rucne)
 *
 */

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectDialog;
}

/**
 *
 * Trida zastitujici zobrazeni a praci s "Connect dialogem"
 *
 */
class ConnectDialog : public QDialog
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici ConnecDialog
      * @param predek pro uklid
      */
    explicit ConnectDialog(QWidget *parent = 0);

    /**
      * Destruktor ConnectDialog
      */
    ~ConnectDialog();
    
private:
    Ui::ConnectDialog *ui; ///< user interface

public slots:
    /**
      * slot pri prijeti dialogu (klik na OK)
      */
    virtual void accept();

private slots:
    /**
      * slot na registraci klienta
      */
    void registerUser();
};

#endif // CONNECTDIALOG_H
