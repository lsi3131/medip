#pragma once

#ifndef CONFUSION_MATRIX_INFO_DLG_H
#define CONFUSION_MATRIX_INFO_DLG_H

#include <qtreewidget.h>

class ConfusionMatrixInfoDialog : public QTreeWidget
{
	Q_OBJECT

private:
	ConfusionMatrixInfoDialog(const ConfusionMatrixInfoDialog& copy) {}
	const ConfusionMatrixInfoDialog& operator=(const ConfusionMatrixInfoDialog& rhs) {}

public:
	explicit ConfusionMatrixInfoDialog(QWidget* parent = nullptr);
	~ConfusionMatrixInfoDialog();

	void setConfusionMatrix(int tp, int tn, int fp, int fn);
	void calculateInfo();

private:
	int TP;
	int TN;
	int FP;
	int FN;
};
#endif