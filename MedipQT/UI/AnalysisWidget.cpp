#include "stdafx.h"
#include "UI/AnalysisWidget.h"
#include "UI/LayerHistogram.h"

#include "System/stringManager.h"
#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/FileManager.h"

#include "algorithm/Radiomics.h"

#include "Actions/ActionManager.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/AnalMPRPlaneView.h"
#include "ProductManager.h"


AnalysisWidget::AnalysisWidget(VOLUME_DATA* pVolumeData, int uid, QWidget* parent /*= NULL*/) : 
	QWidget(parent)
	, m_pSubResultInfo(nullptr)
	, m_sCurrentVisibleString("")
	, m_pCurrentVisibleWgt(nullptr)
	, m_pGMMTableWidget(nullptr)
	, m_pVolumeData(pVolumeData)
{
	m_uid = uid;
	m_fileSeq = -1;
	int nRow = 0;

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* hboxLayout = new QHBoxLayout;
	mainLayout->addLayout(hboxLayout);
	//histogram
	m_histogram = new LayerHistogram(m_uid, this);
	hboxLayout->addWidget(m_histogram);

	hboxLayout = new QHBoxLayout;
	mainLayout->addLayout(hboxLayout);
	//histogram btn
	m_btnLog = new QPushButton("Log", this);
	m_btnLog->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnLog->setCheckable(true);
	m_btnLog->setChecked(false);
	connect(m_btnLog, &QPushButton::clicked, this, &AnalysisWidget::slot_OnLog);

	m_btnGMM = new QPushButton("GMM", this);
	m_btnGMM->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnGMM, &QPushButton::clicked, this, &AnalysisWidget::slot_OnGMM);

	m_labeNumOfCluster = new QLabel("Number of Cluster", this);
	m_labeNumOfCluster->setStyleSheet(STYLE_MANAGER->labelNormal);

	m_spinboxNumOfCluster = new QSpinBox(this);
	m_spinboxNumOfCluster->setStyleSheet(STYLE_MANAGER->spinbox);
	m_spinboxNumOfCluster->setValue(2);
	m_spinboxNumOfCluster->setContentsMargins(10, 0, 0, 0);
	m_spinboxNumOfCluster->setMinimum(2);
	m_spinboxNumOfCluster->setMaximum(50);

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
	{
		// 220314 허 건 과장
		m_pBtnExtractMask = new QPushButton("Extract Mask", this);
		m_pBtnExtractMask->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_pBtnExtractMask, &QPushButton::clicked, this, &AnalysisWidget::slot_OnExtractMask);
	}

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hboxLayout->addWidget(m_btnLog);
	hboxLayout->addWidget(m_btnGMM);

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
	{
		hboxLayout->addWidget(m_labeNumOfCluster);
		hboxLayout->addWidget(m_spinboxNumOfCluster);


		hboxLayout->addWidget(m_pBtnExtractMask);
	}

	hboxLayout->addWidget(emptyBox);

	hboxLayout = new QHBoxLayout;
	mainLayout->addLayout(hboxLayout);

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
	{
		// 220314 허 건 과장
		// table widget for displaye GMM result
		{
			hboxLayout = new QHBoxLayout;
			m_pGMMTableWidget = new QTableWidget(2, 3, this);

			QStringList str_list;

			str_list.push_back("Mean");
			str_list.push_back("SD");
			str_list.push_back("Prior");

			QColor color;
			for (int i = 0; i < m_pGMMTableWidget->columnCount(); i++)
			{
				QTableWidgetItem* item = new QTableWidgetItem(str_list[i]);
				item->setTextColor(color.dark());
				m_pGMMTableWidget->setHorizontalHeaderItem(i, item);
			}

			for (int i = 0; i < m_pGMMTableWidget->rowCount(); i++)
			{
				QTableWidgetItem* item = new QTableWidgetItem(QString().sprintf("GMM%d", i));
				item->setTextColor(color.dark());
				m_pGMMTableWidget->setVerticalHeaderItem(i, item);

				m_pGMMTableWidget->setItemDelegate(new ItemDelegate(m_pGMMTableWidget));
			}

			m_pGMMTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			m_pGMMTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

			m_pGMMTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

			hboxLayout->addWidget(m_pGMMTableWidget);
			mainLayout->addLayout(hboxLayout);
		}

		// border
		{
			hboxLayout = new QHBoxLayout;
			m_pBorderTableWidget = new QTableWidget(2, 2, this);

			QStringList str_list;

			str_list.push_back("Mean1 + SD1");
			str_list.push_back("Mean2 - SD2");

			QColor color;
			for (int i = 0; i < m_pBorderTableWidget->columnCount(); i++)
			{
				QTableWidgetItem* item = new QTableWidgetItem(str_list[i]);
				item->setTextColor(color.dark());
				m_pBorderTableWidget->setHorizontalHeaderItem(i, item);
			}

			for (int i = 0; i < m_pBorderTableWidget->rowCount(); i++)
			{
				QTableWidgetItem* item = new QTableWidgetItem(QString().sprintf("GMM%d\n~ GMM%d", i, i + 1));
				item->setTextColor(color.dark());
				m_pBorderTableWidget->setVerticalHeaderItem(i, item);

				m_pBorderTableWidget->setItemDelegate(new ItemDelegate(m_pGMMTableWidget));
			}

			m_pBorderTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			m_pBorderTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

			m_pBorderTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

			hboxLayout->addWidget(m_pBorderTableWidget);
			mainLayout->addLayout(hboxLayout);
		}
	}

	//feature widget
	hboxLayout = new QHBoxLayout;
	m_treeFeature = new QTreeWidget(this);
	m_treeFeature->setColumnCount(TRMA_COL_BTN_AND_VALUE + 1);

	m_treeFeature->header()->setSectionResizeMode(TRMA_COL_SUB, QHeaderView::Fixed);
	m_treeFeature->header()->setSectionResizeMode(TRMA_COL_VISIBLE, QHeaderView::Fixed);
	m_treeFeature->header()->setSectionResizeMode(TRMA_COL_NAME, QHeaderView::Stretch);
	m_treeFeature->header()->setSectionResizeMode(TRMA_COL_BTN_AND_VALUE, QHeaderView::Interactive);
	m_treeFeature->setHeaderHidden(false);
	m_treeFeature->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_treeFeature->setRootIsDecorated(false);
	m_treeFeature->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_treeFeature->setFixedHeight(500);

	hboxLayout->addWidget(m_treeFeature);
	mainLayout->addLayout(hboxLayout);

	hboxLayout = new QHBoxLayout;
	mainLayout->addLayout(hboxLayout);

	//feature btn
	QPushButton* btn = new QPushButton("Feature copy", this);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btn, &QPushButton::clicked, this, &AnalysisWidget::slot_OnCopy);
	hboxLayout->addWidget(btn);

	btn = new QPushButton("Feature save", this);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btn, &QPushButton::clicked, this, &AnalysisWidget::OnSave);
	hboxLayout->addWidget(btn);

	btn = new QPushButton("Feature calculate", this);
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btn, &QPushButton::clicked, this, &AnalysisWidget::slot_OnAllCalc);
	hboxLayout->addWidget(btn);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	hboxLayout->addWidget(emptyBox);

	connect(m_treeFeature, &QTreeWidget::itemClicked, this, &AnalysisWidget::slot_OnColumnClick);
	connect(m_treeFeature, &QTreeWidget::itemDoubleClicked, this, &AnalysisWidget::slot_OnColumnDoubleClick);
	connect(this, SIGNAL(refresh()), this, SLOT(slot_refreshFeatureValue()));

	m_pSubResultInfo = new TASubClassResultCtrl();
}

