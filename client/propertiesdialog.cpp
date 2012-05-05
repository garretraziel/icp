#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

propertiesDialog::propertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::propertiesDialog)
{
    ui->setupUi(this);
}

propertiesDialog::~propertiesDialog()
{
    delete ui;
}
