/********************************************************************************
** Form generated from reading UI file 'MedipQT.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEDIPQT_H
#define UI_MEDIPQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MedipQTClass
{
public:
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *VersionInfo;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLineEdit *lineEdit_5;
    QPushButton *pushButton;
    QLabel *label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MedipQTClass)
    {
        if (MedipQTClass->objectName().isEmpty())
            MedipQTClass->setObjectName(QStringLiteral("MedipQTClass"));
        MedipQTClass->resize(690, 580);
        centralWidget = new QWidget(MedipQTClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(3);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 0, 661, 521));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        VersionInfo = new QLabel(verticalLayoutWidget);
        VersionInfo->setObjectName(QStringLiteral("VersionInfo"));

        verticalLayout->addWidget(VersionInfo);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        widget = new QWidget(verticalLayoutWidget);
        widget->setObjectName(QStringLiteral("widget"));
        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(140, 60, 471, 21));
        lineEdit_2 = new QLineEdit(widget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(40, 110, 113, 20));
        lineEdit_3 = new QLineEdit(widget);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(160, 110, 113, 20));
        lineEdit_4 = new QLineEdit(widget);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(280, 110, 113, 20));
        lineEdit_5 = new QLineEdit(widget);
        lineEdit_5->setObjectName(QStringLiteral("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(400, 110, 113, 20));
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(530, 110, 91, 21));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(60, 63, 56, 12));

        verticalLayout_2->addWidget(widget);


        verticalLayout->addLayout(verticalLayout_2);

        MedipQTClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MedipQTClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 690, 21));
        MedipQTClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MedipQTClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MedipQTClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MedipQTClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MedipQTClass->setStatusBar(statusBar);

        retranslateUi(MedipQTClass);

        QMetaObject::connectSlotsByName(MedipQTClass);
    } // setupUi

    void retranslateUi(QMainWindow *MedipQTClass)
    {
        MedipQTClass->setWindowTitle(QApplication::translate("MedipQTClass", "MedipQT", Q_NULLPTR));
        VersionInfo->setText(QApplication::translate("MedipQTClass", "Versino Info", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MedipQTClass", "Get License", Q_NULLPTR));
        label->setText(QApplication::translate("MedipQTClass", "PC Code", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MedipQTClass: public Ui_MedipQTClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEDIPQT_H
