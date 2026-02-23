/********************************************************************************
** Form generated from reading UI file 'OmniverseFileViewDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OMNIVERSEFILEVIEWDIALOG_H
#define UI_OMNIVERSEFILEVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OmniverseFileViewDialog
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *m_treeDirectoryView;
    QTreeWidget *m_treeFileView;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *m_editFileName;
    QComboBox *m_cboFileExtension;
    QPushButton *m_btnExport;
    QPushButton *m_btnCancel;

    void setupUi(QDialog *OmniverseFileViewDialog)
    {
        if (OmniverseFileViewDialog->objectName().isEmpty())
            OmniverseFileViewDialog->setObjectName(QStringLiteral("OmniverseFileViewDialog"));
        OmniverseFileViewDialog->resize(807, 418);
        gridLayout = new QGridLayout(OmniverseFileViewDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_2 = new QWidget(OmniverseFileViewDialog);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        m_treeDirectoryView = new QTreeWidget(widget_2);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        m_treeDirectoryView->setHeaderItem(__qtreewidgetitem);
        m_treeDirectoryView->setObjectName(QStringLiteral("m_treeDirectoryView"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_treeDirectoryView->sizePolicy().hasHeightForWidth());
        m_treeDirectoryView->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(m_treeDirectoryView);

        m_treeFileView = new QTreeWidget(widget_2);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        m_treeFileView->setHeaderItem(__qtreewidgetitem1);
        m_treeFileView->setObjectName(QStringLiteral("m_treeFileView"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_treeFileView->sizePolicy().hasHeightForWidth());
        m_treeFileView->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(m_treeFileView);


        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        widget_3 = new QWidget(OmniverseFileViewDialog);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_2 = new QHBoxLayout(widget_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(widget_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        m_editFileName = new QLineEdit(widget_3);
        m_editFileName->setObjectName(QStringLiteral("m_editFileName"));

        horizontalLayout_2->addWidget(m_editFileName);

        m_cboFileExtension = new QComboBox(widget_3);
        m_cboFileExtension->setObjectName(QStringLiteral("m_cboFileExtension"));

        horizontalLayout_2->addWidget(m_cboFileExtension);

        m_btnExport = new QPushButton(widget_3);
        m_btnExport->setObjectName(QStringLiteral("m_btnExport"));

        horizontalLayout_2->addWidget(m_btnExport);

        m_btnCancel = new QPushButton(widget_3);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));

        horizontalLayout_2->addWidget(m_btnCancel);


        gridLayout->addWidget(widget_3, 1, 0, 1, 1);


        retranslateUi(OmniverseFileViewDialog);

        QMetaObject::connectSlotsByName(OmniverseFileViewDialog);
    } // setupUi

    void retranslateUi(QDialog *OmniverseFileViewDialog)
    {
        OmniverseFileViewDialog->setWindowTitle(QString());
        label_2->setText(QApplication::translate("OmniverseFileViewDialog", "File Name", Q_NULLPTR));
        m_btnExport->setText(QApplication::translate("OmniverseFileViewDialog", "Export", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("OmniverseFileViewDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OmniverseFileViewDialog: public Ui_OmniverseFileViewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OMNIVERSEFILEVIEWDIALOG_H
