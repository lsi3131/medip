#pragma once

#ifndef ANALYSISWIDGE_H
#define ANALYSISWIDGE_H

#include "define.h"
#include <QWidget>
#include <QTableWidget>
#include <QStringList>
#include <vector>
#include <map>

#include "UI/TASubClassResultCtrl.h"
#include "Dialogs/HeightMapDlg.h"
#include "algorithm/MagicCut.h"
#include "ActionGMMExtractMask.h"

class LayerHistogram;
class QPushButton;
class QTreeWidget;
class QAction;
class QTreeWidgetItem;

using namespace std;

// itemdelegate.h
class ItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent)
	{
		setParent(parent);
	}

	bool eventFilter(QObject* object, QEvent* event) override
	{
		if (event->type() == QEvent::KeyPress) 
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			


		}
		return false;
	}

};

class AnalysisWidget : public QWidget
{
	Q_OBJECT

public:
	AnalysisWidget(VOLUME_DATA* pVolumeData, int uid,QWidget*parent = NULL);
	virtual ~AnalysisWidget();
	
	void init(QString, int fileSeq=-1);
	void InitTreeWidget();
	void Update(QString, mint16, mint16, QVector<QPointF>, QVector<QPointF>, int fileSeq=-1);
	void UpdateUID(int);
	bool getInfo(mint16 &, mint16 &, QString &,QVector<QPointF>&, QVector<QPointF>&);
	int	 getUID() { return m_uid; }

	void SetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable);
	bool getQTableInfo(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable);

	int		getSeq() { return m_fileSeq;	}

	QString getResult();

	void	OnSave();
	void	OnSaveMacro(QString fileName);
	void	OnSaveMacroHisto(QString fileName);

	void	invisibleIcon(void);
	bool	isVisible2DMap(void) { return m_bVisible2DMap; }

	float*	get3DVolumeMap(void) { return m_p3DVolumeMap; }
	std::vector<std::pair<float, float>> getColorCategory(void) { return m_vecColorCategory; }

	//
	void setVisibleShapeFeature(bool isShape2D);

	QTreeWidget* getTreeWidget() { return m_treeFeature;  }
	QTableWidget* getGMMTable() { return m_pGMMTableWidget;}
	QTableWidget* getBorderTable() { return m_pBorderTableWidget; }

protected:
	void resizeEvent(QResizeEvent *e) override;

signals:
	void clicked(const QString &text, QTreeWidgetItem *item);
	void refresh(void);

public slots:
	void	slot_updateResultExtractMask(void*);

private slots:
	void	slot_OnLog(bool checked = false);
	void	slot_OnGMM(bool bNotMessage = false);
	void	slot_OnExtractMask(); //220314 허 건 과장(심장 솔루션)
	void	slot_OnCopy();
	void	slot_OnColumnClick(QTreeWidgetItem *item, int column);
	void	slot_OnColumnDoubleClick(QTreeWidgetItem *item, int column);
	void	slot_slotButtonClicked(QString strName, QTreeWidgetItem *item);
	void	slot_refreshFeatureValue(void);
	void	slot_OnAllCalc();

private:
	LayerHistogram	*m_histogram;
	QPushButton		*m_btnLog;
	QPushButton		*m_btnGMM;
	QLabel* m_labeNumOfCluster;
	QSpinBox* m_spinboxNumOfCluster;
	QTreeWidget		*m_treeFeature;
	int				m_uid;
	int				m_fileSeq;

	QString				m_sCurrentVisibleString;
	QTreeWidgetItem*	m_pCurrentVisibleWgt;
	float*				m_p3DVolumeMap;
	std::vector<std::pair<float, float>>	m_vecColorCategory;
	bool				m_bVisible2DMap = false;

	QString				m_strMiddleGap = "  ";
	QString				m_strMinorGap = "    ";
	TASubClassResultCtrl *m_pSubResultInfo;

	//
	QTreeWidgetItem *m_middleItemShape3D;
	QTreeWidgetItem *m_middleItemShape2D;

	enum COLUME_ROLE_STATE
	{
		WGT_NOT_USE = 0,
		WGT_VISIBLE,
		WGT_INVISIBLE,
	};

	//220314 허 건 과장(심장 솔루션)
	QPushButton*	m_pBtnExtractMask;			
	QTableWidget*	m_pGMMTableWidget;
	QTableWidget*   m_pBorderTableWidget;

	VOLUME_DATA* m_pVolumeData;
};
#endif