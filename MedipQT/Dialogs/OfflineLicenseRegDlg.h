#pragma once

#ifndef OFFLINE_LICENSE_REG_DLG_H
#define OFFLINE_LICENSE_REG_DLG_H

#include <QDialog>
#include "MedipType.h"

class OfflineLicenseRegDlg : public QDialog
{
	Q_OBJECT

public:
	OfflineLicenseRegDlg(QWidget *parent = Q_NULLPTR);
	~OfflineLicenseRegDlg();

	void setFunctionTreeData();
	void makeFunctionTreeList(QTreeWidget **curTreeWidget);
	void getCheckedFunctionTreeList(QVector<qulonglong> &functionList, QTreeWidget *curTreeWidget);
	void getCheckedFunctionTreeList(QVector<qulonglong> &functionList, QMap<qulonglong, int> &usableCountMap, QTreeWidget *curTreeWidget);

	void addPluginProductTypeFunctionLevel(QVector<qulonglong> &functionList, QMap<qulonglong, int> usableCountMap);

	void makeFunctionListString(QVector<qulonglong> &functionList, QString &strOutput);

	bool GetMacAddress();

	void makeFunctionLevelIDEnumVector();

protected:
	void closeEvent(QCloseEvent*) override;

public slots:
	void OnMakePCcode(void);
	void OnAuthActivate(void);
	void OnAuthInfoSave();

	void OnitemChanged_FunctionList(QTreeWidgetItem* item, int column);

private:
	// authentication info
	//// pc info
	QComboBox *m_combo_PCcode = nullptr;
	QLineEdit *m_lineEdit_PCcode = nullptr;

	//// private info
	QLineEdit *m_lineEdit_Auth_ID = nullptr;
	QLineEdit *m_lineEdit_Auth_PWD = nullptr;
	QLineEdit *m_lineEdit_Auth_Period_StartDate = nullptr;
	QLineEdit *m_lineEdit_Auth_Period_EndDate = nullptr;
	QCheckBox *m_check_Auth_TrialVer = nullptr;
	QComboBox *m_combo_Auth_Product_Type = nullptr;
	QComboBox *m_combo_Auth_License_Type = nullptr;
	QTreeWidget *m_treeWidget_Auth_FunctionList = nullptr;

//	QMultiMap<QString, QVector<sFunctionData>>		mapFunctions;
	QList<QPair<QString, QVector<sFunctionData>>>	listFunctions;
	QMap<QString, QString>							mapNicNameToMacAddr;

	QVector<eMEDIP_FUNCTION_LEVEL> vectorFunctionLevelIDEnum;
	QMap<eMEDIP_FUNCTION_LEVEL, QString> mapFunctionLevelIDToTooltipStr;

};
#endif