#pragma once

#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H

#include "defineMEDIP.h"
#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QTreeWidget>
#include <vector>

/*
@brief		리스트 위젯
@author		허 건 과장
@date		2021.12.09
*/
class VOLUME_DATA;

class CListWidget : public QTreeWidget
{
	Q_OBJECT

public:
	CListWidget(QWidget* parent = nullptr, VOLUME_DATA* p_volume_data = nullptr);
	virtual ~CListWidget();

	void getSeletedList(QList<muint32>& _list);

	void update(QList<QTreeWidgetItem*>& _vt_list, QList<muint32>& _vt_uid);
	void selectionRefresh(int index_dest);
	void selectionUpdate(int index_dest);
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	VOLUME_DATA* m_p_VolumeData;
	std::vector<muint32> m_vt_UID;
};

#endif