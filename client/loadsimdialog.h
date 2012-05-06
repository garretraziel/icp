/**
 * @file loadsimdialog.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog pro zobrazeni, vyhledani a nacetni siti ze serveru
 *
 */


#ifndef LOADSIMDIALOG_H
#define LOADSIMDIALOG_H

#include <QDialog>
#include <QStringList>
#include "communicator.h"


namespace Ui {
class loadSimDialog;
}


/**
 *
 * Trida zastitujici zobrazeni a praci s "Load Simulation dialogem"
 *
 */
class loadSimDialog : public QDialog
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici loadSimDialog
      * @param predek pro uklid
      */
    explicit loadSimDialog(QWidget *parent = 0);

    /**
      * Destruktor loadSimDialog
      */
    ~loadSimDialog();
    void pushSim(StringVector sim);
    
private:
    Ui::loadSimDialog *ui;///user interface
    int index; ///index do seznamu simulaci
    simList items; ///seznam simulaci

public slots:
    void updateList();
    void accept();
    void clearSims();
};

#endif // LOADSIMDIALOG_H
