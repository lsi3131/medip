/********************************************************************************
** Form generated from reading UI file 'OmniversePresetDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSEPRESETDIALOG_H
#define UI_OMNIVERSEPRESETDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniversePresetDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout;
    QWidget *widget1;
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QTreeWidget *m_treePresetWidget;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *m_btnAdd;
    QPushButton *m_btnModify;
    QPushButton *m_btnDelete;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_4;
    QWidget *widget_6;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QTreeWidget *m_treeMeshToMaterialWidget;
    QWidget *widget_7;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QTreeWidget *m_treeUsdPrimWidget;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_6;
    QLabel *label_7;
    QComboBox *m_cboRenderSetting_RenderMode;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QDoubleSpinBox *m_Hydra_DefaltWidth_SpinBox;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QDoubleSpinBox *m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red;
    QCheckBox *m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox;
    QDoubleSpinBox *m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue;
    QLabel *label_5;
    QDoubleSpinBox *m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green;
    QCheckBox *m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_5;
    QLabel *label_6;
    QDoubleSpinBox *m_Histogram_WhiteScale_SpinBox;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_8;
    QDoubleSpinBox *m_RenderSetting_Wireframe_Thickness_spinBox;
    QSpacerItem *verticalSpacer;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnApply;
    QPushButton *m_btnCancel;

    void setupUi(QDialog *OmniversePresetDialog)
    {
        if (OmniversePresetDialog->objectName().isEmpty())
            OmniversePresetDialog->setObjectName(QStringLiteral("OmniversePresetDialog"));
        OmniversePresetDialog->resize(1108, 532);
        gridLayout = new QGridLayout(OmniversePresetDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new QWidget(OmniversePresetDialog);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        widget_3 = new QWidget(widget);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout = new QHBoxLayout(widget_3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget1 = new QWidget(widget_3);
        widget1->setObjectName(QStringLiteral("widget1"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget1->sizePolicy().hasHeightForWidth());
        widget1->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(widget1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_3 = new QLabel(widget1);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        m_treePresetWidget = new QTreeWidget(widget1);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        m_treePresetWidget->setHeaderItem(__qtreewidgetitem);
        m_treePresetWidget->setObjectName(QStringLiteral("m_treePresetWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_treePresetWidget->sizePolicy().hasHeightForWidth());
        m_treePresetWidget->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(m_treePresetWidget);

        widget_5 = new QWidget(widget1);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        horizontalLayout_3 = new QHBoxLayout(widget_5);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        m_btnAdd = new QPushButton(widget_5);
        m_btnAdd->setObjectName(QStringLiteral("m_btnAdd"));

        horizontalLayout_3->addWidget(m_btnAdd);

        m_btnModify = new QPushButton(widget_5);
        m_btnModify->setObjectName(QStringLiteral("m_btnModify"));

        horizontalLayout_3->addWidget(m_btnModify);

        m_btnDelete = new QPushButton(widget_5);
        m_btnDelete->setObjectName(QStringLiteral("m_btnDelete"));

        horizontalLayout_3->addWidget(m_btnDelete);


        verticalLayout->addWidget(widget_5);


        horizontalLayout->addWidget(widget1);

        widget_2 = new QWidget(widget_3);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(4);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy2);
        horizontalLayout_4 = new QHBoxLayout(widget_2);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        widget_6 = new QWidget(widget_2);
        widget_6->setObjectName(QStringLiteral("widget_6"));
        verticalLayout_4 = new QVBoxLayout(widget_6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label = new QLabel(widget_6);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_4->addWidget(label);

        m_treeMeshToMaterialWidget = new QTreeWidget(widget_6);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        m_treeMeshToMaterialWidget->setHeaderItem(__qtreewidgetitem1);
        m_treeMeshToMaterialWidget->setObjectName(QStringLiteral("m_treeMeshToMaterialWidget"));

        verticalLayout_4->addWidget(m_treeMeshToMaterialWidget);


        horizontalLayout_4->addWidget(widget_6);

        widget_7 = new QWidget(widget_2);
        widget_7->setObjectName(QStringLiteral("widget_7"));
        verticalLayout_3 = new QVBoxLayout(widget_7);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_2 = new QLabel(widget_7);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_3->addWidget(label_2);

        m_treeUsdPrimWidget = new QTreeWidget(widget_7);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(0, QStringLiteral("1"));
        m_treeUsdPrimWidget->setHeaderItem(__qtreewidgetitem2);
        m_treeUsdPrimWidget->setObjectName(QStringLiteral("m_treeUsdPrimWidget"));

        verticalLayout_3->addWidget(m_treeUsdPrimWidget);


        horizontalLayout_4->addWidget(widget_7);

        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_5 = new QVBoxLayout(groupBox);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox_6 = new QGroupBox(groupBox);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_6 = new QGridLayout(groupBox_6);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_7 = new QLabel(groupBox_6);
        label_7->setObjectName(QStringLiteral("label_7"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy3);

        gridLayout_6->addWidget(label_7, 0, 0, 1, 1);

        m_cboRenderSetting_RenderMode = new QComboBox(groupBox_6);
        m_cboRenderSetting_RenderMode->setObjectName(QStringLiteral("m_cboRenderSetting_RenderMode"));

        gridLayout_6->addWidget(m_cboRenderSetting_RenderMode, 0, 1, 1, 1);


        verticalLayout_5->addWidget(groupBox_6);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy3.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        m_Hydra_DefaltWidth_SpinBox = new QDoubleSpinBox(groupBox_2);
        m_Hydra_DefaltWidth_SpinBox->setObjectName(QStringLiteral("m_Hydra_DefaltWidth_SpinBox"));

        gridLayout_2->addWidget(m_Hydra_DefaltWidth_SpinBox, 0, 1, 1, 1);


        verticalLayout_5->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_3 = new QGridLayout(groupBox_3);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox_4 = new QGroupBox(groupBox_3);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red = new QDoubleSpinBox(groupBox_4);
        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red->setObjectName(QStringLiteral("m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red"));

        gridLayout_4->addWidget(m_Post_BackgroundAlpha_DefaultColor_SpinBox_Red, 2, 1, 1, 1);

        m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox = new QCheckBox(groupBox_4);
        m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox->setObjectName(QStringLiteral("m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox"));

        gridLayout_4->addWidget(m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox, 1, 0, 1, 1);

        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue = new QDoubleSpinBox(groupBox_4);
        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue->setObjectName(QStringLiteral("m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue"));

        gridLayout_4->addWidget(m_Post_BackgroundAlpha_DefaultColor_SpinBox_Blue, 2, 3, 1, 1);

        label_5 = new QLabel(groupBox_4);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_4->addWidget(label_5, 2, 0, 1, 1);

        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green = new QDoubleSpinBox(groupBox_4);
        m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green->setObjectName(QStringLiteral("m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green"));

        gridLayout_4->addWidget(m_Post_BackgroundAlpha_DefaultColor_SpinBox_Green, 2, 2, 1, 1);

        m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox = new QCheckBox(groupBox_4);
        m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox->setObjectName(QStringLiteral("m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox"));

        gridLayout_4->addWidget(m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_4, 0, 0, 1, 1);

        groupBox_5 = new QGroupBox(groupBox_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_5 = new QGridLayout(groupBox_5);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy3.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy3);

        gridLayout_5->addWidget(label_6, 0, 0, 1, 1);

        m_Histogram_WhiteScale_SpinBox = new QDoubleSpinBox(groupBox_5);
        m_Histogram_WhiteScale_SpinBox->setObjectName(QStringLiteral("m_Histogram_WhiteScale_SpinBox"));

        gridLayout_5->addWidget(m_Histogram_WhiteScale_SpinBox, 0, 1, 1, 1);


        gridLayout_3->addWidget(groupBox_5, 1, 0, 1, 1);

        groupBox_7 = new QGroupBox(groupBox_3);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        horizontalLayout_5 = new QHBoxLayout(groupBox_7);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_8 = new QLabel(groupBox_7);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy3.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy3);

        horizontalLayout_5->addWidget(label_8);

        m_RenderSetting_Wireframe_Thickness_spinBox = new QDoubleSpinBox(groupBox_7);
        m_RenderSetting_Wireframe_Thickness_spinBox->setObjectName(QStringLiteral("m_RenderSetting_Wireframe_Thickness_spinBox"));

        horizontalLayout_5->addWidget(m_RenderSetting_Wireframe_Thickness_spinBox);


        gridLayout_3->addWidget(groupBox_7, 2, 0, 1, 1);


        verticalLayout_5->addWidget(groupBox_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);


        horizontalLayout_4->addWidget(groupBox);


        horizontalLayout->addWidget(widget_2);


        verticalLayout_2->addWidget(widget_3);


        gridLayout->addWidget(widget, 0, 0, 1, 1);

        widget_4 = new QWidget(OmniversePresetDialog);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        horizontalLayout_2 = new QHBoxLayout(widget_4);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        m_btnApply = new QPushButton(widget_4);
        m_btnApply->setObjectName(QStringLiteral("m_btnApply"));

        horizontalLayout_2->addWidget(m_btnApply);

        m_btnCancel = new QPushButton(widget_4);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout_2->addWidget(m_btnCancel);


        gridLayout->addWidget(widget_4, 1, 0, 1, 1);


        retranslateUi(OmniversePresetDialog);

        QMetaObject::connectSlotsByName(OmniversePresetDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniversePresetDialog)
    {
        OmniversePresetDialog->setWindowTitle(QString());
        label_3->setText(QApplication::translate("OmniversePresetDialog", "Preset", Q_NULLPTR));
        m_btnAdd->setText(QApplication::translate("OmniversePresetDialog", "Add", Q_NULLPTR));
        m_btnModify->setText(QApplication::translate("OmniversePresetDialog", "Modify", Q_NULLPTR));
        m_btnDelete->setText(QApplication::translate("OmniversePresetDialog", "Delete", Q_NULLPTR));
        label->setText(QApplication::translate("OmniversePresetDialog", "Mesh, Material Bind", Q_NULLPTR));
        label_2->setText(QApplication::translate("OmniversePresetDialog", "Usd Prim", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("OmniversePresetDialog", "Render Setting", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("OmniversePresetDialog", "common", Q_NULLPTR));
        label_7->setText(QApplication::translate("OmniversePresetDialog", "render mode : ", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("OmniversePresetDialog", "hydra", Q_NULLPTR));
        label_4->setText(QApplication::translate("OmniversePresetDialog", "default width :", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("OmniversePresetDialog", "post processing", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("OmniversePresetDialog", "Backgroun Zero Alpha", Q_NULLPTR));
        m_Post_BackgroundAlpha_DefaultColor_Composite_ChkBox->setText(QApplication::translate("OmniversePresetDialog", "Composite", Q_NULLPTR));
        label_5->setText(QApplication::translate("OmniversePresetDialog", "Default Color :", Q_NULLPTR));
        m_Post_BackgroundAlpha_DefaultColor_Enable_ChkBox->setText(QApplication::translate("OmniversePresetDialog", "Enable", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("OmniversePresetDialog", "histogram", Q_NULLPTR));
        label_6->setText(QApplication::translate("OmniversePresetDialog", "white scale :", Q_NULLPTR));
        groupBox_7->setTitle(QApplication::translate("OmniversePresetDialog", "wire frame", Q_NULLPTR));
        label_8->setText(QApplication::translate("OmniversePresetDialog", "thickness : ", Q_NULLPTR));
        m_btnApply->setText(QApplication::translate("OmniversePresetDialog", "Apply", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniversePresetDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniversePresetDialog: public Ui_OmniversePresetDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSEPRESETDIALOG_H
