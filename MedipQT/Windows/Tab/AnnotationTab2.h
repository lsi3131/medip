#pragma once

#ifndef ANIMATIONTAB2_H
#define ANIMATIONTAB2_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include <QTreeWidget>
#include <QPushButton>

class AnnotationTab2 : public CollapseWidget
{
	Q_OBJECT

public:
	explicit AnnotationTab2(QWidget* parent = NULL);

	QTreeWidget* m_annoList;

	void UpdateAnnoList(MAINTAB_TYPE type) { AddAnnoList(type); }

	void setOpenState(MAINTAB_TYPE);
	void updateOpenState(MAINTAB_TYPE);
protected:
	bool eventFilter(QObject* watched, QEvent* e) override;

private:
	bool m_SegOpen;
	bool m_MeasureOpen;//TODO
	QPushButton* m_btnDel;
private:
	void AddAnnoList(MAINTAB_TYPE Maintype);
	void UpdateAnnoItem(QTreeWidgetItem*, int);
private slots:
	void slot_OnColumnClick(QTreeWidgetItem* item, int column);
	void slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column);
	void slot_OnAnnoDel();
};
#endif