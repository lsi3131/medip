/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-12-09
@brief			CVoxelCountRatioDlg 구현파일
*/

#include "stdafx.h"
#include "CVoxelCountRatioDlg.h"
#include "stringManager.h"
#include "styleManager.h"

/*
@brief
*/
CVoxelCountRatioDlg::CVoxelCountRatioDlg(MedipQT *pParent, ROITab2* pRoiTab, VOLUME_DATA* p_volume_data)
	: m_p_ParentWidget(pParent),
	m_p_RoiTab(pRoiTab),
	m_p_VolumeData(p_volume_data)
{
	setWindowTitle(QString("Voxel Ratio Calculator"));

	init();
}

/*
@brief
*/
CVoxelCountRatioDlg::CVoxelCountRatioDlg(QString title, MedipQT *pParent, ROITab2* pRoiTab, VOLUME_DATA* p_volume_data)
	: m_p_ParentWidget(pParent),
	m_p_RoiTab(pRoiTab),
	m_p_VolumeData(p_volume_data)
{
	setWindowTitle(title);

	init();
}

/*
@brief
*/
CVoxelCountRatioDlg::~CVoxelCountRatioDlg()
{

}

/*
@brief
@return
*/
void CVoxelCountRatioDlg::init()
{
	QWidget* container = new QWidget(this);

	QVBoxLayout *layoutV = new QVBoxLayout;
	QHBoxLayout *layoutH_Top = new QHBoxLayout;
	QHBoxLayout *layoutH_Bottom = new QHBoxLayout;

	layoutV->addLayout(layoutH_Top);
	layoutV->addLayout(layoutH_Bottom);

	container->setStyleSheet("background-color: #414141;");
	QRect rect = container->geometry();
	rect.setWidth(m_p_ParentWidget->width());
	rect.setHeight(m_p_ParentWidget->height());
	container->setGeometry(rect);

	m_ListOrigin = new CListWidget(this, m_p_VolumeData);
	m_ListOrigin->header()->setMinimumSectionSize(20);
	m_ListOrigin->setColumnCount(L_COL_COUNT);
	m_ListOrigin->setColumnWidth(L_COL_ALPHA, 20);
	m_ListOrigin->setColumnWidth(L_COL_COLOR, 20);
	m_ListOrigin->setColumnWidth(L_COL_SHOW, 20);
	m_ListOrigin->setColumnWidth(L_COL_AI, 20);
	m_ListOrigin->setHeaderHidden(true);
	m_ListOrigin->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_ListOrigin->setAutoFillBackground(true);
	m_ListOrigin->setContentsMargins(0, 0, 0, 0);
	m_ListOrigin->setFixedSize(QSize(250, 500));
	
	layoutH_Top->addWidget(m_ListOrigin, 0, Qt::AlignTop);

	m_btnArrow1 = new QPushButton(this);
	m_btnArrow1->setText("->");
	m_btnArrow1->setFixedSize(60, 50);
	m_btnArrow1->setVisible(true);
	m_btnArrow1->setAutoDefault(false);
	m_btnArrow1->setDefault(false);
	m_btnArrow1->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnArrow1->setMouseTracking(true);
	m_btnArrow1->installEventFilter(this);
	layoutH_Top->addWidget(m_btnArrow1);

	//m_btnArrow11 = new QPushButton(this);
	//m_btnArrow11->setText("<-");
	//m_btnArrow11->setFixedSize(60, 50);
	//m_btnArrow11->setVisible(true);
	//m_btnArrow11->setAutoDefault(false);
	//m_btnArrow11->setDefault(false);
	//m_btnArrow11->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//layoutH_Top->addWidget(m_btnArrow11);

	m_btnArrow2 = new QPushButton(this);
	m_btnArrow2->setText("->");
	m_btnArrow2->setFixedSize(60, 50);
	m_btnArrow2->setVisible(true);
	m_btnArrow2->setAutoDefault(false);
	m_btnArrow2->setDefault(false);
	m_btnArrow2->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnArrow2->setMouseTracking(true);
	m_btnArrow2->installEventFilter(this);
	layoutH_Top->addWidget(m_btnArrow2);

	//m_btnArrow21 = new QPushButton(this);
	//m_btnArrow21->setText("<-");
	//m_btnArrow21->setFixedSize(60, 50);
	//m_btnArrow21->setVisible(true);
	//m_btnArrow21->setAutoDefault(false);
	//m_btnArrow21->setDefault(false);
	//m_btnArrow21->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//layoutH_Top->addWidget(m_btnArrow21);

	m_ListSelection1 = new CListWidget(this, m_p_VolumeData);
	m_ListSelection1->header()->setMinimumSectionSize(20);
	m_ListSelection1->setColumnCount(L_COL_COUNT);
	m_ListSelection1->setColumnWidth(L_COL_ALPHA, 20);
	m_ListSelection1->setColumnWidth(L_COL_COLOR, 20);
	m_ListSelection1->setColumnWidth(L_COL_SHOW, 20);
	m_ListSelection1->setColumnWidth(L_COL_AI, 20);
	m_ListSelection1->setHeaderHidden(true);
	m_ListSelection1->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_ListSelection1->setAutoFillBackground(true);
	m_ListSelection1->setContentsMargins(0, 0, 0, 0);
	m_ListSelection1->setFixedSize(QSize(250, 225));

	layoutH_Top->addWidget(m_ListSelection1, 0, Qt::AlignRight);

	m_lbVoxelCount1 = new QLabel(this);
	m_lbVoxelCount1->setText("Voxel Sum1 : 0\n");
	m_lbVoxelCount1->setStyleSheet("color : white; font-weight : bold;");
	m_lbVoxelCount1->setAlignment(Qt::AlignLeft);
	m_lbVoxelCount1->setFixedWidth(250);
	layoutH_Top->addWidget(m_lbVoxelCount1);

	m_ListSelection2 = new CListWidget(this, m_p_VolumeData);
	m_ListSelection2->header()->setMinimumSectionSize(20);
	m_ListSelection2->setColumnCount(L_COL_COUNT);
	m_ListSelection2->setColumnWidth(L_COL_ALPHA, 20);
	m_ListSelection2->setColumnWidth(L_COL_COLOR, 20);
	m_ListSelection2->setColumnWidth(L_COL_SHOW, 20);
	m_ListSelection2->setColumnWidth(L_COL_AI, 20);
	m_ListSelection2->setHeaderHidden(true);
	m_ListSelection2->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_ListSelection2->setAutoFillBackground(true);
	m_ListSelection2->setContentsMargins(0, 0, 0, 0);
	m_ListSelection2->setFixedSize(QSize(250, 225));

	layoutH_Top->addWidget(m_ListSelection2, 0, Qt::AlignRight);

	m_lbVoxelCount2 = new QLabel(this);
	m_lbVoxelCount2->setText("Voxel Sum2 : 0\n");
	m_lbVoxelCount2->setStyleSheet("color : white; font-weight : bold;");
	m_lbVoxelCount2->setAlignment(Qt::AlignLeft);
	m_lbVoxelCount2->setFixedWidth(250);
	layoutH_Top->addWidget(m_lbVoxelCount2);

	m_lbVoxelRatio = new QLabel(this);
	m_lbVoxelRatio->setText("Voxel(sum1 / sum2) = 0 %\n");
	m_lbVoxelRatio->setStyleSheet("color : white; font-weight : bold;");
	m_lbVoxelRatio->setAlignment(Qt::AlignRight);
	m_lbVoxelRatio->setFixedWidth(300);
	layoutH_Bottom->addWidget(m_lbVoxelRatio);

	//m_btnCalculate = new QPushButton(this);
	//m_btnCalculate->setText("Calculate");
	//m_btnCalculate->setFixedSize(100, 50);
	//m_btnCalculate->setVisible(true);
	//m_btnCalculate->setAutoDefault(false);
	//m_btnCalculate->setDefault(false);
	//m_btnCalculate->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//m_btnCalculate->setMouseTracking(true);
	//m_btnCalculate->installEventFilter(this);
	//layoutH_Bottom->addWidget(m_btnCalculate);

	m_btnCancel = new QPushButton(this);
	m_btnCancel->setText(STRING_MANAGER->getString(STR_CANCEL));
	m_btnCancel->setFixedSize(60, 50);
	m_btnCancel->setVisible(true);
	m_btnCancel->setAutoDefault(false);
	m_btnCancel->setDefault(false);
	m_btnCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCancel->setMouseTracking(true);
	m_btnCancel->installEventFilter(this);
	layoutH_Bottom->addWidget(m_btnCancel);

	m_lbVoxelCountOrg = new QLabel(this);
	m_lbVoxelCountOrg->setText("Voxel Count : 0\n");
	m_lbVoxelCountOrg->setStyleSheet("color : white; font-weight : bold;");
	m_lbVoxelCountOrg->setAlignment(Qt::AlignLeft);
	m_lbVoxelCountOrg->setFixedWidth(250);
	layoutH_Bottom->addWidget(m_lbVoxelCountOrg);
	
	//connect(m_btnCalculate, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnCalculate);
	connect(m_btnCancel, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnCancel);

	connect(m_btnArrow1, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnInsertList1);
	connect(m_btnArrow2, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnInsertList2);
	//connect(m_btnArrow11, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnRemoveList1);
	//connect(m_btnArrow21, &QPushButton::clicked, this, &CVoxelCountRatioDlg::OnRemoveList2);
	
	setLayout(layoutV);

	setFixedSize(620, 620);

	if (m_p_RoiTab)
	{
		MaskListWidget* plist_widget = m_p_RoiTab->ROIList;

		if (plist_widget)
		{
			int n_items = plist_widget->topLevelItemCount();

			if (n_items > 0)
			{
				QList<QTreeWidgetItem*> list_items;
				QList<muint32> list_uid;
				for (int i = 0; i < n_items; ++i)
				{
					list_items.push_back(plist_widget->topLevelItem(i));
					list_uid.push_back(i);
				}

				m_ListOrigin->update(list_items, list_uid);

				if (m_p_VolumeData)
				{
					muint32 voxel_count = voxelcount(list_uid);

					m_lbVoxelCountOrg->blockSignals(true);
					m_lbVoxelCountOrg->setText(QString().sprintf("Voxel Sum : %d\n", voxel_count));
					m_lbVoxelCountOrg->blockSignals(false);
				}
			}
		}
	}

	m_nVoxelCount[0] = 0;
	m_nVoxelCount[1] = 0;
	
}

