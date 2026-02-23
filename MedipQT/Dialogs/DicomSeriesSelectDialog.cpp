#include "stdafx.h"
#include "DicomSeriesSelectDialog.h"

#include <System/stringManager.h>

DicomSeriesSelectDialog::DicomSeriesSelectDialog(std::vector<DcmtkSeriesInfo> & seriesInfoList, DISPLAY_MODE eMode)
{
	m_tree = NULL;
	selected_Index = -1;	
	QVBoxLayout * layout = new QVBoxLayout(this);

	QStringList headList;

	if (eMode == DISPLAY_MODE::DM_SERIES)
	{
		this->resize(1200, 300);

		headList.push_back(tr("No."));
		headList.push_back(STRING_MANAGER->getString(STR_UID));
		headList.push_back(STRING_MANAGER->getString(STR_IMG_POS));
		headList.push_back(STRING_MANAGER->getString(STR_NAME));
		headList.push_back(STRING_MANAGER->getString(STR_AGE));
		headList.push_back(STRING_MANAGER->getString(STR_SEX));
		headList.push_back(STRING_MANAGER->getString(STR_EXAM_DATE));
		headList.push_back(STRING_MANAGER->getString(STR_SERIES_DESC));
		headList.push_back(STRING_MANAGER->getString(STR_MODALITY));
		headList.push_back(STRING_MANAGER->getString(STR_WIDTH));
		headList.push_back(STRING_MANAGER->getString(STR_HEIGHT));
		headList.push_back(STRING_MANAGER->getString(STR_SLICE));

		m_tree = new QTreeWidget(this);
		m_tree->setColumnCount(12);
		m_tree->setHeaderLabels(headList);
		m_tree->setHeaderHidden(false);
		m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tree->setAnimated(false);
		m_tree->setIndentation(20);
		m_tree->setSortingEnabled(true);
		const QSize availableSize = this->size();
		m_tree->resize(availableSize / 3);
		m_tree->setColumnWidth(0, 60);
		m_tree->setColumnWidth(1, 310);//SeriesUID
		m_tree->setColumnWidth(2, 120);//ImagePosition
		m_tree->setColumnWidth(4, 60);//age
		m_tree->setColumnWidth(5, 60);//sex
		m_tree->setColumnWidth(8, 60);//Modality
		m_tree->setColumnWidth(9, 60);//Width
		m_tree->setColumnWidth(10, 50);//Height
		m_tree->setColumnWidth(11, 50);//Slice
	}
	else if (eMode == DISPLAY_MODE::DM_ACQUISITIONNUM)
	{
		this->resize(700, 250);

		headList.push_back(tr("No."));
		headList.push_back(STRING_MANAGER->getString(STR_ACQUISITION_TIME));
		headList.push_back(STRING_MANAGER->getString(STR_SERIES_DESC));
		headList.push_back(STRING_MANAGER->getString(STR_WIDTH));
		headList.push_back(STRING_MANAGER->getString(STR_HEIGHT));
		headList.push_back(STRING_MANAGER->getString(STR_SLICE));		

		m_tree = new QTreeWidget(this);
		m_tree->setColumnCount(6);
		m_tree->setHeaderLabels(headList);
		m_tree->setHeaderHidden(false);
		m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tree->setAnimated(false);
		m_tree->setIndentation(20);
		m_tree->setSortingEnabled(true);
		const QSize availableSize = this->size();
		m_tree->resize(availableSize / 3);
		m_tree->setColumnWidth(0, 60);
		m_tree->setColumnWidth(1, 120);// Acquistion number
		m_tree->setColumnWidth(2, 200);//Series desc
		m_tree->setColumnWidth(3, 60);//Width
		m_tree->setColumnWidth(4, 50);//Height
		m_tree->setColumnWidth(5, 50);//Slice
	}
	
	connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnListDoubleClickted(QTreeWidgetItem *, int)));
	connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnListClickted(QTreeWidgetItem *, int)));

	m_tree->sortByColumn(0, Qt::AscendingOrder); //0번째 컬럼 기준으로 오름차순 정렬
	layout->addWidget(m_tree);

	QList<QTreeWidgetItem *> items;
	for (int i = 0; i < seriesInfoList.size(); ++i)
	{
		DcmtkSeriesInfo & info = seriesInfoList[i];
		
		QTreeWidgetItem *item = new QTreeWidgetItem(m_tree);
		if (eMode == DISPLAY_MODE::DM_SERIES)
		{
			item->setText(0, QString::number(i).rightJustified(3));
			item->setText(1, QString::fromLocal8Bit((info.strSeriesUID.c_str())));
			item->setText(2, QString::fromLocal8Bit(info.imagePosition.c_str()));
			item->setText(3, QString::fromLocal8Bit(info.patientsName_.c_str()));
			item->setText(4, QString::fromLocal8Bit(info.age_.c_str()));
			item->setText(5, QString::fromLocal8Bit(info.sex_.c_str()));
			item->setText(6, QString::fromLocal8Bit(info.seriesDate.c_str()));
			item->setText(7, QString::fromLocal8Bit(info.description_.c_str()));
			item->setText(8, QString::fromLocal8Bit(info.modality_.c_str()));
			item->setText(9, QString::fromLocal8Bit(info.width.c_str()));
			item->setText(10, QString::fromLocal8Bit(info.height.c_str()));
			item->setText(11, QString::fromLocal8Bit(info.numImages_.c_str()));
		}
		else if (eMode == DISPLAY_MODE::DM_ACQUISITIONNUM)
		{
			item->setText(0, QString::number(i).rightJustified(3));
			item->setText(1, QString::fromLocal8Bit(info.strAcquisitionNum.c_str()));
			item->setText(2, QString::fromLocal8Bit(info.description_.c_str()));			
			item->setText(3, QString::fromLocal8Bit(info.width.c_str()));
			item->setText(4, QString::fromLocal8Bit(info.height.c_str()));
			item->setText(5, QString::fromLocal8Bit(info.numImages_.c_str()));
		}
	}
	m_tree->insertTopLevelItems(0, items);
	
	QPushButton * button = new QPushButton(STRING_MANAGER->getString(STR_LOAD), this);
	button->setFixedSize(120, 50); //사이즈
	QPushButton::connect(button, SIGNAL(clicked()), this, SLOT(OnClickedButtonLoad()));

	layout->addWidget(button);
	setLayout(layout);
}

DicomSeriesSelectDialog::~DicomSeriesSelectDialog()
{
}

void DicomSeriesSelectDialog::OnClickedButtonLoad()
{
	accept();
}

void DicomSeriesSelectDialog::OnListDoubleClickted(QTreeWidgetItem * item, int column)
{
	selected_Index = m_tree->indexOfTopLevelItem(item);
	accept();
}

void DicomSeriesSelectDialog::OnListClickted(QTreeWidgetItem * item, int column)
{
	selected_Index = m_tree->indexOfTopLevelItem(item);
}
