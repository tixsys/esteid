/********************************************************************************
** Form generated from reading ui file 'pinchangedialog.ui'
**
** Created: Thu Feb 26 14:41:53 2009
**      by: Qt User Interface Compiler version 4.5.0-rc1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PINCHANGEDIALOG_H
#define UI_PINCHANGEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PinChangeDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *currentLabel;
    QLineEdit *currentLineEdit;
    QLabel *newlabel;
    QLineEdit *newLineEdit;
    QLabel *repeatLabel;
    QLineEdit *repeatLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PinChangeDialog)
    {
        if (PinChangeDialog->objectName().isEmpty())
            PinChangeDialog->setObjectName(QString::fromUtf8("PinChangeDialog"));
        PinChangeDialog->resize(156, 197);
        verticalLayout = new QVBoxLayout(PinChangeDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        currentLabel = new QLabel(PinChangeDialog);
        currentLabel->setObjectName(QString::fromUtf8("currentLabel"));

        verticalLayout->addWidget(currentLabel);

        currentLineEdit = new QLineEdit(PinChangeDialog);
        currentLineEdit->setObjectName(QString::fromUtf8("currentLineEdit"));

        verticalLayout->addWidget(currentLineEdit);

        newlabel = new QLabel(PinChangeDialog);
        newlabel->setObjectName(QString::fromUtf8("newlabel"));

        verticalLayout->addWidget(newlabel);

        newLineEdit = new QLineEdit(PinChangeDialog);
        newLineEdit->setObjectName(QString::fromUtf8("newLineEdit"));

        verticalLayout->addWidget(newLineEdit);

        repeatLabel = new QLabel(PinChangeDialog);
        repeatLabel->setObjectName(QString::fromUtf8("repeatLabel"));

        verticalLayout->addWidget(repeatLabel);

        repeatLineEdit = new QLineEdit(PinChangeDialog);
        repeatLineEdit->setObjectName(QString::fromUtf8("repeatLineEdit"));

        verticalLayout->addWidget(repeatLineEdit);

        buttonBox = new QDialogButtonBox(PinChangeDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        currentLabel->setBuddy(currentLineEdit);
        newlabel->setBuddy(newLineEdit);
        repeatLabel->setBuddy(repeatLineEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(currentLineEdit, newLineEdit);
        QWidget::setTabOrder(newLineEdit, repeatLineEdit);
        QWidget::setTabOrder(repeatLineEdit, buttonBox);

        retranslateUi(PinChangeDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PinChangeDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PinChangeDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(PinChangeDialog);
    } // setupUi

    void retranslateUi(QDialog *PinChangeDialog)
    {
        PinChangeDialog->setWindowTitle(QApplication::translate("PinChangeDialog", "Change PIN", 0, QApplication::UnicodeUTF8));
        currentLabel->setText(QApplication::translate("PinChangeDialog", "Current PIN:", 0, QApplication::UnicodeUTF8));
        newlabel->setText(QApplication::translate("PinChangeDialog", "New PIN:", 0, QApplication::UnicodeUTF8));
        repeatLabel->setText(QApplication::translate("PinChangeDialog", "Repeat new PIN:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(PinChangeDialog);
    } // retranslateUi

};

namespace Ui {
    class PinChangeDialog: public Ui_PinChangeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PINCHANGEDIALOG_H