/*
@brief
@return
*/
bool		CVoxelCountRatioDlg::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == NULL)
	{
		return QWidget::eventFilter(watched, event);
	}

	QEvent::Type event_type = event->type(); 

	//if (event_type == QEvent::Resize)
	{
		UpdateCtrlPosition();
	}

	return QWidget::eventFilter(watched, event);
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::UpdateCtrlPosition()
{
	int width = this->width();
	int height = this->height();

	QPoint pos;
	QSize  size;

	m_ListOrigin->move(20, 20);

	size = m_ListOrigin->size();
	m_lbVoxelCountOrg->move(20, size.height() + 30);

	size = m_ListOrigin->size();
	m_ListSelection1->move(100 + size.width(), 20);

	m_lbVoxelCount1->move(100 + size.width(), m_ListSelection1->height() + 30);

	size = m_btnArrow1->size();
	m_btnArrow1->move(220 + size.width(), 50 + size.height());

	size = m_btnArrow2->size();
	m_btnArrow2->move(220 + size.width(), 340 + size.height());

	//size = m_btnArrow1->size();
	//m_btnArrow1->move(220 + size.width(), 30 + size.height());

	//size = m_btnArrow2->size();
	//m_btnArrow2->move(220 + size.width(), 330 + size.height());

	//size = m_btnArrow11->size();
	//m_btnArrow11->move(220 + size.width(), 90 + size.height());

	//size = m_btnArrow21->size();
	//m_btnArrow21->move(220 + size.width(), 250 + size.height());

	size = m_ListSelection2->size();
	m_ListSelection2->move(100 + size.width(), 70 + size.height());

	m_lbVoxelCount2->move(100 + size.width(), 80 + size.height() + m_ListSelection2->height());

	size = m_btnCancel->size();
	m_btnCancel->move(width - size.width() - 20, height - size.height() - 15);

	//pos = m_btnCancel->pos();
	//size = m_btnCalculate->size();
	//m_btnCalculate->move(pos.x() - size.width() - 20, pos.y());

	pos = m_btnCancel->pos();
	size = m_lbVoxelRatio->size();
	m_lbVoxelRatio->move(pos.x() - size.width() - 10, pos.y() + 20);

	this->update();
}

