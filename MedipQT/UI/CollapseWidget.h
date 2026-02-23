#pragma once

#ifndef COLLAPSEWIDGET_H
#define COLLAPSEWIDGET_H

#include "define.h"
#include <QWidget>
#include "qvector.h"

class QToolButton;
class QFrame;
class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;
class QScrollArea;
//class QParallelAnimationGroup;

class CollapseWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CollapseWidget(const QString& title, QWidget *parent = NULL);
	
	void addWidget(QWidget *child, int nRow = 0, int stretch=0,QMargins margins=QMargins(),Qt::Alignment align=Qt::Alignment(), bool isVerti=false); //nRow -> 순서대로 증가
	void delete_addWidget(QWidget *child, int nRow = 0, int stretch = 0, QMargins margins = QMargins(), Qt::Alignment align = Qt::Alignment(), bool isVerti = false); //nRow -> 순서대로 증가	

	void updateUI();

	void setOpenWidget(bool val);
	QWidget * getEmptyBox() { return m_emptyBox; };
	QToolButton* getBtnCollapse() { return m_TbtnCollapse; };

	void SetContainerEnable(bool enable);

protected:
	void setTitle(const QString &title);
	void updateHeight();
//	void setOpenWidget(bool val);
	bool isCollapse();
	void createFrameLine(int nRow);

	QBoxLayout* getLayout(int nRow);
	QScrollArea *m_scrollArea;
	QToolButton* m_TbtnCollapse;
private:
	QFrame*		m_frameLine;
	QVector<QBoxLayout*> m_vecLay;
	QVBoxLayout *m_vlayoutWidgets;
	QWidget *m_widMain;
	QWidget *m_emptyBox;
	bool m_bDutyCheck;
//	QParallelAnimationGroup *toggleAnimation;
protected slots:
	virtual void OnCollapse();

};
#endif
