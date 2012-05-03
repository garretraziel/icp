#include "editdialog.h"
#include "ui_editdialog.h"
#include "pngui.h"
#include <QString>
#include <QStringList>
#include <QRegExp>
#include "../server/constraint.h"
#include <map>
#include <iostream>

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
    ui->label_2->hide(); //WTF, proc toto nejede!
    ui->funcEdit->hide();
    switch(sender->primType){
    case TRANS:
    {
        ui->label_2->show();
        ui->funcEdit->show();

        std::map<QString,int> toRel;

        toRel["<"] = OP_LESS;
        toRel["<="] = OP_LESSEQ;
        toRel[">="] = OP_MOREEQ;
        toRel[">"] = OP_MORE;
        toRel["=="] = OP_EQ;
        toRel["!="] = OP_NOTEQ;

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

            ((pnRect *)sender)->simTrans->constraints.push_back(new Constraint(rx->cap(2),toRel[rx->cap(4)],rx->cap(6)));
            delete(rx);
        }
        QString funcText = ui->funcEdit->text();
        QStringList funcs = funcText.split(";");
        OutputOperations opBackup = ((pnRect *)sender)->simTrans->operations;
        ((pnRect *)sender)->simTrans->operations.clear();
        foreach(QString func, funcs){
            QRegExp * rx = checkExp("(\\s*)(\\S+)(\\s*=)(.*)", func,
                                    "Bad operations: repair the output operations");
            if(!rx){
                ((pnRect *)sender)->simTrans->operations.clear();
                ((pnRect *)sender)->simTrans->operations= opBackup;
                return;
            }
            QString output = rx->cap(2);

            int pos = 0;
            OperationVector operations;
            QRegExp rxNext("(\\s*)(\\+|\\-|)(\\s*)([^\\s\\+\\-]+)");
            while((pos = rxNext.indexIn(rx->cap(4), pos)) != -1){
                Operations op = (rxNext.cap(2)=="-")? SUB : ADD;
                operations.push_back(Operation(op,rxNext.cap(4)));
                pos += rxNext.matchedLength();
            }
            ((pnRect *)sender)->simTrans->operations.push_back(OneOut(operations,output));
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
        break;
    case EDGE:
    {
        ui->label->setText("Variable");
#define startPoint ((pnLine *)sender)->start
#define endPoint ((pnLine *)sender)->end
        if(((pnLine *)sender)->start->primType == TRANS){
            ((pnRect *)startPoint)->simTrans->out_names[ui->labelEdit->text()]
                    = ((pnCircle *)(endPoint))->simPlace;
        }else {
            ((pnRect *)endPoint)->simTrans->in_names[ui->labelEdit->text()]
                    = ((pnCircle *)(startPoint))->simPlace;
        }
        //TODO, osetrit tvar
        sender->label->setPlainText(ui->labelEdit->text());
#undef startPoint
#undef endPoint
    }
    }
    this->hide();
}

void editDialog::loadData(pnPrimitive * _sender){
    sender = _sender;
    ui->labelEdit->setText(sender->label->toPlainText());
}
