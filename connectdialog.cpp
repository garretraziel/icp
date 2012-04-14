#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <iostream>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::accept(){
    std::cout << "Hurr durr" << std::endl;
    this->hide();
}
