#pragma once

#ifndef DRAWCUTTAB_H
#define DRAWCUTTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include "color.h"
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>
#include <QLineEdit>
#include <QAction>
#include <QButtonGroup>
#include <QVector>
#include <QCheckBox>

class DrawcutTab : public CollapseWidget
{
	Q_OBJECT

private:
	QPushButton* btnDrawCut;
	QPushButton* btnApply;

	QComboBox* comboDrawing[2];//0:right, 1:left

public:
	explicit DrawcutTab(QWidget* parent = NULL);

	double		getLambda();
	void		changeSeedColor(int, COLOR);
	void		UpdateSeed(bool init = false);
	void		UpdateVoxel();
	void		drawCutOnOff(bool val);

	void		ClearSeed();
	void		AddSeed(int uid);
	void		AddSeed(std::vector<muint32> uid);
	void		DelSeed(int uid);
	void		DelSeed(std::vector<muint32> uid);
	void		ChangeSeed(int oldUID, int newUID);
	bool		contains(int seedUID);

	void		setDrawCutMode(DRAW_CUT_SHAPE shape);

private:
	QTreeWidget* m_listSeed = nullptr;
	QLabel* m_labelSeed = nullptr;
	QPushButton* m_btnFitbox = nullptr;
	QSlider* m_slideLambda = nullptr;
	QLineEdit* m_textLambda = nullptr;
	QAction* m_actClear = nullptr;
	QAction* m_actDelSeed = nullptr;
	QButtonGroup* m_grCursor = nullptr;
	QVector<muint32> m_seedUID;//layer uid

							   //
	QCheckBox* m_cbTargeted;


protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void	slot_OnCursorChanged(int);
	void	slot_OnReset();
	void	slot_OnSliderMoved(int val);
	void	slot_OnTextChanged(const QString& txt);
	void	slot_OnTextEditFinished();
	void	slot_OnSeedDelete();//use only multi-drawcut mode
	void	slot_OnColumnClicked(QTreeWidgetItem* item, int column);
	void	slot_OnColumnDoubleClicked(QTreeWidgetItem* item, int column);
	void	slot_OnContext(const QPoint& pos);
	void	slot_OnFitBoundingBox();
	void	slot_OnSelectChange(QTreeWidgetItem*, QTreeWidgetItem*);
	void	slot_OnSketchDrawSegmentation();
	void	slot_OnApply();
	void	slot_OnClear();

	void	slot_OnDrawingLayerChanged(int _index);
};
#endif