AnalysisWidget::~AnalysisWidget()
{
	// 220316 허 건 과장 주석처리 
	// 프로그램 종료 시, Error 발생
	//SAFE_DELETE(m_pSubResultInfo);
	//SAFE_DELETE(m_p3DVolumeMap);
}

void AnalysisWidget::setVisibleShapeFeature(bool isShape3D)
{
	if (isShape3D)
	{
		m_middleItemShape2D->setHidden(true);
		m_middleItemShape3D->setHidden(false);
	}
	else
	{
		m_middleItemShape2D->setHidden(false);
		m_middleItemShape3D->setHidden(true);
	}

}

void AnalysisWidget::resizeEvent(QResizeEvent* e)
{
	int e_width = e->size().width();
	int e_height = e->size().height();

	int width = m_treeFeature->size().width();

	int nSubSize = 40;
	int nVisibleSize = 22;
	int nValueSize = 80;

	//int nNameSize = width - nSubSize - nVisibleSize - nValueSize - 6;

	m_treeFeature->setColumnWidth(TRMA_COL_SUB, nSubSize);
	m_treeFeature->setColumnWidth(TRMA_COL_VISIBLE, nVisibleSize);
	//m_treeFeature->setColumnWidth(TRMA_COL_NAME, nNameSize);
	m_treeFeature->setColumnWidth(TRMA_COL_BTN_AND_VALUE, nValueSize);
}

