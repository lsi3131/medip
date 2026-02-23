/********************************************************************************
** Form generated from reading UI file 'OpenSourceLicenseDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENSOURCELICENSEDLG_H
#define UI_OPENSOURCELICENSEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OpenSourceLicenseDlg
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QTextBrowser *m_textBrowser;

    void setupUi(QDialog *OpenSourceLicenseDlg)
    {
        if (OpenSourceLicenseDlg->objectName().isEmpty())
            OpenSourceLicenseDlg->setObjectName(QStringLiteral("OpenSourceLicenseDlg"));
        OpenSourceLicenseDlg->resize(559, 630);
        gridLayout = new QGridLayout(OpenSourceLicenseDlg);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_2 = new QWidget(OpenSourceLicenseDlg);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);

        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        m_textBrowser = new QTextBrowser(OpenSourceLicenseDlg);
        m_textBrowser->setObjectName(QStringLiteral("m_textBrowser"));

        gridLayout->addWidget(m_textBrowser, 1, 0, 1, 1);


        retranslateUi(OpenSourceLicenseDlg);

        QMetaObject::connectSlotsByName(OpenSourceLicenseDlg);
    } // setupUi

    void retranslateUi(QDialog *OpenSourceLicenseDlg)
    {
        OpenSourceLicenseDlg->setWindowTitle(QApplication::translate("OpenSourceLicenseDlg", "Image Calculator", Q_NULLPTR));
        m_textBrowser->setHtml(QApplication::translate("OpenSourceLicenseDlg", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Gulim'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OpenSourceLicenseDlg: public Ui_OpenSourceLicenseDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENSOURCELICENSEDLG_H
