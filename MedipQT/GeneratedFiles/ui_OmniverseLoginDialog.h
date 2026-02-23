/********************************************************************************
** Form generated from reading UI file 'OmniverseLoginDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSELOGINDIALOG_H
#define UI_OMNIVERSELOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniverseLoginDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *m_cboServerName;
    QPushButton *m_btnEdit;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnConnect;
    QPushButton *m_btnCancel;

    void setupUi(QDialog *OmniverseLoginDialog)
    {
        if (OmniverseLoginDialog->objectName().isEmpty())
            OmniverseLoginDialog->setObjectName(QStringLiteral("OmniverseLoginDialog"));
        OmniverseLoginDialog->resize(449, 156);
        gridLayout = new QGridLayout(OmniverseLoginDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_3 = new QWidget(OmniverseLoginDialog);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_2 = new QHBoxLayout(widget_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(widget_3);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_2);

        m_cboServerName = new QComboBox(widget_3);
        m_cboServerName->setObjectName(QStringLiteral("m_cboServerName"));

        horizontalLayout_2->addWidget(m_cboServerName);

        m_btnEdit = new QPushButton(widget_3);
        m_btnEdit->setObjectName(QStringLiteral("m_btnEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_btnEdit->sizePolicy().hasHeightForWidth());
        m_btnEdit->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(m_btnEdit);


        gridLayout->addWidget(widget_3, 0, 0, 1, 1);

        widget = new QWidget(OmniverseLoginDialog);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy2);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_btnConnect = new QPushButton(widget);
        m_btnConnect->setObjectName(QStringLiteral("m_btnConnect"));

        horizontalLayout->addWidget(m_btnConnect);

        m_btnCancel = new QPushButton(widget);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout->addWidget(m_btnCancel);


        gridLayout->addWidget(widget, 1, 0, 1, 1);


        retranslateUi(OmniverseLoginDialog);

        QMetaObject::connectSlotsByName(OmniverseLoginDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniverseLoginDialog)
    {
        OmniverseLoginDialog->setWindowTitle(QString());
        label_2->setText(QApplication::translate("OmniverseLoginDialog", "Omniverse:", Q_NULLPTR));
        m_btnEdit->setText(QApplication::translate("OmniverseLoginDialog", "Edit", Q_NULLPTR));
        m_btnConnect->setText(QApplication::translate("OmniverseLoginDialog", "Connect", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniverseLoginDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniverseLoginDialog: public Ui_OmniverseLoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSELOGINDIALOG_H