void AnalysisWidget::init(QString str, int fileSeq)
{
	m_fileSeq = fileSeq;
	m_histogram->initPlot();
	m_histogram->CreateHistogram(m_fileSeq);

	QStringList featureList = str.split("\n");

	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int depth = m_pVolumeData->getCZ();
	m_p3DVolumeMap = new float[width * height * depth];


	//////////////////////

	////////////////////////////

	InitTreeWidget();
}

void AnalysisWidget::InitTreeWidget()
{
	m_treeFeature->clear();

	//TASubClassResultCtrl subResultInfo;
	int nMajorSize = m_pSubResultInfo->getMajorClassSize();
	for (int i = 0; i < nMajorSize; i++)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(m_treeFeature);

		item->setText(TRMA_COL_SUB, QString("+"));
		item->setText(TRMA_COL_NAME, std::move(m_pSubResultInfo->getMajorClassName(i)));
		item->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, QVariant(WGT_NOT_USE));

		m_treeFeature->addTopLevelItem(item);
		//m_treeFeature->resizeColumnToContents(1);
		m_treeFeature->setMinimumWidth(5);

		int nMiddleSize = m_pSubResultInfo->getMiddleClassSize(i);
		for (int j = 0; j < nMiddleSize; j++)
		{
			QTreeWidgetItem* middleItem = new QTreeWidgetItem(item);
			middleItem->setText(TRMA_COL_SUB, QString("+"));
			middleItem->setText(TRMA_COL_NAME, m_strMiddleGap + std::move(m_pSubResultInfo->getMiddleClassName(i, j)));
			middleItem->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, QVariant(WGT_NOT_USE));
			m_treeFeature->addTopLevelItem(middleItem);

			//
			if (m_pSubResultInfo->getMiddleClassName(i, j).compare(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_3D)) == 0)
				m_middleItemShape3D = middleItem;
			if (m_pSubResultInfo->getMiddleClassName(i, j).compare(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_2D)) == 0)
				m_middleItemShape2D = middleItem;


			QPushButton* pBtnAllEx = new QPushButton();
			pBtnAllEx->setText(STRING_MANAGER->getString(STR_BTN_TA_FEATURE_ALL_EXTRACT));
			//pBtnAllEx->setFixedWidth(70);
			pBtnAllEx->setFixedHeight(20);
			pBtnAllEx->setStyleSheet(STYLE_MANAGER->buttonNormal);
			m_treeFeature->setItemWidget(middleItem, TRMA_COL_BTN_AND_VALUE, pBtnAllEx);

			QString strTmp(std::move(m_pSubResultInfo->getMiddleClassName(i, j)));
			connect(pBtnAllEx, &QPushButton::clicked, [=] {
				slot_slotButtonClicked(strTmp, middleItem);
				});

			int nMinorSize = m_pSubResultInfo->getMinorClassSize(i, j);
			for (int k = 0; k < nMinorSize; k++)
			{
				QTreeWidgetItem* minorItem = new QTreeWidgetItem(middleItem);
				minorItem->setText(TRMA_COL_SUB, QString(""));
				minorItem->setIcon(TRMA_COL_VISIBLE, RESOURCE_MANAGER->getIcon(true ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
				minorItem->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, QVariant(WGT_INVISIBLE));

				minorItem->setTextAlignment(TRMA_COL_VISIBLE, Qt::AlignHCenter);
				minorItem->setText(TRMA_COL_NAME, m_strMinorGap + std::move(m_pSubResultInfo->getMinorClassName(i, j, k)));
				minorItem->setTextAlignment(TRMA_COL_BTN_AND_VALUE, Qt::AlignRight);

				m_treeFeature->addTopLevelItem(minorItem);
			}
		}
	}
}

