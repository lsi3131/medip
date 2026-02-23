/********************************************************************************
** Form generated from reading UI file 'OmniverseLiveSessionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSELIVESESSIONDIALOG_H
#define UI_OMNIVERSELIVESESSIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniverseLiveSessionDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QRadioButton *m_rdoMode_Join;
    QRadioButton *m_rdoMode_Create;
    QStackedWidget *m_containerMode;
    QWidget *m_containerModePage1;
    QVBoxLayout *verticalLayout_4;
    QComboBox *m_cboJoinSessionList;
    QLabel *label;
    QListWidget *m_listJoinLiveSessionUserList;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnJoinLiveSession;
    QPushButton *m_btnJoinCancel;
    QWidget *m_containerModePage2;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *m_editCreateSessionName;
    QSpacerItem *verticalSpacer;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *m_btnCreateKeepLocalSource;
    QPushButton *m_btnCreateFetchFromOmniverse;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *m_btnCreateCancel;

    void setupUi(QDialog *OmniverseLiveSessionDialog)
    {
        if (OmniverseLiveSessionDialog->objectName().isEmpty())
            OmniverseLiveSessionDialog->setObjectName(QStringLiteral("OmniverseLiveSessionDialog"));
        OmniverseLiveSessionDialog->resize(547, 371);
        gridLayout = new QGridLayout(OmniverseLiveSessionDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_3 = new QWidget(OmniverseLiveSessionDialog);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        verticalLayout_2 = new QVBoxLayout(widget_3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox_2 = new QGroupBox(widget_3);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        m_rdoMode_Join = new QRadioButton(groupBox_2);
        m_rdoMode_Join->setObjectName(QStringLiteral("m_rdoMode_Join"));
        m_rdoMode_Join->setChecked(true);

        verticalLayout->addWidget(m_rdoMode_Join);

        m_rdoMode_Create = new QRadioButton(groupBox_2);
        m_rdoMode_Create->setObjectName(QStringLiteral("m_rdoMode_Create"));

        verticalLayout->addWidget(m_rdoMode_Create);


        verticalLayout_2->addWidget(groupBox_2);

        m_containerMode = new QStackedWidget(widget_3);
        m_containerMode->setObjectName(QStringLiteral("m_containerMode"));
        m_containerModePage1 = new QWidget();
        m_containerModePage1->setObjectName(QStringLiteral("m_containerModePage1"));
        verticalLayout_4 = new QVBoxLayout(m_containerModePage1);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        m_cboJoinSessionList = new QComboBox(m_containerModePage1);
        m_cboJoinSessionList->setObjectName(QStringLiteral("m_cboJoinSessionList"));

        verticalLayout_4->addWidget(m_cboJoinSessionList);

        label = new QLabel(m_containerModePage1);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_4->addWidget(label);

        m_listJoinLiveSessionUserList = new QListWidget(m_containerModePage1);
        m_listJoinLiveSessionUserList->setObjectName(QStringLiteral("m_listJoinLiveSessionUserList"));

        verticalLayout_4->addWidget(m_listJoinLiveSessionUserList);

        widget = new QWidget(m_containerModePage1);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_btnJoinLiveSession = new QPushButton(widget);
        m_btnJoinLiveSession->setObjectName(QStringLiteral("m_btnJoinLiveSession"));

        horizontalLayout->addWidget(m_btnJoinLiveSession);

        m_btnJoinCancel = new QPushButton(widget);
        m_btnJoinCancel->setObjectName(QStringLiteral("m_btnJoinCancel"));

        horizontalLayout->addWidget(m_btnJoinCancel);


        verticalLayout_4->addWidget(widget);

        m_containerMode->addWidget(m_containerModePage1);
        m_containerModePage2 = new QWidget();
        m_containerModePage2->setObjectName(QStringLiteral("m_containerModePage2"));
        verticalLayout_3 = new QVBoxLayout(m_containerModePage2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        m_editCreateSessionName = new QLineEdit(m_containerModePage2);
        m_editCreateSessionName->setObjectName(QStringLiteral("m_editCreateSessionName"));

        verticalLayout_3->addWidget(m_editCreateSessionName);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        widget_2 = new QWidget(m_containerModePage2);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        m_btnCreateKeepLocalSource = new QPushButton(widget_2);
        m_btnCreateKeepLocalSource->setObjectName(QStringLiteral("m_btnCreateKeepLocalSource"));

        horizontalLayout_2->addWidget(m_btnCreateKeepLocalSource);

        m_btnCreateFetchFromOmniverse = new QPushButton(widget_2);
        m_btnCreateFetchFromOmniverse->setObjectName(QStringLiteral("m_btnCreateFetchFromOmniverse"));

        horizontalLayout_2->addWidget(m_btnCreateFetchFromOmniverse);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        m_btnCreateCancel = new QPushButton(widget_2);
        m_btnCreateCancel->setObjectName(QStringLiteral("m_btnCreateCancel"));

        horizontalLayout_2->addWidget(m_btnCreateCancel);


        verticalLayout_3->addWidget(widget_2);

        m_containerMode->addWidget(m_containerModePage2);

        verticalLayout_2->addWidget(m_containerMode);


        gridLayout->addWidget(widget_3, 0, 0, 1, 1);


        retranslateUi(OmniverseLiveSessionDialog);

        QMetaObject::connectSlotsByName(OmniverseLiveSessionDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniverseLiveSessionDialog)
    {
        OmniverseLiveSessionDialog->setWindowTitle(QString());
        groupBox_2->setTitle(QApplication::translate("OmniverseLiveSessionDialog", "Live Session mode", Q_NULLPTR));
        m_rdoMode_Join->setText(QApplication::translate("OmniverseLiveSessionDialog", "Join an existing live session", Q_NULLPTR));
        m_rdoMode_Create->setText(QApplication::translate("OmniverseLiveSessionDialog", "Create a new live session", Q_NULLPTR));
        label->setText(QApplication::translate("OmniverseLiveSessionDialog", "Participants", Q_NULLPTR));
        m_btnJoinLiveSession->setText(QApplication::translate("OmniverseLiveSessionDialog", "Join", Q_NULLPTR));
        m_btnJoinCancel->setText(QApplication::translate("OmniverseLiveSessionDialog", "Cancel", Q_NULLPTR));
        m_btnCreateKeepLocalSource->setText(QApplication::translate("OmniverseLiveSessionDialog", "Keep Local Source", Q_NULLPTR));
        m_btnCreateFetchFromOmniverse->setText(QApplication::translate("OmniverseLiveSessionDialog", "Fetch from Omniverse", Q_NULLPTR));
        m_btnCreateCancel->setText(QApplication::translate("OmniverseLiveSessionDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniverseLiveSessionDialog: public Ui_OmniverseLiveSessionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSELIVESESSIONDIALOG_H
