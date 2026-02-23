#include "stdafx.h"
#include "AnalysisTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ActionManager.h"
#include "AnalysisWidget.h"
#include "DataContext.h"

AnalysisTab::AnalysisTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
{
	int nRow = 0;

	tabList = new QTabWidget(this);
	tabList->setStyleSheet(STYLE_MANAGER->listTabWidget);
	tabList->setTabsClosable(true);
	tabList->hide();
	addWidget(tabList, nRow, 0, QMargins(), Qt::Alignment(), true);
	connect(tabList, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_DeleteResult(int)));

	m_labelDefault = new QLabel(this);
	m_labelDefault->setText("No results");

	addWidget(m_labelDefault, nRow++, 0, QMargins(), Qt::AlignCenter, true);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0, QMargins(), Qt::Alignment(), true);

	QString title = "Analysis result";
	setTitle(title);
}

void AnalysisTab::DeleteResult(int index, bool bUID)
{
	//todo
	if (!bUID)
	{
		tabList->removeTab(index);
	}
	else
	{
		bool res = false;
		for (int i = 0; i < tabList->count(); i++)
		{
			AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (_pre)
			{
				if (index == _pre->getUID())
				{
					res = true;
					tabList->removeTab(i);
					//SAFE_DELETE(_pre); 	// 220316 허 건 과장 주석처리(Undo / Redo 수행 시, Error 발생) 
					if (tabList->count() > 0)
						tabList->setCurrentIndex(0);
					else
						m_labelDefault->show();
					break;
				}
			}
		}
		if (!res) return;
	}

	if (tabList->count() > 0)
		tabList->setCurrentIndex(0);
	else
	{
		m_labelDefault->show();
		tabList->hide();
	}
	bool chkCollapse = isCollapse();

	if (chkCollapse)
		setOpenWidget(true);

	updateHeight();
	updateGeometry();

	if (chkCollapse)
		setOpenWidget(false);
}

void AnalysisTab::slot_OnCurrentChanged(int index)
{
	AnalysisWidget* pWgt = (AnalysisWidget*)tabList->widget(index);
	if (pWgt)
	{
		if (pWgt->isVisible2DMap())
		{
			WIN_MANAGER->pRadiomics3DVolume = pWgt->get3DVolumeMap();
			WIN_MANAGER->vecRadiomicsColorCategory = pWgt->getColorCategory();
		}
		else
		{
			WIN_MANAGER->pRadiomics3DVolume = nullptr;
			WIN_MANAGER->vecRadiomicsColorCategory.clear();
		}

		WIN_MANAGER->forceUpdateRadiomicsView();
	}
}

void AnalysisTab::slot_OntabCloseRequested(int index)
{
	int cnt = tabList->count();
	if (cnt < 1)
	{
		WIN_MANAGER->pRadiomics3DVolume = nullptr;
		WIN_MANAGER->forceUpdateRadiomicsView();
	}
}

int AnalysisTab::existResult(int index, bool bUID)
{
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));

	if (child)
		return child->getSeq();
	return -1;
}

bool AnalysisTab::GetResult(mint16& _min, mint16& _max, QString& _result,
	QVector<QPointF>& _points, QVector<QPointF>& _logPoints, int index, bool bUID)
{
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));

	if (child)
		return child->getInfo(_min, _max, _result, _points, _logPoints);

	return false;
}

bool AnalysisTab::GetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable, int index, bool bUID)
{
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
	{
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));
	}

	if (child)
	{
		return child->getQTableInfo(vecGMMTable, vecBorderTable);
	}

	return false;
}

void AnalysisTab::InitTreeWidget(int index, bool bUID)
{
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
	{
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));
	}

	if (child)
	{
		return child->InitTreeWidget();
	}

}

QTreeWidget* AnalysisTab::getTreeWidget(int index, bool bUID)
{
	QTreeWidget* treeWidget = nullptr;
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
	{
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));
	}

	if (child)
	{
		treeWidget = child->getTreeWidget();
	}

	return treeWidget;
}

void AnalysisTab::slot_DeleteResult(int index)
{
	//	DeleteResult(index, false);

	AnalysisWidget* child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));


	if (child)
	{
		int uid = child->getUID();

		ACTION_MANAGER->action_ImageFeatureExtractor_Del(uid);
	}
}

void AnalysisTab::ClearResult()
{
	for (int i = 0; i < tabList->count(); i++)
		tabList->removeTab(0);

	if (tabList->count() > 0)
		tabList->setCurrentIndex(0);
	else
	{
		m_labelDefault->show();
		tabList->hide();
	}
	bool chkCollapse = isCollapse();

	if (chkCollapse)
		setOpenWidget(true);

	updateHeight();
	updateGeometry();

	if (chkCollapse)
		setOpenWidget(false);

}

