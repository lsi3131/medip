/********************************************************************************
** Form generated from reading UI file 'DebugOmniverseLiveSessionWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGOMNIVERSELIVESESSIONWIDGET_H
#define UI_DEBUGOMNIVERSELIVESESSIONWIDGET_H

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
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DebugOmniverseLiveSessionWidget
{
public:
    QPushButton *m_btnLogin;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *m_btnOpen;
    QPushButton *m_btnLiveMode;
    QLineEdit *m_editServerName;
    QLineEdit *m_editUserName;
    QLineEdit *m_editCurrentFile;
    QPushButton *m_btnCreate;
    QTabWidget *m_tab;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_6;
    QWidget *m_omniverseStageViewer;
    QGroupBox *groupBox_6;
    QWidget *m_omniversePropertyViewer;
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *m_groupLiveSession;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QPushButton *m_btnLiveChannel_Left;
    QPushButton *m_btnLiveChannel_Join;
    QPushButton *m_btnLiveChannel_Hello;
    QPushButton *m_btnLiveChannel_GetUsers;
    QPushButton *m_btnLiveChannel_MergeStarted;
    QPushButton *m_btnLiveChannel_MergeFinished;
    QLabel *label_4;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_2;
    QListWidget *m_listLiveSessionUserList;
    QLineEdit *m_editSessionName;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *m_textLiveSessionLog;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QListWidget *m_listLiveSessionList;

    void setupUi(QWidget *DebugOmniverseLiveSessionWidget)
    {
        if (DebugOmniverseLiveSessionWidget->objectName().isEmpty())
            DebugOmniverseLiveSessionWidget->setObjectName(QStringLiteral("DebugOmniverseLiveSessionWidget"));
        DebugOmniverseLiveSessionWidget->resize(1011, 774);
        m_btnLogin = new QPushButton(DebugOmniverseLiveSessionWidget);
        m_btnLogin->setObjectName(QStringLiteral("m_btnLogin"));
        m_btnLogin->setGeometry(QRect(20, 80, 75, 23));
        label = new QLabel(DebugOmniverseLiveSessionWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 20, 56, 12));
        label_2 = new QLabel(DebugOmniverseLiveSessionWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(230, 20, 91, 16));
        label_3 = new QLabel(DebugOmniverseLiveSessionWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 50, 81, 16));
        m_btnOpen = new QPushButton(DebugOmniverseLiveSessionWidget);
        m_btnOpen->setObjectName(QStringLiteral("m_btnOpen"));
        m_btnOpen->setGeometry(QRect(110, 80, 75, 23));
        m_btnLiveMode = new QPushButton(DebugOmniverseLiveSessionWidget);
        m_btnLiveMode->setObjectName(QStringLiteral("m_btnLiveMode"));
        m_btnLiveMode->setGeometry(QRect(290, 80, 75, 23));
        m_editServerName = new QLineEdit(DebugOmniverseLiveSessionWidget);
        m_editServerName->setObjectName(QStringLiteral("m_editServerName"));
        m_editServerName->setGeometry(QRect(80, 20, 113, 20));
        m_editUserName = new QLineEdit(DebugOmniverseLiveSessionWidget);
        m_editUserName->setObjectName(QStringLiteral("m_editUserName"));
        m_editUserName->setGeometry(QRect(310, 20, 113, 20));
        m_editCurrentFile = new QLineEdit(DebugOmniverseLiveSessionWidget);
        m_editCurrentFile->setObjectName(QStringLiteral("m_editCurrentFile"));
        m_editCurrentFile->setGeometry(QRect(90, 50, 491, 20));
        m_btnCreate = new QPushButton(DebugOmniverseLiveSessionWidget);
        m_btnCreate->setObjectName(QStringLiteral("m_btnCreate"));
        m_btnCreate->setGeometry(QRect(200, 80, 75, 23));
        m_tab = new QTabWidget(DebugOmniverseLiveSessionWidget);
        m_tab->setObjectName(QStringLiteral("m_tab"));
        m_tab->setGeometry(QRect(10, 120, 971, 641));
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        horizontalLayout = new QHBoxLayout(tab_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox_5 = new QGroupBox(tab_2);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        verticalLayout_6 = new QVBoxLayout(groupBox_5);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        m_omniverseStageViewer = new QWidget(groupBox_5);
        m_omniverseStageViewer->setObjectName(QStringLiteral("m_omniverseStageViewer"));

        verticalLayout_6->addWidget(m_omniverseStageViewer);


        horizontalLayout->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(tab_2);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        m_omniversePropertyViewer = new QWidget(groupBox_6);
        m_omniversePropertyViewer->setObjectName(QStringLiteral("m_omniversePropertyViewer"));
        m_omniversePropertyViewer->setGeometry(QRect(10, 20, 451, 566));

        horizontalLayout->addWidget(groupBox_6);

        m_tab->addTab(tab_2, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_5 = new QVBoxLayout(tab);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        m_groupLiveSession = new QGroupBox(tab);
        m_groupLiveSession->setObjectName(QStringLiteral("m_groupLiveSession"));
        groupBox = new QGroupBox(m_groupLiveSession);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 30, 141, 211));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        m_btnLiveChannel_Left = new QPushButton(groupBox);
        m_btnLiveChannel_Left->setObjectName(QStringLiteral("m_btnLiveChannel_Left"));

        verticalLayout->addWidget(m_btnLiveChannel_Left);

        m_btnLiveChannel_Join = new QPushButton(groupBox);
        m_btnLiveChannel_Join->setObjectName(QStringLiteral("m_btnLiveChannel_Join"));

        verticalLayout->addWidget(m_btnLiveChannel_Join);

        m_btnLiveChannel_Hello = new QPushButton(groupBox);
        m_btnLiveChannel_Hello->setObjectName(QStringLiteral("m_btnLiveChannel_Hello"));

        verticalLayout->addWidget(m_btnLiveChannel_Hello);

        m_btnLiveChannel_GetUsers = new QPushButton(groupBox);
        m_btnLiveChannel_GetUsers->setObjectName(QStringLiteral("m_btnLiveChannel_GetUsers"));

        verticalLayout->addWidget(m_btnLiveChannel_GetUsers);

        m_btnLiveChannel_MergeStarted = new QPushButton(groupBox);
        m_btnLiveChannel_MergeStarted->setObjectName(QStringLiteral("m_btnLiveChannel_MergeStarted"));

        verticalLayout->addWidget(m_btnLiveChannel_MergeStarted);

        m_btnLiveChannel_MergeFinished = new QPushButton(groupBox);
        m_btnLiveChannel_MergeFinished->setObjectName(QStringLiteral("m_btnLiveChannel_MergeFinished"));

        verticalLayout->addWidget(m_btnLiveChannel_MergeFinished);

        label_4 = new QLabel(m_groupLiveSession);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(390, 20, 101, 16));
        groupBox_3 = new QGroupBox(m_groupLiveSession);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(380, 50, 276, 201));
        verticalLayout_2 = new QVBoxLayout(groupBox_3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        m_listLiveSessionUserList = new QListWidget(groupBox_3);
        m_listLiveSessionUserList->setObjectName(QStringLiteral("m_listLiveSessionUserList"));

        verticalLayout_2->addWidget(m_listLiveSessionUserList);

        m_editSessionName = new QLineEdit(m_groupLiveSession);
        m_editSessionName->setObjectName(QStringLiteral("m_editSessionName"));
        m_editSessionName->setGeometry(QRect(500, 20, 141, 20));
        groupBox_4 = new QGroupBox(m_groupLiveSession);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(670, 10, 276, 241));
        verticalLayout_3 = new QVBoxLayout(groupBox_4);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        m_textLiveSessionLog = new QTextEdit(groupBox_4);
        m_textLiveSessionLog->setObjectName(QStringLiteral("m_textLiveSessionLog"));
        m_textLiveSessionLog->setReadOnly(true);

        verticalLayout_3->addWidget(m_textLiveSessionLog);

        groupBox_2 = new QGroupBox(m_groupLiveSession);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(160, 20, 211, 231));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        m_listLiveSessionList = new QListWidget(groupBox_2);
        m_listLiveSessionList->setObjectName(QStringLiteral("m_listLiveSessionList"));

        verticalLayout_4->addWidget(m_listLiveSessionList);


        verticalLayout_5->addWidget(m_groupLiveSession);

        m_tab->addTab(tab, QString());

        retranslateUi(DebugOmniverseLiveSessionWidget);

        m_tab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DebugOmniverseLiveSessionWidget);
    } // setupUi

    void retranslateUi(QWidget *DebugOmniverseLiveSessionWidget)
    {
        DebugOmniverseLiveSessionWidget->setWindowTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Form", Q_NULLPTR));
        m_btnLogin->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Login", Q_NULLPTR));
        label->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Server : ", Q_NULLPTR));
        label_2->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "User Name : ", Q_NULLPTR));
        label_3->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Working : ", Q_NULLPTR));
        m_btnOpen->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Open", Q_NULLPTR));
        m_btnLiveMode->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Live Mode", Q_NULLPTR));
        m_btnCreate->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Create", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Stage", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Property", Q_NULLPTR));
        m_tab->setTabText(m_tab->indexOf(tab_2), QApplication::translate("DebugOmniverseLiveSessionWidget", "Stage", Q_NULLPTR));
        m_groupLiveSession->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Live Session", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Live Channel Message", Q_NULLPTR));
        m_btnLiveChannel_Left->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Left", Q_NULLPTR));
        m_btnLiveChannel_Join->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Join", Q_NULLPTR));
        m_btnLiveChannel_Hello->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Hello", Q_NULLPTR));
        m_btnLiveChannel_GetUsers->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "GetUsers", Q_NULLPTR));
        m_btnLiveChannel_MergeStarted->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Merge Started", Q_NULLPTR));
        m_btnLiveChannel_MergeFinished->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Merge Finished", Q_NULLPTR));
        label_4->setText(QApplication::translate("DebugOmniverseLiveSessionWidget", "Session Name : ", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "User List", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Log", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("DebugOmniverseLiveSessionWidget", "Session List", Q_NULLPTR));
        m_tab->setTabText(m_tab->indexOf(tab), QApplication::translate("DebugOmniverseLiveSessionWidget", "Live Session", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DebugOmniverseLiveSessionWidget: public Ui_DebugOmniverseLiveSessionWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGOMNIVERSELIVESESSIONWIDGET_H
