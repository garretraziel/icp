/**
 * @file aboutdialog.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Dialog, ktery zobrazuje informace o nazvu a autorech programu.
 *
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"


/**
  * Konstruktor vytvarejici aboutDialog
  * @param predek pro uklid
  */
aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
}

/**
  * Destruktor aboutDialog
  */
aboutDialog::~aboutDialog()
{
    delete ui;

}