// init
void AnalysisTab::SetResult(int uid, QString str, int fileSeq, QVector<QPointF>* _p, QVector<QPointF>* _lp)
{
	for (int i = 0; i < tabList->count(); i++)
	{
		AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

		if (_pre)
		{
			if (uid == _pre->getUID())
			{
				tabList->removeTab(i);
				if (tabList->count() > 0)
					tabList->setCurrentIndex(0);
				break;
			}
		}
	}

	AnalysisWidget* pAnalysisWgt = new AnalysisWidget(&DATA_CONTEXT->volume_data, uid, this);
	pAnalysisWgt->init(str, fileSeq);

	QString strMskName = DATA_CONTEXT->volume_data.getMaskName(uid, true);

	tabList->setCurrentIndex(tabList->addTab(pAnalysisWgt, strMskName));
	connect(tabList, &QTabWidget::currentChanged, this, &AnalysisTab::slot_OnCurrentChanged);
	connect(tabList, &QTabWidget::tabCloseRequested, this, &AnalysisTab::slot_OntabCloseRequested);
	if (!m_labelDefault->isHidden())
		m_labelDefault->hide();

	bool chkCollapse = isCollapse();

	if (chkCollapse)
		setOpenWidget(true);

	if (tabList->isHidden())
		tabList->show();

	updateHeight();
	updateGeometry();
	//todo uid check
	if (chkCollapse)
		setOpenWidget(false);

}


void AnalysisTab::MacroSave(QString fileName, int num)
{

	AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(num));
	_pre->OnSaveMacro(fileName);


}

void AnalysisTab::MacroSaveHisto(QString fileName, int num)
{

	AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(num));
	_pre->OnSaveMacroHisto(fileName);


}

void AnalysisTab::SetResultFromFile(int uid, int fileSeq)
{
	mint16 huMin, huMax;
	int pointCount;
	QVector<QPointF> points, logPoints;
	QString result;

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = (STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(fileSeq));
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)&huMin, sizeof(mint16));
			file.read((char*)&huMax, sizeof(mint16));
			file.read((char*)&pointCount, sizeof(int));
			points.resize(pointCount);
			file.read((char*)points.data(), sizeof(QPointF) * (pointCount));
			file.read((char*)&pointCount, sizeof(int));
			logPoints.resize(pointCount);
			file.read((char*)logPoints.data(), sizeof(QPointF) * (pointCount));
			file.read((char*)&pointCount, sizeof(int));
			result.resize(pointCount);
			file.read((char*)result.data(), sizeof(QChar) * pointCount);
			file.close();

			SetResult(uid, result, huMin, huMax, points, logPoints, fileSeq);
		}
	}
}

void AnalysisTab::UpdateName(int uid, QString _name)
{
	if (tabList->count() == 0)return;

	for (int i = 0; i < tabList->count(); i++)
	{
		AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

		if (_pre)
		{
			if (uid == _pre->getUID())
			{
				tabList->setTabText(i, _name);
				break;
			}
		}
	}
}

void AnalysisTab::ChangeUID(int pre_uid, int chg_uid)
{
	for (int i = 0; i < tabList->count(); i++)
	{
		AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

		if (_pre)
		{
			if (pre_uid == _pre->getUID())
			{
				_pre->UpdateUID(chg_uid);
				break;
			}
		}
	}
}

void AnalysisTab::SetResult(int uid, QString _name, mint16 _min, mint16 _max, QVector<QPointF> _points, QVector<QPointF> _logPoints, int fileSeq)
{
	//	if (tabList->count() == 0)return;
	bool _exist = false;
	for (int i = 0; i < tabList->count(); i++)
	{
		AnalysisWidget* _pre = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

		if (_pre)
		{
			if (uid == _pre->getUID())
			{
				_pre->Update(_name, _min, _max, _points, _logPoints, fileSeq);
				_exist = true;
				break;
			}
		}
	}

	if (!_exist)
	{
		AnalysisWidget* result = new AnalysisWidget(&DATA_CONTEXT->volume_data,uid, this);
		result->Update(_name, _min, _max, _points, _logPoints, fileSeq);

		QString layername = DATA_CONTEXT->volume_data.getMaskName(uid, true);

		tabList->setCurrentIndex(tabList->addTab(result, layername));
		if (!m_labelDefault->isHidden())
			m_labelDefault->hide();

		bool chkCollapse = isCollapse();

		if (chkCollapse)
			setOpenWidget(true);

		if (tabList->isHidden())
			tabList->show();

		updateHeight();
		updateGeometry();

		if (chkCollapse)
			setOpenWidget(false);
	}
}

void AnalysisTab::SetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable, int index, bool bUID)
{
	AnalysisWidget* child = NULL;
	if (bUID)
	{
		for (int i = 0; i < tabList->count(); i++)
		{
			child = dynamic_cast<AnalysisWidget*>(tabList->widget(i));

			if (child)
			{
				if (index == child->getUID())
					break;
				else
					child = NULL;
			}
		}
	}
	else
	{
		child = dynamic_cast<AnalysisWidget*>(tabList->widget(index));
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
	{
		if (child)
		{
			child->SetQTableResult(vecGMMTable, vecBorderTable);
		}
	}
}


