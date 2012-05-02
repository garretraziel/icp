/********************************************************************************
** Form generated from reading UI file 'editdialog.ui'
**
** Created: Wed May 2 12:15:06 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITDIALOG_H
#define UI_EDITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_editDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *labelEdit;

    void setupUi(QDialog *editDialog)
    {
        if (editDialog->objectName().isEmpty())
            editDialog->setObjectName(QString::fromUtf8("editDialog"));
        editDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(editDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(300, 30, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        formLayoutWidget = new QWidget(editDialog);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(9, 30, 271, 241));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        labelEdit = new QLineEdit(formLayoutWidget);
        labelEdit->setObjectName(QString::fromUtf8("labelEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, labelEdit);


        retranslateUi(editDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), editDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), editDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(editDialog);
    } // setupUi

    void retranslateUi(QDialog *editDialog)
    {
        editDialog->setWindowTitle(QApplication::translate("editDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("editDialog", "Label", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class editDialog: public Ui_editDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITDIALOG_H
