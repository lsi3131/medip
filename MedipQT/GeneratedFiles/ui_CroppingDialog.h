/********************************************************************************
** Form generated from reading UI file 'CroppingDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CROPPINGDIALOG_H
#define UI_CROPPINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "windows/croppingviewcontrolwidget.h"

QT_BEGIN_NAMESPACE

class Ui_CroppingDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    CroppingViewControlWidget *m_axialViewWidget;
    CroppingViewControlWidget *m_coronalViewWidget;
    CroppingViewControlWidget *m_sagittalViewWidget;
    QWidget *m_widgetControlPanel;
    QVBoxLayout *verticalLayout;
    QGroupBox *m_groupMemoryInfo;
    QVBoxLayout *verticalLayout_2;
    QLabel *m_totalMemory;
    QLabel *m_availablelMemory;
    QLabel *m_volumeMemory;
    QGroupBox *m_groupLengthInfo;
    QVBoxLayout *verticalLayout_3;
    QLabel *m_pQLabelxlen;
    QLabel *m_pQLabelylen;
    QLabel *m_pQLabelzlen;
    QSpacerItem *verticalSpacer;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *m_btnOpen;
    QPushButton *m_btnReset;

    void setupUi(QDialog *CroppingDialog)
    {
        if (CroppingDialog->objectName().isEmpty())
            CroppingDialog->setObjectName(QStringLiteral("CroppingDialog"));
        CroppingDialog->resize(807, 418);
        gridLayout = new QGridLayout(CroppingDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new QWidget(CroppingDialog);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        m_axialViewWidget = new CroppingViewControlWidget(widget);
        m_axialViewWidget->setObjectName(QStringLiteral("m_axialViewWidget"));

        horizontalLayout->addWidget(m_axialViewWidget);

        m_coronalViewWidget = new CroppingViewControlWidget(widget);
        m_coronalViewWidget->setObjectName(QStringLiteral("m_coronalViewWidget"));

        horizontalLayout->addWidget(m_coronalViewWidget);

        m_sagittalViewWidget = new CroppingViewControlWidget(widget);
        m_sagittalViewWidget->setObjectName(QStringLiteral("m_sagittalViewWidget"));

        horizontalLayout->addWidget(m_sagittalViewWidget);


        gridLayout->addWidget(widget, 0, 0, 1, 1);

        m_widgetControlPanel = new QWidget(CroppingDialog);
        m_widgetControlPanel->setObjectName(QStringLiteral("m_widgetControlPanel"));
        verticalLayout = new QVBoxLayout(m_widgetControlPanel);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        m_groupMemoryInfo = new QGroupBox(m_widgetControlPanel);
        m_groupMemoryInfo->setObjectName(QStringLiteral("m_groupMemoryInfo"));
        verticalLayout_2 = new QVBoxLayout(m_groupMemoryInfo);
        verticalLayout_2->setSpacing(12);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        m_totalMemory = new QLabel(m_groupMemoryInfo);
        m_totalMemory->setObjectName(QStringLiteral("m_totalMemory"));

        verticalLayout_2->addWidget(m_totalMemory);

        m_availablelMemory = new QLabel(m_groupMemoryInfo);
        m_availablelMemory->setObjectName(QStringLiteral("m_availablelMemory"));

        verticalLayout_2->addWidget(m_availablelMemory);

        m_volumeMemory = new QLabel(m_groupMemoryInfo);
        m_volumeMemory->setObjectName(QStringLiteral("m_volumeMemory"));

        verticalLayout_2->addWidget(m_volumeMemory);


        verticalLayout->addWidget(m_groupMemoryInfo);

        m_groupLengthInfo = new QGroupBox(m_widgetControlPanel);
        m_groupLengthInfo->setObjectName(QStringLiteral("m_groupLengthInfo"));
        verticalLayout_3 = new QVBoxLayout(m_groupLengthInfo);
        verticalLayout_3->setSpacing(12);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        m_pQLabelxlen = new QLabel(m_groupLengthInfo);
        m_pQLabelxlen->setObjectName(QStringLiteral("m_pQLabelxlen"));

        verticalLayout_3->addWidget(m_pQLabelxlen);

        m_pQLabelylen = new QLabel(m_groupLengthInfo);
        m_pQLabelylen->setObjectName(QStringLiteral("m_pQLabelylen"));

        verticalLayout_3->addWidget(m_pQLabelylen);

        m_pQLabelzlen = new QLabel(m_groupLengthInfo);
        m_pQLabelzlen->setObjectName(QStringLiteral("m_pQLabelzlen"));

        verticalLayout_3->addWidget(m_pQLabelzlen);


        verticalLayout->addWidget(m_groupLengthInfo);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        widget_5 = new QWidget(m_widgetControlPanel);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget_5->sizePolicy().hasHeightForWidth());
        widget_5->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(widget_5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        m_btnOpen = new QPushButton(widget_5);
        m_btnOpen->setObjectName(QStringLiteral("m_btnOpen"));
        m_btnOpen->setMinimumSize(QSize(76, 30));

        horizontalLayout_2->addWidget(m_btnOpen);

        m_btnReset = new QPushButton(widget_5);
        m_btnReset->setObjectName(QStringLiteral("m_btnReset"));
        m_btnReset->setMinimumSize(QSize(76, 30));

        horizontalLayout_2->addWidget(m_btnReset);


        verticalLayout->addWidget(widget_5);


        gridLayout->addWidget(m_widgetControlPanel, 0, 1, 1, 1);


        retranslateUi(CroppingDialog);

        QMetaObject::connectSlotsByName(CroppingDialog);
    } // setupUi

    void retranslateUi(QDialog *CroppingDialog)
    {
        CroppingDialog->setWindowTitle(QString());
        m_groupMemoryInfo->setTitle(QApplication::translate("CroppingDialog", "Memory Info", Q_NULLPTR));
        m_totalMemory->setText(QApplication::translate("CroppingDialog", "Total : ", Q_NULLPTR));
        m_availablelMemory->setText(QApplication::translate("CroppingDialog", "Free : ", Q_NULLPTR));
        m_volumeMemory->setText(QApplication::translate("CroppingDialog", "Volume : ", Q_NULLPTR));
        m_groupLengthInfo->setTitle(QApplication::translate("CroppingDialog", "Length Info", Q_NULLPTR));
        m_pQLabelxlen->setText(QApplication::translate("CroppingDialog", "x length : ", Q_NULLPTR));
        m_pQLabelylen->setText(QApplication::translate("CroppingDialog", "y length : ", Q_NULLPTR));
        m_pQLabelzlen->setText(QApplication::translate("CroppingDialog", "z length : ", Q_NULLPTR));
        m_btnOpen->setText(QApplication::translate("CroppingDialog", "Open", Q_NULLPTR));
        m_btnReset->setText(QApplication::translate("CroppingDialog", "Reset", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CroppingDialog: public Ui_CroppingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CROPPINGDIALOG_H
