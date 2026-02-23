#pragma once

#ifndef SUMMARYTAB_H
#define SUMMARYTAB_H

#include "CollapseWidget.h"
#include "DicomReader.h"
#include <QPushButton>
#include <QTreeWidget>

class SummaryTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit SummaryTab(QWidget* parent = NULL);

public:
	void setSummary();

private:
	void AddItemToList(QString name, QString value);
	QString GetOrientationText();
	bool IsUploadDicomDataNotExist();

private slots:
	void slot_OnCopy();
	void slot_OnUpdateInfo();

private:
	QTreeWidget* m_infoList;
	QPushButton* m_btnCopy;
	QPushButton* m_btnUpdateInfo;

	DcmtkSeriesInfo m_originSeriesInfo;
	bool m_isFirstSeriesInfoInitialized;
};
#endif

