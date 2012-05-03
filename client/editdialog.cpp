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

QRegExp * checkExp(QString pattern, QString str, QString errStr){
    QRegExp * rx = new QRegExp(pattern);
    if(!rx->exactMatch(str)){
        QMessageBox::critical(NULL,errStr.left(14),errStr);
        delete(rx);
        return NULL;
    }
    return rx;
}

void editDialog::accept(){
    switch(sender->primType){
    case TRANS:
        {
            ui->label->setText("Guard");
            QString text = ui->labelEdit->text();
            QStringList parts= text.split("&&");
            ConstraintVector backup =   ((pnRect *)sender)->simTrans->constraints;
            ((pnRect *)sender)->simTrans->constraints.clear();
            foreach(QString part, parts){
                QRegExp * rx = checkExp("(\\s*)(\\S+)(\\s*)(<|>|<=|>=|==|!=)(\\s*)(\\S+)(\\s*)", part,
                                      "Bad guarding cond: repair the guardign codition");
                if(!rx){
                    ((pnRect *)sender)->simTrans->constraints.clear();
                    ((pnRect *)sender)->simTrans->constraints = backup;
                    return;
                }
                ((pnRect *)sender)->simTrans->constraints.push_back(new Constraint(rx->cap(2),0,rx->cap(6)));
                delete(rx);
            }
            sender->label->setPlainText(text);
        }
        break;
    case PLACE:
       {
             ui->label->setText("Tokens");
             QString text = ui->labelEdit->text();
             QStringList parts = text.split(",");
             TokenVector backup = ((pnCircle *)sender)->simPlace->getTokens();
             ((pnCircle *)sender)->simPlace->removeTokens();
             if(!(parts.size()==1 && parts[0]=="")){
                 foreach(QString part, parts){
                     QRegExp * rx = checkExp("(\\s*)(\\d+)(\\s*)", part,
                                             "Bad token list: repair the token list condition");
                     if(!rx){
                         ((pnCircle *)sender)->simPlace->removeTokens();
                         foreach(pntype token, backup){
                               ((pnCircle *)sender)->simPlace->putToken(token);
                         }
                         return;
                     }
                     ((pnCircle *)sender)->simPlace->putToken(rx->cap(2).toInt());
                     delete(rx);
                 }
             }
             sender->label->setPlainText(text);
       }
    }
    this->hide();
}

void editDialog::loadData(pnPrimitive * _sender){
    sender = _sender;
    ui->labelEdit->setText(sender->label->toPlainText());
}
