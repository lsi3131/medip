#include "stdafx.h"
#include "RegionGrowTab.h"
#include "ProductManager.h"
#include "StyleManager.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "LicenseManager.h"
#include "ResourceManager.h"
#include "CustomHistogram.h"
#include "ThreSholdTab.h"
#include "Tabwindow.h"
#include "Main/MainSegmentWidget.h"
#include "RangeWidget.h"
#include "DataContext.h"

RegionGrowTab::RegionGrowTab(QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent)
{
	int nRow = 0;

	m_scrollArea->setStyleSheet("QScrollArea {border : 0px; }");

	addWidget(NULL, nRow++);

	QBoxLayout* LayMain = getLayout(nRow - 1);

	QVBoxLayout* LayButtons = new QVBoxLayout;
	QVBoxLayout* LayMode = new QVBoxLayout;
	QVBoxLayout* LaySelect = new QVBoxLayout;
	LayButtons->addLayout(LayMode);
	LayButtons->addLayout(LaySelect);

	QVBoxLayout* LayList = new QVBoxLayout;
	LayMain->addLayout(LayButtons);
	LayMain->addLayout(LayList);

	m_mode = RG_MODE_HU_RANGE;
	m_connectivity = RG_CONNECTIVITY_26;

	QGroupBox* grpBox = new QGroupBox(this);
	grpBox->setTitle("Mode");
	QBoxLayout* boxMain = new QHBoxLayout;
	grpBox->setLayout(boxMain);

	m_grMode = new QButtonGroup(this);

	QRadioButton* radio1 = new QRadioButton(this);
	radio1->setText("HU Range");
	radio1->setCheckable(true);
	radio1->setChecked(true);
	radio1->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	boxMain->addWidget(radio1);
	m_grMode->addButton(radio1, 0);

	radio1 = new QRadioButton(this);
	radio1->setText("Layer");
	radio1->setCheckable(true);
	radio1->setChecked(false);
	radio1->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	boxMain->addWidget(radio1);
	m_grMode->addButton(radio1, 1);

	connect(m_grMode, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeMode(int)));

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	LayMode->addWidget(grpBox);
	LayMode->addWidget(emptyBox0);



	grpBox = new QGroupBox(this);
	grpBox->setTitle("Connectivity");
	boxMain = new QHBoxLayout;
	grpBox->setLayout(boxMain);

	m_grConnect = new QButtonGroup(this);

	radio1 = new QRadioButton(this);
	radio1->setText("26");
	radio1->setCheckable(true);
	radio1->setChecked(true);
	radio1->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	boxMain->addWidget(radio1);
	m_grConnect->addButton(radio1, 0);

	radio1 = new QRadioButton(this);
	radio1->setText("6");
	radio1->setCheckable(true);
	radio1->setChecked(false);
	radio1->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	boxMain->addWidget(radio1);
	m_grConnect->addButton(radio1, 1);

	connect(m_grConnect, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeConnect(int)));

	emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	LayMode->addWidget(grpBox);
	LayMode->addWidget(emptyBox0);

	QPushButton* btnReset = new QPushButton(this);
	btnReset->setText(STRING_MANAGER->getString(STR_RESET));
	btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btnReset, &QPushButton::clicked, this, &RegionGrowTab::slot_OnReset);

	QPushButton* btnApply = new QPushButton(this);
	btnApply->setText(STRING_MANAGER->getString(STR_APPLY));
	btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btnApply, &QPushButton::clicked, this, &RegionGrowTab::slot_OnApply);

	m_btnSelect = new QPushButton(this);
	m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED));
	m_btnSelect->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(m_btnSelect, &QPushButton::clicked, this, &RegionGrowTab::slot_OnSelectSeed);

	LaySelect->addWidget(btnApply);

	m_RGList = new QTreeWidget(this);
	m_RGList->setColumnCount(1);
	m_RGList->setContentsMargins(0, 0, 0, 5);
	m_RGList->setHeaderHidden(true);
	m_RGList->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_RGList->setRootIsDecorated(false);
	m_RGList->setMinimumWidth(20);
	m_RGList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_RGList->installEventFilter(this);

	grpBox = new QGroupBox(this);
	grpBox->setTitle("Seed points");
	LayList->addWidget(grpBox);

	boxMain = new QVBoxLayout;
	grpBox->setLayout(boxMain);
	QVBoxLayout* vLay = new QVBoxLayout;
	QHBoxLayout* hLay = new QHBoxLayout;
	boxMain->addLayout(vLay);
	boxMain->addLayout(hLay);

	vLay->addWidget(m_RGList);
	hLay->addWidget(btnReset);
	hLay->addWidget(m_btnSelect);

	QString title = "Region Growing";
	setTitle(title);
}

void RegionGrowTab::slot_OnChangeMode(int mode)
{
	if (mode == m_mode)
	{
		return;
	}

	if (mode == 0)
	{
		m_grMode->button(1)->setChecked(false);
		m_mode = RG_MODE_HU_RANGE;
	}
	else if (mode == 1)
	{
		m_grMode->button(0)->setChecked(false);
		m_mode = RG_MODE_LAYER;
	}
}

