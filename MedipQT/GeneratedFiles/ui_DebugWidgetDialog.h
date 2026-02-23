/********************************************************************************
** Form generated from reading UI file 'DebugWidgetDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGWIDGETDIALOG_H
#define UI_DEBUGWIDGETDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_DebugWidgetDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QListWidget *m_listWidget;
    QGroupBox *m_groupWidget;

    void setupUi(QDialog *DebugWidgetDialog)
    {
        if (DebugWidgetDialog->objectName().isEmpty())
            DebugWidgetDialog->setObjectName(QStringLiteral("DebugWidgetDialog"));
        DebugWidgetDialog->resize(1110, 693);
        horizontalLayout_2 = new QHBoxLayout(DebugWidgetDialog);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox = new QGroupBox(DebugWidgetDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        m_listWidget = new QListWidget(groupBox);
        m_listWidget->setObjectName(QStringLiteral("m_listWidget"));

        horizontalLayout->addWidget(m_listWidget);


        horizontalLayout_2->addWidget(groupBox);

        m_groupWidget = new QGroupBox(DebugWidgetDialog);
        m_groupWidget->setObjectName(QStringLiteral("m_groupWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(4);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_groupWidget->sizePolicy().hasHeightForWidth());
        m_groupWidget->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(m_groupWidget);


        retranslateUi(DebugWidgetDialog);

        QMetaObject::connectSlotsByName(DebugWidgetDialog);
    } // setupUi

    void retranslateUi(QDialog *DebugWidgetDialog)
    {
        DebugWidgetDialog->setWindowTitle(QApplication::translate("DebugWidgetDialog", "Dialog", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("DebugWidgetDialog", "Widget List", Q_NULLPTR));
        m_groupWidget->setTitle(QApplication::translate("DebugWidgetDialog", "Widget", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DebugWidgetDialog: public Ui_DebugWidgetDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGWIDGETDIALOG_H