void AnalysisWidget::Update(QString _name, mint16 _min, mint16 _max, QVector<QPointF> _points, QVector<QPointF> _logPoints, int fileSeq)
{
	m_fileSeq = fileSeq;
	m_histogram->initPlot();
	m_histogram->SetHistogram(_min, _max, _points, _logPoints, fileSeq);
	m_treeFeature->clear();

	QStringList featureList = _name.split("\n");

	for (int i = 0; i < featureList.size(); i++)
	{
		QStringList _content = featureList.at(i).split("\t");

		if (_content.size() == 2)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(m_treeFeature);
			item->setText(0, _content.at(0));
			item->setText(1, _content.at(1));

			m_treeFeature->addTopLevelItem(item);
			m_treeFeature->resizeColumnToContents(0);
		}
	}
}

void AnalysisWidget::UpdateUID(int uid)
{
	m_uid = uid;
}

bool AnalysisWidget::getInfo(mint16& _min, mint16& _max, QString& _name, QVector<QPointF>& _points, QVector<QPointF>& _logPoints)
{
	QString result;

	for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_treeFeature->topLevelItem(i);

		if (item)
		{
			result.append(item->text(0));
			result.append("\t");
			result.append(item->text(1));
			result.append("\n");
		}
	}

	m_histogram->getRange(_min, _max);
	_name = result;
	_points.clear();
	_points = m_histogram->getPoints();
	_logPoints.clear();
	_logPoints = m_histogram->getPoints(true);

	return true;
}

/**
 * \brief
 *
 * \param vecGMMTable
 * \param vecBorderTable
*/
void AnalysisWidget::SetQTableResult(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable)
{
	if (m_pGMMTableWidget)
	{
		int rows = (int)vecGMMTable.size();

		m_pGMMTableWidget->setRowCount(rows);

		for (int i = 0; i < rows; ++i)
		{
			int cols = (int)vecGMMTable[i].size();

			m_pGMMTableWidget->setColumnCount(cols);

			for (int j = 0; j < cols; ++j)
			{
				if (vecGMMTable[i][j] != -DBL_MAX)
				{
					QTableWidgetItem* item = m_pGMMTableWidget->item(i, j);

					if (item)
					{
						item->setText(QString().sprintf("%lf", vecGMMTable[i][j]));
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
					}
					else
					{
						item = new QTableWidgetItem();
						item->setText(QString().sprintf("%lf", vecGMMTable[i][j]));
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

						m_pGMMTableWidget->setItem(i, j, item);
					}
				}
			}
		}
	}

	if (m_pGMMTableWidget)
	{
		int rows = (int)vecBorderTable.size();

		m_pBorderTableWidget->setRowCount(rows);

		for (int i = 0; i < rows; ++i)
		{
			int cols = (int)vecBorderTable[i].size();

			m_pGMMTableWidget->setColumnCount(cols);

			for (int j = 0; j < cols; ++j)
			{
				if (vecBorderTable[i][j] != -DBL_MAX)
				{
					QTableWidgetItem* item = m_pGMMTableWidget->item(i, j);

					if (item)
					{
						item->setText(QString().sprintf("%lf", vecBorderTable[i][j]));
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
					}
					else
					{
						item = new QTableWidgetItem();
						item->setText(QString().sprintf("%lf", vecBorderTable[i][j]));
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

						m_pGMMTableWidget->setItem(i, j, item);
					}
				}
			}
		}
	}
}

