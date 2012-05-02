#include "editdialog.h"
#include "ui_editdialog.h"
#include "pngui.h"
#include <QString>
#include <QStringList>
#include <QRegExp>
#include "../server/constraint.h"

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
    QString text = ui->labelEdit->text();
    QStringList parts= text.split("&&");
    foreach(QString part, parts){
        //QString rel = (part.split(QRegExp("\\s")))[1];
        QRegExp rx("(\\s*)(\\S+)(\\s*)(<|>|<=|>=|==|!=)(\\s*)(\\S+)(\\s*)");
        if(!rx.exactMatch(part)){
            QMessageBox::critical(this,tr("Wrong guard"),tr("Correct the guarding condition"));
            return;
        }
        //TODO...
        ((pnRect *)sender)->simTrans->constraints.push_back(new Constraint(rx.cap(2),0,rx.cap(6)));

    }
    sender->label->setPlainText(text);
    this->hide();
}

void editDialog::loadData(pnPrimitive * _sender){
    sender = _sender;
    ui->labelEdit->setText(sender->label->toPlainText());
}
