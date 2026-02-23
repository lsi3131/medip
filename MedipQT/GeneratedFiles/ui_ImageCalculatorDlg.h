/********************************************************************************
** Form generated from reading UI file 'ImageCalculatorDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGECALCULATORDLG_H
#define UI_IMAGECALCULATORDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageCalculatorDlg
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *m_chkPreview;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QRadioButton *m_rdoDivide;
    QRadioButton *m_rdoMinus;
    QRadioButton *m_rdoMultiply;
    QRadioButton *m_rdoPlus;
    QRadioButton *m_rdoReplace;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *m_btnOK;
    QPushButton *m_btnCancel;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QDoubleSpinBox *m_spinValue;
    QScrollBar *m_scrollValue;

    void setupUi(QDialog *ImageCalculatorDlg)
    {
        if (ImageCalculatorDlg->objectName().isEmpty())
            ImageCalculatorDlg->setObjectName(QStringLiteral("ImageCalculatorDlg"));
        ImageCalculatorDlg->resize(266, 279);
        gridLayout = new QGridLayout(ImageCalculatorDlg);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_3 = new QWidget(ImageCalculatorDlg);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(40, -1, 40, -1);
        m_chkPreview = new QCheckBox(widget_3);
        m_chkPreview->setObjectName(QStringLiteral("m_chkPreview"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_chkPreview->sizePolicy().hasHeightForWidth());
        m_chkPreview->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(m_chkPreview, 0, Qt::AlignHCenter);


        gridLayout->addWidget(widget_3, 3, 0, 1, 1);

        widget = new QWidget(ImageCalculatorDlg);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(70, -1, 40, -1);
        m_rdoDivide = new QRadioButton(widget);
        m_rdoDivide->setObjectName(QStringLiteral("m_rdoDivide"));
        QFont font;
        font.setPointSize(20);
        m_rdoDivide->setFont(font);

        gridLayout_2->addWidget(m_rdoDivide, 2, 2, 1, 1);

        m_rdoMinus = new QRadioButton(widget);
        m_rdoMinus->setObjectName(QStringLiteral("m_rdoMinus"));
        m_rdoMinus->setFont(font);

        gridLayout_2->addWidget(m_rdoMinus, 1, 2, 1, 1);

        m_rdoMultiply = new QRadioButton(widget);
        m_rdoMultiply->setObjectName(QStringLiteral("m_rdoMultiply"));
        m_rdoMultiply->setFont(font);

        gridLayout_2->addWidget(m_rdoMultiply, 2, 1, 1, 1);

        m_rdoPlus = new QRadioButton(widget);
        m_rdoPlus->setObjectName(QStringLiteral("m_rdoPlus"));
        m_rdoPlus->setFont(font);

        gridLayout_2->addWidget(m_rdoPlus, 1, 1, 1, 1);

        m_rdoReplace = new QRadioButton(widget);
        m_rdoReplace->setObjectName(QStringLiteral("m_rdoReplace"));
        m_rdoReplace->setFont(font);

        gridLayout_2->addWidget(m_rdoReplace, 3, 1, 1, 1);


        gridLayout->addWidget(widget, 0, 0, 1, 1);

        widget_4 = new QWidget(ImageCalculatorDlg);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        sizePolicy1.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(widget_4);
        horizontalLayout_2->setSpacing(20);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(30, -1, 30, -1);
        m_btnOK = new QPushButton(widget_4);
        m_btnOK->setObjectName(QStringLiteral("m_btnOK"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(m_btnOK->sizePolicy().hasHeightForWidth());
        m_btnOK->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(m_btnOK);

        m_btnCancel = new QPushButton(widget_4);
        m_btnCancel->setObjectName(QStringLiteral("m_btnCancel"));
        sizePolicy2.setHeightForWidth(m_btnCancel->sizePolicy().hasHeightForWidth());
        m_btnCancel->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(m_btnCancel);


        gridLayout->addWidget(widget_4, 4, 0, 1, 1);

        widget_2 = new QWidget(ImageCalculatorDlg);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_5 = new QWidget(widget_2);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        horizontalLayout_4 = new QHBoxLayout(widget_5);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(widget_5);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy3);

        horizontalLayout_4->addWidget(label);

        m_spinValue = new QDoubleSpinBox(widget_5);
        m_spinValue->setObjectName(QStringLiteral("m_spinValue"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(1);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(m_spinValue->sizePolicy().hasHeightForWidth());
        m_spinValue->setSizePolicy(sizePolicy4);
        m_spinValue->setMinimum(-9999.99);
        m_spinValue->setMaximum(9999.99);
        m_spinValue->setSingleStep(0.1);

        horizontalLayout_4->addWidget(m_spinValue);


        verticalLayout->addWidget(widget_5, 0, Qt::AlignHCenter);

        m_scrollValue = new QScrollBar(widget_2);
        m_scrollValue->setObjectName(QStringLiteral("m_scrollValue"));
        m_scrollValue->setMinimum(-999999);
        m_scrollValue->setMaximum(999999);
        m_scrollValue->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(m_scrollValue);


        gridLayout->addWidget(widget_2, 1, 0, 1, 1);

        QWidget::setTabOrder(m_rdoPlus, m_rdoMinus);
        QWidget::setTabOrder(m_rdoMinus, m_rdoMultiply);
        QWidget::setTabOrder(m_rdoMultiply, m_rdoDivide);
        QWidget::setTabOrder(m_rdoDivide, m_btnOK);
        QWidget::setTabOrder(m_btnOK, m_btnCancel);

        retranslateUi(ImageCalculatorDlg);

        QMetaObject::connectSlotsByName(ImageCalculatorDlg);
    } // setupUi

    void retranslateUi(QDialog *ImageCalculatorDlg)
    {
        ImageCalculatorDlg->setWindowTitle(QApplication::translate("ImageCalculatorDlg", "Image Calculator", Q_NULLPTR));
        m_chkPreview->setText(QApplication::translate("ImageCalculatorDlg", "Preview", Q_NULLPTR));
        m_rdoDivide->setText(QApplication::translate("ImageCalculatorDlg", "\303\267", Q_NULLPTR));
        m_rdoMinus->setText(QApplication::translate("ImageCalculatorDlg", "\357\274\215", Q_NULLPTR));
        m_rdoMultiply->setText(QApplication::translate("ImageCalculatorDlg", "\303\227", Q_NULLPTR));
        m_rdoPlus->setText(QApplication::translate("ImageCalculatorDlg", "\357\274\213", Q_NULLPTR));
        m_rdoReplace->setText(QApplication::translate("ImageCalculatorDlg", "\342\206\222", Q_NULLPTR));
        m_btnOK->setText(QApplication::translate("ImageCalculatorDlg", "OK", Q_NULLPTR));
        m_btnCancel->setText(QApplication::translate("ImageCalculatorDlg", "Cancel", Q_NULLPTR));
        label->setText(QApplication::translate("ImageCalculatorDlg", "Value : ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ImageCalculatorDlg: public Ui_ImageCalculatorDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGECALCULATORDLG_H
