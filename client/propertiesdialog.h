/**
 * @file prepertiesdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro nastaveni nazvu a informace u siti.
 *
 * Vygeneroval automaticky Qt Creator
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
    /**
      * Konstruktor vytvarejici propertiesDialog
      * @param predek pro uklid
      */
    explicit propertiesDialog(QWidget *parent = 0);
    /**
      * Destruktor propertiesDialog
      */
    ~propertiesDialog();

    /**
      * Nacte data do dialogu
      * @param name jmeno simulace
      * @param author jmeno autora
      * @param version verze
      * @param info informace o simulaci
      */
    void loadData(QString name, QString author, QString version, QString info);
    
private:
    Ui::propertiesDialog *ui; /// user interface

public slots:
    /**
      * Slot volany po prijeti dialogu (klik na OK)
      */
    void accept();
};

#endif // PROPERTIESDIALOG_H