/**
 * \brief
 *
 * \param	vecGMMTable
 * \param	vecBorderTable
 * \return
*/
bool AnalysisWidget::getQTableInfo(std::vector<std::vector<double>>& vecGMMTable, std::vector<std::vector<double>>& vecBorderTable)
{
	// get GMM value
	vecGMMTable.resize(m_pGMMTableWidget->rowCount());
	for (int i = 0; i < m_pGMMTableWidget->rowCount(); ++i)
	{
		for (int j = 0; j < m_pGMMTableWidget->columnCount(); ++j)
		{
			QTableWidgetItem* item = m_pGMMTableWidget->item(i, j);

			if (item)
			{
				vecGMMTable[i].push_back(item->text().toDouble());
			}
			else
			{
				vecGMMTable[i].push_back(-DBL_MAX);
			}
		}
	}

	vecBorderTable.resize(m_pBorderTableWidget->rowCount());
	for (int i = 0; i < m_pBorderTableWidget->rowCount(); ++i)
	{
		for (int j = 0; j < m_pBorderTableWidget->columnCount(); ++j)
		{
			QTableWidgetItem* item = m_pBorderTableWidget->item(i, j);

			if (item)
			{
				vecBorderTable[i].push_back(item->text().toDouble());
			}
			else
			{
				vecBorderTable[i].push_back(-DBL_MAX);
			}
		}
	}

	return true;
}

QString AnalysisWidget::getResult()
{
	QString result;

	for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_treeFeature->topLevelItem(i);

		if (item)
		{
			result.append(item->text(0));
			result.append("\t");
			result.append(item->text(1));
			result.append("\n");
		}
	}
	return result;
}

void AnalysisWidget::slot_OnLog(bool checked /*= false*/)
{
	if (m_btnLog->isChecked())
		m_btnLog->setText(QString("Non-Log"));
	else
		m_btnLog->setText(QString("Log"));

	m_histogram->setCheckZoom(m_btnLog->isChecked());
}

void AnalysisWidget::slot_OnGMM(bool bNotMessage)
{
	if (m_pVolumeData->isValidate())
	{
		//MaskInfo* pMaskInfo = m_pVolumeData->getCurrentMaskInfo();
		MaskInfo* pMaskInfo = m_pVolumeData->getMaskInfo(m_uid, true);
		mask8 maskBit = m_pVolumeData->getMask(pMaskInfo->uid);

		int byteIdx = m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid);
		mask8* pMaskData = m_pVolumeData->getMaskDataPoint(byteIdx);

		mint16* pHUData = m_pVolumeData->getHUDataPoint();

		muint32 cx = m_pVolumeData->getCX();
		muint32 cy = m_pVolumeData->getCY();
		muint32 cz = m_pVolumeData->getCZ();

		int num = m_spinboxNumOfCluster->value();

		std::vector<GMMOverlapedData> GMMDatas;
		getGMMOverlapValue(pMaskData, pHUData, cx, cy, cz, maskBit, num, GMMDatas);

		if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
		{
			m_pGMMTableWidget->setRowCount((int)GMMDatas.size());

			QColor color;
			for (int i = 0; i < m_pGMMTableWidget->rowCount(); ++i)
			{
				QTableWidgetItem* item = m_pGMMTableWidget->verticalHeaderItem(i);

				if (!item)
				{
					item = new QTableWidgetItem(QString().sprintf("GMM%d", i));
					m_pGMMTableWidget->setVerticalHeaderItem(i, item);
				}
				else
				{
					item->setText(QString().sprintf("GMM%d", i));
				}

				item->setTextColor(color.dark());
			}
		}

		QString copyStr;
		for (int i = 0; i < GMMDatas.size(); ++i)
		{
			copyStr += QString("GMM%1->mean:%2\tSD:%3\tPrior:%4\n").
				arg(i).
				arg(GMMDatas[i].MeanCluster).
				arg(GMMDatas[i].SD).
				arg(GMMDatas[i].Prior);

			QStringList str_list;

			str_list.push_back(QString().sprintf("%lf", GMMDatas[i].MeanCluster));
			str_list.push_back(QString().sprintf("%lf", GMMDatas[i].SD));
			str_list.push_back(QString().sprintf("%lf", GMMDatas[i].Prior));

			if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
			{
				for (int j = 0; j < m_pGMMTableWidget->columnCount(); ++j)
				{
					QTableWidgetItem* item = m_pGMMTableWidget->item(i, j);
					if (!item)
					{
						item = new QTableWidgetItem();

						item->setText(str_list[j]);
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

						m_pGMMTableWidget->setItem(i, j, item);
					}
					else
					{
						item->setText(str_list[j]);
						item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
					}
				}
			}
		}

		if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
		{
			m_pGMMTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
			//m_pGMMTableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
		}

		if (!bNotMessage)
		{
			QMessageBox::information(this, "GMM", copyStr);
		}
	}
}

