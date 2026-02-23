/********************************************************************************
** Form generated from reading UI file 'SessionSelectDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SESSIONSELECTDIALOG_H
#define UI_SESSIONSELECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SessionSelectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *m_cboSession;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;

    void setupUi(QDialog *SessionSelectDialog)
    {
        if (SessionSelectDialog->objectName().isEmpty())
            SessionSelectDialog->setObjectName(QStringLiteral("SessionSelectDialog"));
        SessionSelectDialog->resize(306, 123);
        verticalLayout = new QVBoxLayout(SessionSelectDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new QWidget(SessionSelectDialog);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        m_cboSession = new QComboBox(widget);
        m_cboSession->setObjectName(QStringLiteral("m_cboSession"));

        horizontalLayout->addWidget(m_cboSession);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(SessionSelectDialog);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        m_btnOk = new QPushButton(widget_2);
        m_btnOk->setObjectName(QStringLiteral("m_btnOk"));

        horizontalLayout_2->addWidget(m_btnOk);

        m_btnCancel = new QPushButton(widget_2);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout_2->addWidget(m_btnCancel);


        verticalLayout->addWidget(widget_2);


        retranslateUi(SessionSelectDialog);

        QMetaObject::connectSlotsByName(SessionSelectDialog);
    } // setupUi

    void retranslateUi(QDialog *SessionSelectDialog)
    {
        SessionSelectDialog->setWindowTitle(QApplication::translate("SessionSelectDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("SessionSelectDialog", "Session : ", Q_NULLPTR));
        m_btnOk->setText(QApplication::translate("SessionSelectDialog", "Ok", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("SessionSelectDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SessionSelectDialog: public Ui_SessionSelectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SESSIONSELECTDIALOG_H