void RegionGrowTab::slot_OnChangeConnect(int mode)
{
	if (mode == m_connectivity)
	{
		return;
	}

	if (mode == 0)
	{
		m_grConnect->button(1)->setChecked(false);
		m_connectivity = RG_CONNECTIVITY_26;
	}
	else
	{
		m_grConnect->button(0)->setChecked(false);
		m_connectivity = RG_CONNECTIVITY_6;
	}
}

void RegionGrowTab::slot_OnReset()
{
	WIN_MANAGER->seedLocation.clear();
	WIN_MANAGER->layerRGLocation.clear();

	WIN_MANAGER->regionGrowingHu = WIN_MANAGER->regionGrowingX = WIN_MANAGER->regionGrowingY = WIN_MANAGER->regionGrowingZ = -1;
	UpdateList();
}

void RegionGrowTab::slot_OnDel()
{
	QList<QTreeWidgetItem*>& list = m_RGList->selectedItems();

	if (list.size() <= 0)
	{
		QMessageBox::warning(this, tr("Region Growing deletion"), tr("Please select items to delete."));
		return;
	}

	for (int i = list.size() - 1; i >= 0; i--)
	{
		int index = m_RGList->indexOfTopLevelItem(list.at(i));

		WIN_MANAGER->seedLocation.remove(index);
	}

	UpdateList();

	WIN_MANAGER->renderLater_GridView(false);
}

void RegionGrowTab::slot_OnApply()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if ((WIN_MANAGER->seedLocation.size() < 1)
		&& (WIN_MANAGER->layerRGLocation.size() < 1)) return;

	int nUpper, nLower;

	if (m_mode == RG_MODE_HU_RANGE)
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();
		nUpper = WIN_MANAGER->getThreUpper();
		nLower = WIN_MANAGER->getThreLower();

		if (tab)
		{
			if (nLower > WIN_MANAGER->regionGrowingHu)
			{
				tab->setStartHU(nLower - (nLower - WIN_MANAGER->regionGrowingHu));
			}

			else if (nUpper < WIN_MANAGER->regionGrowingHu)
			{
				tab->setEndHU(WIN_MANAGER->regionGrowingHu);
			}
		}

		CustomHistogram* view = WIN_MANAGER->mainSegmentWidget->getHistogramView();
		if (view)
		{
			if (view->HuRangeSlider)
			{
				nUpper = WIN_MANAGER->getThreUpper();
				nLower = WIN_MANAGER->getThreLower();

				view->HuRangeSlider->setFirstValue(nLower);
				view->HuRangeSlider->setSecondValue(nUpper);
			}
		}

	}

	nUpper = WIN_MANAGER->getThreUpper();
	nLower = WIN_MANAGER->getThreLower();

#ifdef SELECT_HIDE
	WIN_MANAGER->setSeedShow(WIN_MANAGER->getSeedShow() | RT_SELECT);
#endif
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();
		if (tab)
		{
			tab->PreviewFinish();
		}
	}

	bool underROI;
	if (m_mode == RG_MODE_HU_RANGE)
	{
		underROI = false;
	}
	else
	{
		underROI = true;
	}

	bool isConnectivity_6 = false;
	if (m_connectivity == RG_CONNECTIVITY_26)
	{
		isConnectivity_6 = false;
	}
	else
	{
		isConnectivity_6 = true;
	}

	ACTION_MANAGER->action_RegionGrowing(
		WIN_MANAGER->regionGrowingX,
		WIN_MANAGER->regionGrowingY,
		WIN_MANAGER->regionGrowingZ,
		nUpper, nLower,
		(!WIN_MANAGER->layerRGLocation.isEmpty()) ? true : underROI,
		WIN_MANAGER->getSelectedMask(),
		WIN_MANAGER->getSelectedMaskByteIndex(),
		isConnectivity_6,
		WIN_MANAGER->getRGType());
}

void RegionGrowTab::slot_OnSelectSeed()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	WORK_MODE mode = WIN_MANAGER->getWorkMode();

	if (mode == WORK_SEED_SELECT)
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
	else
	{
		m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED) + " mode cancel");
		WIN_MANAGER->selectSeedMode();
	}
}

void RegionGrowTab::Init()
{
	UpdateList();
}

void RegionGrowTab::ApplyRegionGrowing()
{
	slot_OnApply();
}

void RegionGrowTab::cancelSelectMode()
{
	if (m_btnSelect)
		m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED));
}

void RegionGrowTab::UpdateList()
{
	m_RGList->clear();
	QList<QTreeWidgetItem*> items;

	for (int i = 0; i < WIN_MANAGER->seedLocation.size(); i++)
	{
		QVector3D pos = WIN_MANAGER->seedLocation.at(i);
		QTreeWidgetItem* item = new QTreeWidgetItem(m_RGList);

		item->setText(0, QString("Seed point : (%1, %2, %3) %4").arg(pos.x()).arg(pos.y()).arg(pos.z()).
			arg(DATA_CONTEXT->volume_data.getData(pos.x(), pos.y(), pos.z())));

		items.append(item);
	}
	if (!items.isEmpty())
		m_RGList->insertTopLevelItems(0, items);

}

bool RegionGrowTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL);

	if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Delete)
		{
			slot_OnDel();
		}

	}

	return QObject::eventFilter(watched, event);
}

