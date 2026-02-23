#pragma once

#ifndef CLINICAL_INFORMATION_DLG_H
#define CLINICAL_INFORMATION_DLG_H

#include <QDialog>

class ClinicalInfoTableWidget;

typedef enum {
	eCIDMGlobal,
	eCIDMLocal,
} eClinicalInformationDlgMode;

class ClinicalInformationDlg : public QDialog
{
	Q_OBJECT
public slots:
	bool makeClinicalInfo();
	void cancelClinicalInfo();
//	void autoSelection();

public:
	ClinicalInformationDlg(eClinicalInformationDlgMode _eDlgMode = eCIDMGlobal, QDialog *parent = nullptr);
	~ClinicalInformationDlg();

	void setBasicTableData(QVector<QPair<QString, int>> &maskNameToVoxelCountVec);

	QVector<QPair<QString, QString>> &getClinicalInfo()							{ return clinicalInfo; }
	QVector<sLocalClinicalInfo> &getLocalClinicalInfo()							{ return localClinicalInfo; }

private:
	ClinicalInfoTableWidget *tableWidget;
	QCheckBox *chkBox_autoSelection;

	QVector<QPair<QString, QString>> clinicalInfo;				// global clinical info.
	QVector<sLocalClinicalInfo> localClinicalInfo;			// local clinical info.
	eClinicalInformationDlgMode m_eDlgMode;
};
#endif