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
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    //scene = new QGraphicsScene();
    ui->setupUi(this);
    //ui->graphicsView->setScene(scene);
    //scene->addPixmap(QPixmap("typologo.png"));
    //  QWidget * img = new QWidget(this);
}

aboutDialog::~aboutDialog()
{
    delete ui;

}