void	AnalysisWidget::slot_updateResultExtractMask(void* ExtractMaskInfo)
{
	// Update Undo, Redo
	ACTION_MANAGER->action_MaskExtractGMM_Add(ExtractMaskInfo);
}

//220314 허 건 과장(심장 솔루션)
void	AnalysisWidget::slot_OnExtractMask()
{
	int n_cluster = m_spinboxNumOfCluster->value();

	if (n_cluster != 2)
	{
		QMessageBox::information(this, "Information", "Set cluster number 2, please");

		return;
	}

	const int		nHeight = m_pVolumeData->getCY();
	const int		nWidth = m_pVolumeData->getCX();
	const int		nSlice = m_pVolumeData->getCZ();

	MaskInfo* orgMaskInfo = m_pVolumeData->getMaskInfo(m_uid, true);
	int			orgMaskByteIdx = orgMaskInfo->uid >= MASK_SECOND_MAX ? (orgMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	uchar		orgMaskBit = orgMaskByteIdx == 0 ? orgMaskInfo->mask_id : orgMaskInfo->mask_id2;

	ACTION_MANAGER->action_MaskExtractGMM(this, orgMaskInfo, n_cluster);
}

void AnalysisWidget::slot_OnCopy()
{
	auto clip = QApplication::clipboard();

	clip->clear();

	QString copyStr = "";

	// 	for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
	// 	{
	// 		QTreeWidgetItem *item = m_treeFeature->topLevelItem(i);
	// 
	// 		if (item)
	// 		{
	// 			copyStr.append(item->text(0));
	// 			copyStr.append(item->text(1));
	// 			copyStr.append("\r\n");
	// 		}
	//	}
	//	copyStr.replace(" : ", "\t");

	for (auto i = 0; i < m_pSubResultInfo->getMajorClassSize(); ++i)
	{
		copyStr.append(m_pSubResultInfo->getMajorClassName(i));
		copyStr.append("\r\n");
		for (auto j = 0; j < m_pSubResultInfo->getMiddleClassSize(i); ++j)
		{
			copyStr.append("\t");
			copyStr.append(m_pSubResultInfo->getMiddleClassName(i, j));
			copyStr.append("\r\n");
			for (auto k = 0; k < m_pSubResultInfo->getMinorClassSize(i, j); ++k)
			{
				copyStr.append("\t\t");
				copyStr.append(m_pSubResultInfo->getMinorClassName(i, j, k));
				copyStr.append("\t\t");
				copyStr.append(QString::number(m_pSubResultInfo->getFeatureValue(i, j, k)));
				copyStr.append("\r\n");
			}
		}
	}

	clip->setText(copyStr);
}

void AnalysisWidget::slot_OnAllCalc()
{
	ACTION_MANAGER->action_TA_CalculateFeature("All Calculate", m_uid, m_pSubResultInfo);
	m_treeFeature->expandAll();
	//	item->setText(TRMA_COL_SUB, QString("-"));
}

void AnalysisWidget::OnSaveMacro(QString fileName)
{
	if (!fileName.isEmpty())
	{
		QString copyStr = "";

		for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
		{
			QTreeWidgetItem* item = m_treeFeature->topLevelItem(i);

			if (item)
			{
				copyStr.append(item->text(0));
				copyStr.append(item->text(1));
				copyStr.append("\r\n");
			}
		}

		copyStr.replace(" : ", ",");

		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(NULL, "Feature file save fail.", "csv file create fail.");
			return;
		}

		newFile.write((const char*)copyStr.toStdWString().c_str(), sizeof(WCHAR) * copyStr.size());

		newFile.close();
	}
}

