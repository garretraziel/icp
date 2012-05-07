/**
 * @file helpdialog.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog zobrazujici napovedu.
 *
 */

#include "helpdialog.h"
#include "ui_helpdialog.h"

/**
  * Konstruktor vytvarejici helpDialog
  * @param predek pro uklid
  */
helpDialog::helpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpDialog)
{
    ui->setupUi(this);

}

/**
  * Destruktor helpDialog
  */
helpDialog::~helpDialog()
{
    delete ui;
}
