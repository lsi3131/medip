/********************************************************************************
** Form generated from reading UI file 'DebugUsdStageViewerWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGUSDSTAGEVIEWERWIDGET_H
#define UI_DEBUGUSDSTAGEVIEWERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "UI/Omniverse/omniversestagelistwidget.h"
#include "ui/Omniverse/omniversepropertywidget.h"

QT_BEGIN_NAMESPACE

class Ui_DebugUsdStageViewerWidget
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLineEdit *m_editCurrentFile;
    QPushButton *m_btnOpenUsd;
    QGroupBox *m_groupBox;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_6;
    OmniverseStageListWidget *m_omniverseStageViewer;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_6;
    OmniversePropertyWidget *m_omniversePropertyViewer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QListWidget *listWidget;

    void setupUi(QWidget *DebugUsdStageViewerWidget)
    {
        if (DebugUsdStageViewerWidget->objectName().isEmpty())
            DebugUsdStageViewerWidget->setObjectName(QStringLiteral("DebugUsdStageViewerWidget"));
        DebugUsdStageViewerWidget->resize(1011, 774);
        verticalLayout = new QVBoxLayout(DebugUsdStageViewerWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new QWidget(DebugUsdStageViewerWidget);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        m_editCurrentFile = new QLineEdit(widget);
        m_editCurrentFile->setObjectName(QStringLiteral("m_editCurrentFile"));

        horizontalLayout_2->addWidget(m_editCurrentFile);

        m_btnOpenUsd = new QPushButton(widget);
        m_btnOpenUsd->setObjectName(QStringLiteral("m_btnOpenUsd"));

        horizontalLayout_2->addWidget(m_btnOpenUsd);


        verticalLayout->addWidget(widget);

        m_groupBox = new QGroupBox(DebugUsdStageViewerWidget);
        m_groupBox->setObjectName(QStringLiteral("m_groupBox"));
        horizontalLayout = new QHBoxLayout(m_groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox_5 = new QGroupBox(m_groupBox);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy1);
        verticalLayout_6 = new QVBoxLayout(groupBox_5);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        m_omniverseStageViewer = new OmniverseStageListWidget(groupBox_5);
        m_omniverseStageViewer->setObjectName(QStringLiteral("m_omniverseStageViewer"));

        verticalLayout_6->addWidget(m_omniverseStageViewer);


        horizontalLayout->addWidget(groupBox_5);

        widget_2 = new QWidget(m_groupBox);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);
        verticalLayout_2 = new QVBoxLayout(widget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox_6 = new QGroupBox(widget_2);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(2);
        sizePolicy2.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy2);
        m_omniversePropertyViewer = new OmniversePropertyWidget(groupBox_6);
        m_omniversePropertyViewer->setObjectName(QStringLiteral("m_omniversePropertyViewer"));
        m_omniversePropertyViewer->setGeometry(QRect(10, 20, 451, 566));

        verticalLayout_2->addWidget(groupBox_6);

        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        verticalLayout_3->addWidget(listWidget);


        verticalLayout_2->addWidget(groupBox);


        horizontalLayout->addWidget(widget_2);


        verticalLayout->addWidget(m_groupBox);


        retranslateUi(DebugUsdStageViewerWidget);

        QMetaObject::connectSlotsByName(DebugUsdStageViewerWidget);
    } // setupUi

    void retranslateUi(QWidget *DebugUsdStageViewerWidget)
    {
        DebugUsdStageViewerWidget->setWindowTitle(QApplication::translate("DebugUsdStageViewerWidget", "Form", Q_NULLPTR));
        label_3->setText(QApplication::translate("DebugUsdStageViewerWidget", "Working : ", Q_NULLPTR));
        m_btnOpenUsd->setText(QApplication::translate("DebugUsdStageViewerWidget", "Open", Q_NULLPTR));
        m_groupBox->setTitle(QApplication::translate("DebugUsdStageViewerWidget", "Viewer", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("DebugUsdStageViewerWidget", "Stage", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("DebugUsdStageViewerWidget", "Property", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("DebugUsdStageViewerWidget", "Status", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DebugUsdStageViewerWidget: public Ui_DebugUsdStageViewerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGUSDSTAGEVIEWERWIDGET_H