void AnalysisWidget::slot_OnColumnClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item) return;
	int cnt = m_treeFeature->selectedItems().count();
	if (cnt > 1) return;


	if (TRMA_COL_SUB == column)
	{
		bool res = false;

		res = item->isExpanded();
		item->setExpanded(!res);

		QString strTmp = item->text(TRMA_COL_SUB);

		if (!strTmp.compare(QString("-")) || !strTmp.compare(QString("+")))
		{
			if (res)
				item->setText(TRMA_COL_SUB, QString("+"));
			else
				item->setText(TRMA_COL_SUB, QString("-"));
		}
	}
	else if (TRMA_COL_VISIBLE == column)
	{
		if (m_pCurrentVisibleWgt != nullptr && m_pCurrentVisibleWgt != item)
		{
			m_pCurrentVisibleWgt->setIcon(TRMA_COL_VISIBLE, RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE));
			m_pCurrentVisibleWgt->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, WGT_INVISIBLE);
			m_pCurrentVisibleWgt->setBackground(TRMA_COL_VISIBLE, QBrush(QColor(48, 48, 48)));
			m_pCurrentVisibleWgt->setForeground(TRMA_COL_VISIBLE, QBrush(QColor(48, 48, 48)));
		}

		if (item->data(column, Qt::UserRole + 1) != WGT_NOT_USE)
		{
			int visible = item->data(column, Qt::UserRole + 1).toInt();
			bool res = false;
			if (visible == WGT_INVISIBLE)
				res = true;

			item->setIcon(TRMA_COL_VISIBLE, RESOURCE_MANAGER->getIcon(res ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
			item->setBackground(TRMA_COL_VISIBLE, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));
			item->setForeground(TRMA_COL_VISIBLE, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));

			item->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, QVariant(res ? WGT_VISIBLE : WGT_INVISIBLE));

			m_pCurrentVisibleWgt = item;
			m_sCurrentVisibleString = item->text(TRMA_COL_NAME);
			m_sCurrentVisibleString.replace(m_strMinorGap, "");

			if (!res)
			{
				m_bVisible2DMap = false;
				WIN_MANAGER->pRadiomics3DVolume = nullptr;
				int width = m_pVolumeData->getCX();
				int height = m_pVolumeData->getCY();
				int depth = m_pVolumeData->getCZ();
				memset(m_p3DVolumeMap, 0, width * height * depth * sizeof(float));
				WIN_MANAGER->forceUpdateRadiomicsView();
			}
			else
			{
				ACTION_MANAGER->action_TA_Calculate2DMap(m_sCurrentVisibleString, m_p3DVolumeMap, m_uid, &m_vecColorCategory);
				m_bVisible2DMap = true;
			}
		}
	}
}

void AnalysisWidget::slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item) return;
	int cnt = m_treeFeature->selectedItems().count();
	if (cnt > 1) return;

	if (TRMA_COL_NAME == column)
	{
		QString curMinorClassName = "";
		curMinorClassName = item->text(column);
		if (curMinorClassName.contains(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_3D_DIAMETER), Qt::CaseInsensitive)
			|| curMinorClassName.contains(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_SLICE), Qt::CaseInsensitive)
			|| curMinorClassName.contains(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_COLUMN), Qt::CaseInsensitive)
			|| curMinorClassName.contains(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_ROW), Qt::CaseInsensitive))
		{
			ACTION_MANAGER->action_TA_CalculateFeature(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_3D), m_uid, m_pSubResultInfo);
			item->setExpanded(true);
			item->setText(TRMA_COL_SUB, QString("-"));
		}
	}
}

