#pragma once

#ifndef ANIMATIONTAB_H
#define ANIMATIONTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include <QTreeWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>

class AnimationTab : public CollapseWidget 
{
	Q_OBJECT

public:
	explicit AnimationTab(QWidget* parent = NULL);

	void	AddAnipoint(int, int group = 0, QTreeWidgetItem* item = NULL); // add a points
	void	UpdateLength(int group);
	void	UpdateAniList(); // del or all list update
	void	ClearAniList(); // clear points

	void	StopAnimation();
	void	CompleteAnimation();

	int		getCurrentGroup() { return m_group; }

	bool	isSpiral();

	QTreeWidget* takeList(int group = 0);
	void				InsertList(QTreeWidget*, int group = 0);
	QTreeWidgetItem* takeItem(int index, int group = 0);

protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

private:
	QPushButton* m_btnCreate;
	QPushButton* m_btnRepeat;
	QPushButton* m_btnPlay;
	QTimer* m_AniTimer;
	QComboBox* m_comboSpeed;
	QTreeWidget* m_listDefault;
	QTabWidget* tabList;
	QCheckBox* m_chkSpiral;
	int			m_group;
	int			m_nSec;
	QAction* m_actDel;
	QVector<QTreeWidget*>	m_vecPath;
	PLAYBACK_MODE m_playback;
	bool		m_Repeat; /*single,start : false, end : (default)false, (a-b)true*/
private:
	void	updateTabname();
	void	createNewTab();
	bool	isShowUsage();
	QVector3D getAnimationDistance(int index, float nDistance, float ttMSec, float dtMSec);
	float getAnimationTTDistance(int);

private slots:
	void	slot_OnContext(const QPoint& pos);
	void	slot_OnCreate(); //onappend
	void	slot_OnPlayback();
	void	slot_OnPlay();	//onstop
	void	slot_OnDelete();
	void	slot_OnAnimate();
	void	slot_OnSpiral();
	void	slot_OnTabClicked(int index);
	void	slot_OnTabClear(int index);
	void	slot_OnTabChanged(int index);
	void	slot_OnComboChanged(int index);
};
#endif