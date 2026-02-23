/********************************************************************************
** Form generated from reading UI file 'OmniversePresetAddOrModifyDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSEPRESETADDORMODIFYDIALOG_H
#define UI_OMNIVERSEPRESETADDORMODIFYDIALOG_H

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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniversePresetAddOrModifyDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *m_editPresetName;
    QPushButton *m_btnApply;
    QPushButton *m_btnCancel;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *m_treeMeshMaterialInfo;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *m_treePrimInfo;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *m_btnLoadUSD;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *m_btnAdd;
    QPushButton *m_btnDelete;

    void setupUi(QDialog *OmniversePresetAddOrModifyDialog)
    {
        if (OmniversePresetAddOrModifyDialog->objectName().isEmpty())
            OmniversePresetAddOrModifyDialog->setObjectName(QStringLiteral("OmniversePresetAddOrModifyDialog"));
        OmniversePresetAddOrModifyDialog->resize(545, 621);
        gridLayout = new QGridLayout(OmniversePresetAddOrModifyDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_4 = new QWidget(OmniversePresetAddOrModifyDialog);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widget_4);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(widget_4);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        m_editPresetName = new QLineEdit(widget_4);
        m_editPresetName->setObjectName(QStringLiteral("m_editPresetName"));

        horizontalLayout_2->addWidget(m_editPresetName);

        m_btnApply = new QPushButton(widget_4);
        m_btnApply->setObjectName(QStringLiteral("m_btnApply"));

        horizontalLayout_2->addWidget(m_btnApply);

        m_btnCancel = new QPushButton(widget_4);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout_2->addWidget(m_btnCancel);


        gridLayout->addWidget(widget_4, 1, 0, 1, 1);

        widget_2 = new QWidget(OmniversePresetAddOrModifyDialog);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        m_treeMeshMaterialInfo = new QTreeWidget(groupBox);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        m_treeMeshMaterialInfo->setHeaderItem(__qtreewidgetitem);
        m_treeMeshMaterialInfo->setObjectName(QStringLiteral("m_treeMeshMaterialInfo"));

        verticalLayout_2->addWidget(m_treeMeshMaterialInfo);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(widget_2);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        m_treePrimInfo = new QTreeWidget(groupBox_2);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        m_treePrimInfo->setHeaderItem(__qtreewidgetitem1);
        m_treePrimInfo->setObjectName(QStringLiteral("m_treePrimInfo"));

        verticalLayout_3->addWidget(m_treePrimInfo);


        verticalLayout->addWidget(groupBox_2);

        widget = new QWidget(widget_2);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        m_btnLoadUSD = new QPushButton(widget);
        m_btnLoadUSD->setObjectName(QStringLiteral("m_btnLoadUSD"));

        horizontalLayout->addWidget(m_btnLoadUSD);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        m_btnAdd = new QPushButton(widget);
        m_btnAdd->setObjectName(QStringLiteral("m_btnAdd"));

        horizontalLayout->addWidget(m_btnAdd);

        m_btnDelete = new QPushButton(widget);
        m_btnDelete->setObjectName(QStringLiteral("m_btnDelete"));

        horizontalLayout->addWidget(m_btnDelete);


        verticalLayout->addWidget(widget);


        gridLayout->addWidget(widget_2, 0, 0, 1, 1);


        retranslateUi(OmniversePresetAddOrModifyDialog);

        QMetaObject::connectSlotsByName(OmniversePresetAddOrModifyDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniversePresetAddOrModifyDialog)
    {
        OmniversePresetAddOrModifyDialog->setWindowTitle(QString());
        label->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Name : ", Q_NULLPTR));
        m_btnApply->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Apply", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Cancel", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("OmniversePresetAddOrModifyDialog", "Mesh, Material Info", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("OmniversePresetAddOrModifyDialog", "Prim Info", Q_NULLPTR));
        m_btnLoadUSD->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Load USD", Q_NULLPTR));
        m_btnAdd->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Add", Q_NULLPTR));
        m_btnDelete->setText(QApplication::translate("OmniversePresetAddOrModifyDialog", "Delete", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniversePresetAddOrModifyDialog: public Ui_OmniversePresetAddOrModifyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSEPRESETADDORMODIFYDIALOG_H