void AnalysisWidget::slot_slotButtonClicked(QString strName, QTreeWidgetItem* item)
{
	if (!strName.compare(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_3D)))
		ACTION_MANAGER->action_TA_CalculateFeature(strName, m_uid, m_pSubResultInfo, true);
	else
		ACTION_MANAGER->action_TA_CalculateFeature(strName, m_uid, m_pSubResultInfo);

	item->setExpanded(true);
	item->setText(TRMA_COL_SUB, QString("-"));
}

void AnalysisWidget::slot_refreshFeatureValue(void)
{
	for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* itemMajor = m_treeFeature->topLevelItem(i);
		for (int j = 0; j < itemMajor->childCount(); j++)
		{
			QTreeWidgetItem* itemMiddle = itemMajor->child(j);
			for (int k = 0; k < itemMiddle->childCount(); k++)
			{
				QTreeWidgetItem* itemMinor = itemMiddle->child(k);
				double featureVal = m_pSubResultInfo->getFeatureValue(i, j, k);
				if (featureVal)
					itemMinor->setText(TRMA_COL_BTN_AND_VALUE, QString::number(featureVal, 'f', 5));

			}
		}
	}
}

void AnalysisWidget::OnSave()
{
	MaskInfo* info = m_pVolumeData->getMaskInfo(m_uid, true);
	QString strCSV;
	bool latest = WIN_MANAGER->lastestPathGet(strCSV, true);
	if (latest)
		strCSV += "/" + m_pVolumeData->getMaskName(m_uid, true) + "_Feature";
	else
		strCSV = "";

	QFileInfo fileInfo(strCSV);
	QString fileName = ExportFileDialog(this,
		STRING_MANAGER->getString(STR_EXPORT_STL),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("CSV File(*.csv;*.CSV)"),
		QFileDialog::ShowDirsOnly);

	if (!fileName.isEmpty())
	{
		QString copyStr = "";

		// 		for (int i = 0; i < m_treeFeature->topLevelItemCount(); i++)
		// 		{
		// 			QTreeWidgetItem *item = m_treeFeature->topLevelItem(i);
		// 
		// 			if (item)
		// 			{
		// 				copyStr.append(item->text(0));
		// 				copyStr.append(item->text(1));
		// 				copyStr.append("\r\n");
		// 			}
		// 		}
		//		copyStr.replace(" : ", ",");

		for (auto i = 0; i < m_pSubResultInfo->getMajorClassSize(); ++i)
		{
			copyStr.append(m_pSubResultInfo->getMajorClassName(i));
			copyStr.append("\r\n");
			for (auto j = 0; j < m_pSubResultInfo->getMiddleClassSize(i); ++j)
			{
				copyStr.append(",");
				copyStr.append(m_pSubResultInfo->getMiddleClassName(i, j));
				copyStr.append("\r\n");
				for (auto k = 0; k < m_pSubResultInfo->getMinorClassSize(i, j); ++k)
				{
					copyStr.append(",,");
					copyStr.append(m_pSubResultInfo->getMinorClassName(i, j, k));
					copyStr.append(",");
					copyStr.append(QString::number(m_pSubResultInfo->getFeatureValue(i, j, k)));
					copyStr.append("\r\n");
				}
			}
		}


		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(NULL, "Feature file save fail.", "csv file create fail.");
			return;
		}

		newFile.write((const char*)copyStr.toStdWString().c_str(), sizeof(WCHAR) * copyStr.size());

		newFile.close();
	}
}

void AnalysisWidget::OnSaveMacroHisto(QString fileName)
{
	m_histogram->OnSaveMacro(fileName);

}

void AnalysisWidget::invisibleIcon(void)
{
	if (m_pCurrentVisibleWgt)
	{
		m_pCurrentVisibleWgt->setIcon(TRMA_COL_VISIBLE, RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE));
		m_pCurrentVisibleWgt->setData(TRMA_COL_VISIBLE, Qt::UserRole + 1, false);
		m_pCurrentVisibleWgt->setBackground(TRMA_COL_VISIBLE, QBrush(QColor(48, 48, 48)));
		m_pCurrentVisibleWgt->setForeground(TRMA_COL_VISIBLE, QBrush(QColor(48, 48, 48)));
	}

}

