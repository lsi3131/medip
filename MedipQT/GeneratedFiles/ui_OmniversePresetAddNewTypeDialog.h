/********************************************************************************
** Form generated from reading UI file 'OmniversePresetAddNewTypeDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSEPRESETADDNEWTYPEDIALOG_H
#define UI_OMNIVERSEPRESETADDNEWTYPEDIALOG_H

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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniversePresetAddNewTypeDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnAdd;
    QPushButton *m_btnCancel;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QComboBox *m_cboUsdPrimType;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QGroupBox *m_groupMaterial;
    QGridLayout *gridLayout_3;
    QLabel *label_4;
    QLineEdit *m_editBindMeshName;
    QLabel *label_2;
    QComboBox *m_cboMaterialType;
    QGroupBox *m_groupLight;
    QGridLayout *gridLayout_4;
    QLabel *label_5;
    QComboBox *comboBox_2;
    QLabel *label_3;
    QLineEdit *m_editLineName;

    void setupUi(QDialog *OmniversePresetAddNewTypeDialog)
    {
        if (OmniversePresetAddNewTypeDialog->objectName().isEmpty())
            OmniversePresetAddNewTypeDialog->setObjectName(QStringLiteral("OmniversePresetAddNewTypeDialog"));
        OmniversePresetAddNewTypeDialog->resize(334, 297);
        gridLayout = new QGridLayout(OmniversePresetAddNewTypeDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_4 = new QWidget(OmniversePresetAddNewTypeDialog);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widget_4);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        m_btnAdd = new QPushButton(widget_4);
        m_btnAdd->setObjectName(QStringLiteral("m_btnAdd"));

        horizontalLayout_2->addWidget(m_btnAdd);

        m_btnCancel = new QPushButton(widget_4);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout_2->addWidget(m_btnCancel);


        gridLayout->addWidget(widget_4, 5, 0, 1, 1);

        widget_2 = new QWidget(OmniversePresetAddNewTypeDialog);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        m_cboUsdPrimType = new QComboBox(widget_2);
        m_cboUsdPrimType->setObjectName(QStringLiteral("m_cboUsdPrimType"));

        gridLayout_2->addWidget(m_cboUsdPrimType, 0, 1, 1, 1);

        label = new QLabel(widget_2);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);


        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 0, 1, 1);

        m_groupMaterial = new QGroupBox(OmniversePresetAddNewTypeDialog);
        m_groupMaterial->setObjectName(QStringLiteral("m_groupMaterial"));
        gridLayout_3 = new QGridLayout(m_groupMaterial);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_4 = new QLabel(m_groupMaterial);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label_4, 1, 0, 1, 1);

        m_editBindMeshName = new QLineEdit(m_groupMaterial);
        m_editBindMeshName->setObjectName(QStringLiteral("m_editBindMeshName"));

        gridLayout_3->addWidget(m_editBindMeshName, 0, 2, 1, 1);

        label_2 = new QLabel(m_groupMaterial);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 0, 0, 1, 1);

        m_cboMaterialType = new QComboBox(m_groupMaterial);
        m_cboMaterialType->setObjectName(QStringLiteral("m_cboMaterialType"));

        gridLayout_3->addWidget(m_cboMaterialType, 1, 2, 1, 1);


        gridLayout->addWidget(m_groupMaterial, 2, 0, 1, 1);

        m_groupLight = new QGroupBox(OmniversePresetAddNewTypeDialog);
        m_groupLight->setObjectName(QStringLiteral("m_groupLight"));
        gridLayout_4 = new QGridLayout(m_groupLight);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_5 = new QLabel(m_groupLight);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_4->addWidget(label_5, 0, 0, 1, 1);

        comboBox_2 = new QComboBox(m_groupLight);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        gridLayout_4->addWidget(comboBox_2, 1, 2, 1, 1);

        label_3 = new QLabel(m_groupLight);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        gridLayout_4->addWidget(label_3, 1, 0, 1, 1);

        m_editLineName = new QLineEdit(m_groupLight);
        m_editLineName->setObjectName(QStringLiteral("m_editLineName"));

        gridLayout_4->addWidget(m_editLineName, 0, 2, 1, 1);


        gridLayout->addWidget(m_groupLight, 3, 0, 1, 1);


        retranslateUi(OmniversePresetAddNewTypeDialog);

        QMetaObject::connectSlotsByName(OmniversePresetAddNewTypeDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniversePresetAddNewTypeDialog)
    {
        OmniversePresetAddNewTypeDialog->setWindowTitle(QString());
        m_btnAdd->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Add", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Cancel", Q_NULLPTR));
        label->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "USD Prim Type :", Q_NULLPTR));
        m_groupMaterial->setTitle(QApplication::translate("OmniversePresetAddNewTypeDialog", "Material", Q_NULLPTR));
        label_4->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Type : ", Q_NULLPTR));
        label_2->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Bind Mesh Name : ", Q_NULLPTR));
        m_groupLight->setTitle(QApplication::translate("OmniversePresetAddNewTypeDialog", "Light", Q_NULLPTR));
        label_5->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Name : ", Q_NULLPTR));
        label_3->setText(QApplication::translate("OmniversePresetAddNewTypeDialog", "Type : ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniversePresetAddNewTypeDialog: public Ui_OmniversePresetAddNewTypeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSEPRESETADDNEWTYPEDIALOG_H
