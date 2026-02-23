/********************************************************************************
** Form generated from reading UI file 'ModuleInfoDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODULEINFODIALOG_H
#define UI_MODULEINFODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModuleInfoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *m_tab;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *m_treeModule_Internal;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *m_treeModule_Internal_2;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_4;
    QTreeWidget *m_treeAIWeight;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;

    void setupUi(QDialog *ModuleInfoDialog)
    {
        if (ModuleInfoDialog->objectName().isEmpty())
            ModuleInfoDialog->setObjectName(QStringLiteral("ModuleInfoDialog"));
        ModuleInfoDialog->resize(723, 528);
        verticalLayout = new QVBoxLayout(ModuleInfoDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        m_tab = new QTabWidget(ModuleInfoDialog);
        m_tab->setObjectName(QStringLiteral("m_tab"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        m_treeModule_Internal = new QTreeWidget(tab);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        m_treeModule_Internal->setHeaderItem(__qtreewidgetitem);
        m_treeModule_Internal->setObjectName(QStringLiteral("m_treeModule_Internal"));

        verticalLayout_2->addWidget(m_treeModule_Internal);

        m_tab->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        m_treeModule_Internal_2 = new QTreeWidget(tab_2);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        m_treeModule_Internal_2->setHeaderItem(__qtreewidgetitem1);
        m_treeModule_Internal_2->setObjectName(QStringLiteral("m_treeModule_Internal_2"));

        verticalLayout_3->addWidget(m_treeModule_Internal_2);

        m_tab->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_4 = new QVBoxLayout(tab_3);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        m_treeAIWeight = new QTreeWidget(tab_3);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(0, QStringLiteral("1"));
        m_treeAIWeight->setHeaderItem(__qtreewidgetitem2);
        m_treeAIWeight->setObjectName(QStringLiteral("m_treeAIWeight"));

        verticalLayout_4->addWidget(m_treeAIWeight);

        m_tab->addTab(tab_3, QString());

        verticalLayout->addWidget(m_tab);

        widget = new QWidget(ModuleInfoDialog);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addWidget(widget);


        retranslateUi(ModuleInfoDialog);

        m_tab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ModuleInfoDialog);
    } // setupUi

    void retranslateUi(QDialog *ModuleInfoDialog)
    {
        ModuleInfoDialog->setWindowTitle(QApplication::translate("ModuleInfoDialog", "Dialog", Q_NULLPTR));
        m_tab->setTabText(m_tab->indexOf(tab), QApplication::translate("ModuleInfoDialog", "Internal", Q_NULLPTR));
        m_tab->setTabText(m_tab->indexOf(tab_2), QApplication::translate("ModuleInfoDialog", "External", Q_NULLPTR));
        m_tab->setTabText(m_tab->indexOf(tab_3), QApplication::translate("ModuleInfoDialog", "AI", Q_NULLPTR));
        pushButton->setText(QApplication::translate("ModuleInfoDialog", "OK", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ModuleInfoDialog: public Ui_ModuleInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODULEINFODIALOG_H
