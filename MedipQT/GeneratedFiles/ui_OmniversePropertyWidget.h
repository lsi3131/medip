/********************************************************************************
** Form generated from reading UI file 'OmniversePropertyWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSEPROPERTYWIDGET_H
#define UI_OMNIVERSEPROPERTYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniversePropertyWidget
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *m_cboMaterial;
    QGroupBox *m_groupPhysics;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_2;

    void setupUi(QWidget *OmniversePropertyWidget)
    {
        if (OmniversePropertyWidget->objectName().isEmpty())
            OmniversePropertyWidget->setObjectName(QStringLiteral("OmniversePropertyWidget"));
        OmniversePropertyWidget->resize(400, 300);
        verticalLayout_3 = new QVBoxLayout(OmniversePropertyWidget);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox = new QGroupBox(OmniversePropertyWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(groupBox);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(label);

        m_cboMaterial = new QComboBox(groupBox);
        m_cboMaterial->setObjectName(QStringLiteral("m_cboMaterial"));

        horizontalLayout_2->addWidget(m_cboMaterial);


        verticalLayout_3->addWidget(groupBox);

        m_groupPhysics = new QGroupBox(OmniversePropertyWidget);
        m_groupPhysics->setObjectName(QStringLiteral("m_groupPhysics"));
        verticalLayout_2 = new QVBoxLayout(m_groupPhysics);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        checkBox = new QCheckBox(m_groupPhysics);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout_2->addWidget(checkBox);

        groupBox_3 = new QGroupBox(m_groupPhysics);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout = new QVBoxLayout(groupBox_3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        checkBox_2 = new QCheckBox(groupBox_3);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout->addWidget(checkBox_2);


        verticalLayout_2->addWidget(groupBox_3);


        verticalLayout_3->addWidget(m_groupPhysics);


        retranslateUi(OmniversePropertyWidget);

        QMetaObject::connectSlotsByName(OmniversePropertyWidget);
    } // setupUi

    void retranslateUi(QWidget *OmniversePropertyWidget)
    {
        OmniversePropertyWidget->setWindowTitle(QApplication::translate("OmniversePropertyWidget", "Form", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("OmniversePropertyWidget", "Materials", Q_NULLPTR));
        label->setText(QApplication::translate("OmniversePropertyWidget", "materials", Q_NULLPTR));
        m_groupPhysics->setTitle(QApplication::translate("OmniversePropertyWidget", "Physics", Q_NULLPTR));
        checkBox->setText(QApplication::translate("OmniversePropertyWidget", "Enable Physics", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("OmniversePropertyWidget", "Deformable Body", Q_NULLPTR));
        checkBox_2->setText(QApplication::translate("OmniversePropertyWidget", "Deformable Enabled", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniversePropertyWidget: public Ui_OmniversePropertyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSEPROPERTYWIDGET_H
