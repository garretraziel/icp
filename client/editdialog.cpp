#include "editdialog.h"
#include "ui_editdialog.h"
#include "pngui.h"

editDialog::editDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editDialog)
{
    ui->setupUi(this);
}

editDialog::~editDialog()
{
    delete ui;
}

void editDialog::accept(){
    sender->label->setPlainText(ui->labelEdit->text() );
    this->hide();
}

void editDialog::loadData(pnPrimitive * _sender){
    sender = _sender;
    ui->labelEdit->setText(sender->label->toPlainText());
}