/*
@brief
@return
*/
muint32		CVoxelCountRatioDlg::voxelcount(QList<muint32> & list_uid)
{
	muint32 voxel = 0;
	for (int i = 0; i < list_uid.size(); ++i)
	{
		MaskInfo* info = m_p_VolumeData->getMaskInfo(list_uid[i]);
			
		voxel += m_p_VolumeData->getVoxelCount(info->uid);
	}

	return voxel;
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnInsertList1()
{
	if (m_ListOrigin)
	{
		if (m_ListOrigin)
		{
			int count = m_ListOrigin->topLevelItemCount();

			if (count > 0)
			{
				QList<QTreeWidgetItem*> list = m_ListOrigin->selectedItems();
				QList<muint32> list_idx;

				for (int i = 0; i < list.size(); ++i)
				{
					list_idx.insert(i, m_ListOrigin->indexOfTopLevelItem(list[i]));
				}

				m_ListSelection1->update(list, list_idx);

				if (m_p_VolumeData)
				{
					m_nVoxelCount[0]= voxelcount(list_idx);

					m_lbVoxelCount1->setText(QString().sprintf("Voxel Sum1 : %d\n", m_nVoxelCount[0]));

					if (m_nVoxelCount[0] != 0)
					{
						m_lbVoxelRatio->setText(QString().sprintf("Voxel(Sum1 / Sum2) : %.2f %\n", (float)m_nVoxelCount[0] / (float)m_nVoxelCount[1] * 100.f));
					}
					else
					{
						m_lbVoxelRatio->setText(QString().sprintf("Voxel(Sum1 / Sum2) : %.2f %\n", 0.f));
					}
				}
			}
		}
	}
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnRemoveList1()
{
	if (m_ListSelection1)
	{

	}
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnInsertList2()
{
	if (m_ListOrigin)
	{
		if (m_ListOrigin)
		{
			int count = m_ListOrigin->topLevelItemCount();

			if (count > 0)
			{
				QList<QTreeWidgetItem*> list = m_ListOrigin->selectedItems();
				QList<muint32> list_idx;

				for (int i = 0; i < list.size(); ++i)
				{
					list_idx.insert(i, m_ListOrigin->indexOfTopLevelItem(list[i]));
				}

				m_ListSelection2->update(list, list_idx);

				if (m_p_VolumeData)
				{
					m_nVoxelCount[1] = voxelcount(list_idx);

					m_lbVoxelCount2->setText(QString().sprintf("Voxel Sum2 : %d\n", m_nVoxelCount[1]));

					if (m_nVoxelCount[0] != 0)
					{
						m_lbVoxelRatio->setText(QString().sprintf("Voxel(Sum1 / Sum2) : %.2f %\n", (float)m_nVoxelCount[0] / (float)m_nVoxelCount[1] * 100.f));
					}
					else
					{
						m_lbVoxelRatio->setText(QString().sprintf("Voxel(Sum1 / Sum2) : %.2f %\n", 0.f));
					}
				}
			}
		}
	}
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnRemoveList2()
{
	if (m_ListSelection2)
	{

	}
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnCalculate()
{
	//this->close();
}

/*
@brief
@return
*/
void		CVoxelCountRatioDlg::OnCancel()
{
	this->close();
}
