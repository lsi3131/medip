/********************************************************************************
** Form generated from reading UI file 'OmniverseLoginInfoEditDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSELOGININFOEDITDIALOG_H
#define UI_OMNIVERSELOGININFOEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniverseLoginInfoEditDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnAdd;
    QPushButton *m_btnEdit;
    QPushButton *m_btnDelete;
    QPushButton *m_btnCancel;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *m_editServerName;

    void setupUi(QDialog *OmniverseLoginInfoEditDialog)
    {
        if (OmniverseLoginInfoEditDialog->objectName().isEmpty())
            OmniverseLoginInfoEditDialog->setObjectName(QStringLiteral("OmniverseLoginInfoEditDialog"));
        OmniverseLoginInfoEditDialog->resize(381, 103);
        gridLayout = new QGridLayout(OmniverseLoginInfoEditDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new QWidget(OmniverseLoginInfoEditDialog);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_btnAdd = new QPushButton(widget);
        m_btnAdd->setObjectName(QStringLiteral("m_btnAdd"));

        horizontalLayout->addWidget(m_btnAdd);

        m_btnEdit = new QPushButton(widget);
        m_btnEdit->setObjectName(QStringLiteral("m_btnEdit"));

        horizontalLayout->addWidget(m_btnEdit);

        m_btnDelete = new QPushButton(widget);
        m_btnDelete->setObjectName(QStringLiteral("m_btnDelete"));

        horizontalLayout->addWidget(m_btnDelete);

        m_btnCancel = new QPushButton(widget);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout->addWidget(m_btnCancel);


        gridLayout->addWidget(widget, 1, 0, 1, 1);

        widget_3 = new QWidget(OmniverseLoginInfoEditDialog);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_2 = new QHBoxLayout(widget_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(widget_3);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        m_editServerName = new QLineEdit(widget_3);
        m_editServerName->setObjectName(QStringLiteral("m_editServerName"));

        horizontalLayout_2->addWidget(m_editServerName);


        gridLayout->addWidget(widget_3, 0, 0, 1, 1);


        retranslateUi(OmniverseLoginInfoEditDialog);

        QMetaObject::connectSlotsByName(OmniverseLoginInfoEditDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniverseLoginInfoEditDialog)
    {
        OmniverseLoginInfoEditDialog->setWindowTitle(QString());
        m_btnAdd->setText(QApplication::translate("OmniverseLoginInfoEditDialog", "Add", Q_NULLPTR));
        m_btnEdit->setText(QApplication::translate("OmniverseLoginInfoEditDialog", "Edit", Q_NULLPTR));
        m_btnDelete->setText(QApplication::translate("OmniverseLoginInfoEditDialog", "Delete", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniverseLoginInfoEditDialog", "Cancel", Q_NULLPTR));
        label->setText(QApplication::translate("OmniverseLoginInfoEditDialog", "Omniverse Server : ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniverseLoginInfoEditDialog: public Ui_OmniverseLoginInfoEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSELOGININFOEDITDIALOG_H
