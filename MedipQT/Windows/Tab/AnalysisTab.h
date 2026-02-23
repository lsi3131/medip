#pragma once

#ifndef ANAYSISTAB_H
#define ANAYSISTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include <QTreeWidget>
#include <QLabel>

class AnalysisTab : public CollapseWidget
{
	Q_OBJECT

public:
	AnalysisTab(QWidget* parent = NULL);

	void ClearResult();
	void SetResultFromFile(int uid, int fileSeq);
	void MacroSave(QString fileName, int num);
	void MacroSaveHisto(QString fileName, int num);
	void UpdateName(int uid, QString _name);
	void ChangeUID(int pre_uid, int chg_uid);

	// 대분류 결과, histogram 결과
	void SetResult(int uid, QString str, int fileSeq = -1, QVector<QPointF>* _p = NULL, QVector<QPointF>* _lp = NULL);
	void SetResult(int uid, QString _name, mint16, mint16, QVector<QPointF>, QVector<QPointF>, int fileSeq = -1);
	void SetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable, int index, bool bUID);
	void DeleteResult(int index, bool bUID);

	int existResult(int index, bool bUID);

	bool GetResult(mint16&, mint16&, QString&, QVector<QPointF>&, QVector<QPointF>&, int index, bool bUID);
	bool GetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable, int index, bool bUID);
	void InitTreeWidget(int index, bool bUID);
	QTreeWidget* getTreeWidget(int index, bool bUID);

	// 세분류 결과 함수들
	//void SetSubResult();

public slots:
	void slot_DeleteResult(int index);
	void slot_OnCurrentChanged(int index);
	void slot_OntabCloseRequested(int index);

	//private:
public:
	QTabWidget* tabList;
	QLabel* m_labelDefault;
};

#endif