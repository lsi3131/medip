#include "stdafx.h"
#include "ConfusionMatrixInfoDialog.h"



ConfusionMatrixInfoDialog::ConfusionMatrixInfoDialog(QWidget* parent)
	: QTreeWidget(parent), TP(0), TN(0), FP(0), FN(0)
{
	setWindowFlags(Qt::Dialog);
	setRootIsDecorated(false);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	//confusionInfo->setStyleSheet(
	//	"QHeaderView::section {"
	//	"background-color:black"
	//	"}"
	//	"QTreeWidget {"
	//	"background-color: rgba(48, 48, 48, 255);"
	//	"show-decoration-selected: 1;"
	//	"}"
	//	"QToolTip {"
	//	"color: rgba(48, 48, 48, 255);"
	//	"}"
	//	"QLineEdit {"
	//	"background-color: rgba(48, 48, 48, 255);"
	//	"}"
	//);
	//vLayout->addWidget(m_textList);

	QStringList headerLabels;
	headerLabels.push_back(tr("Info"));
	headerLabels.push_back(tr("Value"));
	setHeaderLabels(headerLabels);
	setColumnCount(headerLabels.count());

	//QTreeWidgetItem *item = new QTreeWidgetItem(confusionInfo);
	//item->setText(0, "TPR");
	//item->setText(1, "213");
	//confusionInfo->setCurrentItem(item);
}


ConfusionMatrixInfoDialog::~ConfusionMatrixInfoDialog()
{
}

void ConfusionMatrixInfoDialog::setConfusionMatrix(int tp, int tn, int fp, int fn)
{
	TP = tp;
	TN = tn;
	FP = fp;
	FN = fn;
}

void ConfusionMatrixInfoDialog::calculateInfo()
{

}