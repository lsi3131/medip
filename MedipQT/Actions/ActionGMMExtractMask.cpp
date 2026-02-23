/*****************************************************************//**
 * \file   ActionGMMExtractMask.cpp
 * \brief  GMM 경계값을 통한 Mask 추출 클래스 헤더구현
 *
 * \author heog
 * \date   March 2022
 *********************************************************************/

#include "stdafx.h"
#include "ActionGMMExtractMask.h"
#include "windowManager.h"
#include "AnalysisWidget.h"
#include "Tabwindow.h"
#include "stringManager.h"
#include <QDesktopServices>


 //////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///  Start ActionGMMExtractMask class
 //////////////////////////////////////////////////////////////////////////////////////////////////////////
 /**
  * \brief	ActionGMMExtractMask 생성자
  *
  * \return
 */
ActionGMMExtractMask::ActionGMMExtractMask()
{

}

/**
 * \brief ActionGMMExtractMask 생성자
 *
 * \param UId			Mask 아이디
 * \param nCluster		클러스터 개수
*/
ActionGMMExtractMask::ActionGMMExtractMask(MaskInfo* pMaskInfo, int nCluster, VOLUME_DATA* pVolumeData)
	:
	m_nCluster(nCluster)
	, m_pVolumeData(pVolumeData)
{
	m_GMMExtractMaskInfo.orgMaskInfo = pMaskInfo;
}

/**
 * \brief Thread 수행함수
 *
*/
void ActionGMMExtractMask::threadRun()
{
	emit progress(5);

	OnGMM();

	emit progress(50);

	OnCalcBorderValue();

	emit progress(70);

	emit sig_updateResult((void*)(&m_GMMExtractMaskInfo));

	emit finished();

	emit progress(100);
}

/**
 * \brief GMM 수행함수
 *
*/
void	ActionGMMExtractMask::OnGMM()
{
	mask8 maskBit = m_pVolumeData->getMask(m_GMMExtractMaskInfo.orgMaskInfo->uid);

	int byteIdx = m_pVolumeData->GetMaskByteIndex(m_GMMExtractMaskInfo.orgMaskInfo->uid);
	mask8* pMaskData = m_pVolumeData->getMaskDataPoint(byteIdx);

	mint16* pHUData = m_pVolumeData->getHUDataPoint();

	muint32 cx = m_pVolumeData->getCX();
	muint32 cy = m_pVolumeData->getCY();
	muint32 cz = m_pVolumeData->getCZ();

	m_GMMExtractMaskInfo.vecGMMDatas.clear();
	getGMMOverlapValue(pMaskData, pHUData, cx, cy, cz, maskBit, m_nCluster, m_GMMExtractMaskInfo.vecGMMDatas);
}

/**
 * \brief 경계값 수행함수
 *
*/
void	ActionGMMExtractMask::OnCalcBorderValue()
{
	m_GMMExtractMaskInfo.vecBorderVal.resize(m_nCluster - 1);

	for (int i = 0; i < m_nCluster - 1; ++i)
	{
		double& g0_mean = m_GMMExtractMaskInfo.vecGMMDatas[i].MeanCluster;
		double& g0_sd = m_GMMExtractMaskInfo.vecGMMDatas[i].SD;

		double& g1_mean = m_GMMExtractMaskInfo.vecGMMDatas[i + 1].MeanCluster;
		double& g1_sd = m_GMMExtractMaskInfo.vecGMMDatas[i + 1].SD;

		m_GMMExtractMaskInfo.vecBorderVal[i].push_back(g0_mean + g0_sd);
		m_GMMExtractMaskInfo.vecBorderVal[i].push_back(g1_mean - g1_sd);
	}
}

/**
 * \brief Mask 추출함수
 *
*/
void ActionGMMExtractMask::OnExtractMask(VOLUME_DATA* pVolumeData, GMMExtractMaskInfo& gmmInfo)
{
	gmmInfo.vecNewMaskInfo.clear();
	gmmInfo.vecNewMaskByteIdx.clear();
	gmmInfo.vecNewMaskBit.clear();

	int n_cluster = (int)gmmInfo.vecGMMDatas.size();

	for (int i = 0; i < n_cluster - 1; ++i)
	{
		// 1 Cluster 당 3구역 경계구분
		for (int j = 0; j < 3; ++j)
		{
			bool bResult = pVolumeData->createMaskInfo(false, false);

			MaskInfo* new_mask_Info = pVolumeData->getCurrentMaskInfo();

			int maskByteIdx = new_mask_Info->uid >= MASK_SECOND_MAX ? (new_mask_Info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			uchar maskBit = maskByteIdx == 0 ? new_mask_Info->mask_id : new_mask_Info->mask_id2;

			gmmInfo.vecNewMaskInfo.push_back(new_mask_Info);
			gmmInfo.vecNewMaskByteIdx.push_back(maskByteIdx);
			gmmInfo.vecNewMaskBit.push_back(maskBit);
		}
	}

	// Origin Mask bit와 GMM 경계값 비교 통한 마스크 추출
	const int	nHeight = pVolumeData->getCY();
	const int	nWidth = pVolumeData->getCX();
	const int	nSlice = pVolumeData->getCZ();

	MaskInfo* orgMaskInfo = pVolumeData->getMaskInfo(gmmInfo.orgMaskInfo->uid, true);

	gmmInfo.orgMaskInfo = orgMaskInfo;

	int			orgMaskByteIdx = orgMaskInfo->uid >= MASK_SECOND_MAX ? (orgMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	uchar		orgMaskBit = orgMaskByteIdx == 0 ? orgMaskInfo->mask_id : orgMaskInfo->mask_id2;

	int sz_border = (int)gmmInfo.vecBorderVal.size();

	gmmInfo.vecCount.clear();
	gmmInfo.vecCount.resize((n_cluster - 1) * 3, 0);

	int tot_count = 0;
	for (int z = 0; z < nSlice; ++z)
	{
		for (int y = 0; y < nHeight; ++y)
		{
			for (int x = 0; x < nWidth; ++x)
			{
				// Check Origin Mask bit 
				if (pVolumeData->isMaskBit(x, y, z, orgMaskBit, orgMaskByteIdx))
				{
					tot_count++;

					// GMM 경계값 비교를 통한 
					mint16 huVal = pVolumeData->getData(x, y, z);

					for (int l = 0; l < sz_border; ++l)
					{
						if (l == 0)
						{
							if (gmmInfo.vecBorderVal[l][0] < huVal)
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[0], gmmInfo.vecNewMaskByteIdx[0]);
								gmmInfo.vecCount[0]++;

								l = sz_border;
							}
							else if ((gmmInfo.vecBorderVal[l][0] >= huVal) && (gmmInfo.vecBorderVal[l][1] <= huVal))
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[1], gmmInfo.vecNewMaskByteIdx[1]);
								gmmInfo.vecCount[1]++;

								l = sz_border;
							}
							else
							{
								if (sz_border > 1)
								{
									if ((gmmInfo.vecBorderVal[l][1] > huVal) && (gmmInfo.vecBorderVal[l + 1][0] <= huVal))
									{
										pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[2], gmmInfo.vecNewMaskByteIdx[2]);

										l = sz_border;

										gmmInfo.vecCount[2]++;
									}
								}
								else
								{
									pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[2], gmmInfo.vecNewMaskByteIdx[2]);

									l = sz_border;

									gmmInfo.vecCount[2]++;
								}
							}
						}
						else if ((l > 0) && (l == sz_border - 1))
						{
							if ((gmmInfo.vecBorderVal[l - 1][1] > huVal) && (gmmInfo.vecBorderVal[l][0] < huVal))
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[0], gmmInfo.vecNewMaskByteIdx[0]);
								gmmInfo.vecCount[l + (l % 3)]++;

								l = sz_border;
							}
							else if ((gmmInfo.vecBorderVal[l][0] >= huVal) && (gmmInfo.vecBorderVal[l][1] <= huVal))
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[1], gmmInfo.vecNewMaskByteIdx[1]);
								gmmInfo.vecCount[l + (l % 3)]++;

								l = sz_border;
							}
							else
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[2], gmmInfo.vecNewMaskByteIdx[2]);

								gmmInfo.vecCount[l + (l % 3)]++;

								l = sz_border;
							}
						}
						else
						{
							if ((gmmInfo.vecBorderVal[l - 1][1] > huVal) && (gmmInfo.vecBorderVal[l][0] < huVal))
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[0], gmmInfo.vecNewMaskByteIdx[0]);

								gmmInfo.vecCount[l + (l % 3)]++;

								l = sz_border;
							}
							else if ((gmmInfo.vecBorderVal[l][0] >= huVal) && (gmmInfo.vecBorderVal[l][1] <= huVal))
							{
								pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[1], gmmInfo.vecNewMaskByteIdx[1]);

								gmmInfo.vecCount[l + (l % 3)]++;

								l = sz_border;
							}
							else
							{
								if ((gmmInfo.vecBorderVal[l][1] > huVal) && (gmmInfo.vecBorderVal[l + 1][0] <= huVal))
								{
									pVolumeData->setMaskBit(x, y, z, gmmInfo.vecNewMaskBit[2], gmmInfo.vecNewMaskByteIdx[2]);

									gmmInfo.vecCount[l + (l % 3)]++;

									l = sz_border;
								}
							}
						}
					}
				}
			}
		}
	}

#ifdef _DEBUG
	// check voxel count
	int sum_count = std::accumulate(gmmInfo.vecCount.begin(), gmmInfo.vecCount.end(), 0);
	assert(sum_count == tot_count);
#endif 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///  End ActionGMMExtractMask class
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
///  Start ActionGMMExtractMaskAdd class
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * \brief			ActionGMMExtractMaskAdd 생성자
 *
 * \param pInfo		GMMExtractMaskInfo 구조체 포인터
*/
ActionGMMExtractMaskAdd::ActionGMMExtractMaskAdd(VOLUME_DATA* pVolumeData, int widgetIdx, GMMExtractMaskInfo* pInfo)
	: m_pWidget(nullptr)
	, m_redoInfo(*pInfo)
	, m_bFirst(true)
	, m_widgetIndex(widgetIdx)
	, m_pVolumeData(pVolumeData)
{

}

/**
 * \brief undo 수행함수
 *
*/
void ActionGMMExtractMaskAdd::undo()
{
	getWidget();

	if (!m_pWidget)
	{
		return;
	}

	deleteMaskUndo();

	updateGMMTableUndo();

	updateBorderTableUndo();
}

/**
 * \brief redo 수행함수
 *
*/
void ActionGMMExtractMaskAdd::redo()
{
	getWidget();

	if (!m_pWidget)
	{
		return;
	}

	makeUndoData();

	updateGmmTableRedo();

	updateBorderTableRedo();

	addMaskRedo();

	updateROIList();

	m_bFirst = false;
}

/**
 * \brief AnalysisWidget 가져옴
 *
*/
void ActionGMMExtractMaskAdd::getWidget()
{
	TabWindow* pTabList = WIN_MANAGER->GetTab();

	if (pTabList)
	{
		AnalysisTab* pAnalysisTab = pTabList->getAnalysisTab();
		if (pAnalysisTab)
		{
			QTabWidget* pTabWidget = pAnalysisTab->tabList;
			if (pTabWidget)
			{
				m_pWidget = dynamic_cast<AnalysisWidget*>(pTabWidget->widget(m_widgetIndex));

				return;
			}
		}
	}

	m_pWidget = nullptr;
}

/**
 * \brief 생성한 Mask 데이터 삭제
 *
*/
void ActionGMMExtractMaskAdd::deleteMaskUndo()
{
	VOLUME_DATA& volumeData = *m_pVolumeData;

	int n_mask = m_redoInfo.vecNewMaskInfo.size();

	for (int i = 0; i < n_mask; ++i)
	{
		int uid = volumeData.getMaskIndex(m_redoInfo.vecNewMaskInfo[i]->uid);

		volumeData.delMaskInfo(uid);
	}

	volumeData.forceUpdateMaskVolume();
	WIN_MANAGER->updateUI();
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->renderLater_GridView(true);
	WIN_MANAGER->setSaveState(false);
}

/**
 * \brief Undo 수행 시,  GMM QTable 업데이트 함수
 *
*/
void ActionGMMExtractMaskAdd::updateGMMTableUndo()
{
	if (!m_pWidget)
	{
		return;
	}

	QTableWidget* gmmTable = ((AnalysisWidget*)m_pWidget)->getGMMTable();

	if (gmmTable)
	{
		int n_cluster = m_undoInfo.vecGMMDatas.size();

		gmmTable->setRowCount(n_cluster);

		for (int i = 0; i < n_cluster; ++i)
		{
			double data[3] = { 0 };

			data[0] = m_undoInfo.vecGMMDatas[i].MeanCluster;
			data[1] = m_undoInfo.vecGMMDatas[i].SD;
			data[2] = m_undoInfo.vecGMMDatas[i].Prior;

			for (int j = 0; j < 3; ++j)
			{
				QString str_text = ((data[j] != -DBL_MAX) ? QString().sprintf("%lf", data[j]) : QString(""));

				QTableWidgetItem* item = gmmTable->item(i, j);

				if (!item)
				{
					item = new QTableWidgetItem();
					item->setText(str_text);
					item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
					gmmTable->setItem(i, j, item);
				}
				else
				{
					item->setText(str_text);
					item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
				}
			}
		}
	}
}

/**
 * \brief Undo 수행 시,  Border QTable 업데이트 함수
 *
*/
void ActionGMMExtractMaskAdd::updateBorderTableUndo()
{
	if (!m_pWidget)
	{
		return;
	}

	QTableWidget* borderTable = ((AnalysisWidget*)m_pWidget)->getBorderTable();

	if (borderTable)
	{
		int n_cluster = m_undoInfo.vecGMMDatas.size();

		borderTable->setRowCount(n_cluster - 1);

		for (int i = 0; i < n_cluster - 1; ++i)
		{
			for (int j = 0; j < borderTable->columnCount(); ++j)
			{
				QString str_text = ((m_undoInfo.vecBorderVal[i][j] != -DBL_MAX) ? QString().sprintf("%lf", m_undoInfo.vecBorderVal[i][j]) : QString(""));

				QTableWidgetItem* item = borderTable->item(i, j);
				if (!item)
				{
					item = new QTableWidgetItem();

					item->setText(str_text);
					item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

					borderTable->setItem(i, j, item);
				}
				else
				{
					item->setText(str_text);
					item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
				}
			}
		}
	}
}

/**
 * \brief Undo Data 생성함수
 *
*/
void ActionGMMExtractMaskAdd::makeUndoData()
{
	if (!m_pWidget)
	{
		return;
	}

	if (m_bFirst)
	{
		QTableWidget* gmmTable = ((AnalysisWidget*)m_pWidget)->getGMMTable();
		QTableWidget* borderTable = ((AnalysisWidget*)m_pWidget)->getBorderTable();

		int nCluster = gmmTable->rowCount();

		m_undoInfo.vecGMMDatas.clear();
		m_undoInfo.vecGMMDatas.resize(nCluster);

		for (int i = 0; i < nCluster; ++i)
		{
			double* data[3] = { NULL };

			data[0] = &(m_undoInfo.vecGMMDatas[i].MeanCluster);
			data[1] = &(m_undoInfo.vecGMMDatas[i].SD);
			data[2] = &(m_undoInfo.vecGMMDatas[i].Prior);

			for (int j = 0; j < 3; ++j)
			{
				QTableWidgetItem* item = gmmTable->item(i, j);

				if (item)
				{
					*(data[j]) = (item->text()).toDouble();

				}
				else
				{
					*(data[j]) = -DBL_MAX;
				}
			}
		}

		m_undoInfo.vecBorderVal.clear();
		m_undoInfo.vecBorderVal.resize(nCluster - 1);

		for (int i = 0; i < nCluster - 1; ++i)
		{
			QTableWidgetItem* item_g0_mean = gmmTable->item(i, 0);
			QTableWidgetItem* item_g0_sd = gmmTable->item(i, 1);
			QTableWidgetItem* item_g0_prior = gmmTable->item(i, 2);

			QTableWidgetItem* item_g1_mean = gmmTable->item(i + 1, 0);
			QTableWidgetItem* item_g1_sd = gmmTable->item(i + 1, 1);
			QTableWidgetItem* item_g1_prior = gmmTable->item(i + 1, 2);

			if (item_g0_mean)
			{
				double mean1 = (item_g0_mean->text()).toDouble();
				double sd1 = (item_g0_sd->text()).toDouble();
				double prior1 = (item_g0_prior->text()).toDouble();

				double mean2 = (item_g1_mean->text()).toDouble();
				double sd2 = (item_g1_sd->text()).toDouble();
				double prior2 = (item_g1_prior->text()).toDouble();

				m_undoInfo.vecBorderVal[i].push_back(mean1 + sd1);
				m_undoInfo.vecBorderVal[i].push_back(mean2 - sd2);
			}
			else
			{
				m_undoInfo.vecBorderVal[i].push_back(-DBL_MAX);
				m_undoInfo.vecBorderVal[i].push_back(-DBL_MAX);
			}
		}
	}
}

/**
 * \brief Redo 수행 시, Gmm QTable 업데이트 함수
 *
*/
void ActionGMMExtractMaskAdd::updateGmmTableRedo()
{
	QTableWidget* gmmTable = ((AnalysisWidget*)m_pWidget)->getGMMTable();

	if (!gmmTable)
	{
		return;
	}

	int n_cluster = m_redoInfo.vecGMMDatas.size();

	gmmTable->setRowCount(n_cluster);

	for (int i = 0; i < n_cluster; ++i)
	{
		double* data[3] = { NULL };

		data[0] = &(m_redoInfo.vecGMMDatas[i].MeanCluster);
		data[1] = &(m_redoInfo.vecGMMDatas[i].SD);
		data[2] = &(m_redoInfo.vecGMMDatas[i].Prior);

		for (int j = 0; j < 3; ++j)
		{
			QTableWidgetItem* item = gmmTable->item(i, j);

			if (item)
			{
				item->setText(QString().sprintf("%lf", *(data[j])));
				item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
			}
			else
			{
				item = new QTableWidgetItem();

				item->setText(QString().sprintf("%lf", *(data[j])));
				item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

				gmmTable->setItem(i, j, item);
			}
		}
	}
}

/**
 * \brief Redo 수행 시, Border QTable 업데이트 함수
 *
*/
void ActionGMMExtractMaskAdd::updateBorderTableRedo()
{
	QTableWidget* borderTable = ((AnalysisWidget*)m_pWidget)->getBorderTable();

	if (!borderTable)
	{
		return;
	}

	int n_cluster = m_redoInfo.vecGMMDatas.size();

	borderTable->setRowCount(n_cluster - 1);

	for (int i = 0; i < n_cluster - 1; ++i)
	{
		for (int j = 0; j < borderTable->columnCount(); ++j)
		{
			QTableWidgetItem* item = borderTable->item(i, j);
			if (!item)
			{
				item = new QTableWidgetItem();

				item->setText(QString().sprintf("%lf", m_redoInfo.vecBorderVal[i][j]));
				item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

				borderTable->setItem(i, j, item);
			}
			else
			{
				item->setText(QString().sprintf("%lf", m_redoInfo.vecBorderVal[i][j]));
				item->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
			}
		}
	}
}

/**
 * \brief Redo 수행 시, Mask 추가 함수
 *
*/
void ActionGMMExtractMaskAdd::addMaskRedo()
{
	ActionGMMExtractMask ExtractMask;

	ExtractMask.OnExtractMask(m_pVolumeData, m_redoInfo);
}

/**
 * \brief ROI 리스트 업데이트 함수
 *
*/
void ActionGMMExtractMaskAdd::updateROIList()
{
	VOLUME_DATA& volumeData = *m_pVolumeData;

	int n_cluster = m_redoInfo.vecGMMDatas.size();

	for (int i = 0; i < n_cluster - 1; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int uid = m_redoInfo.vecNewMaskInfo[i * 3 + j]->uid;

			volumeData.setMaskName(QString().sprintf("Mask%d", i * 3 + j), uid, true);

			volumeData.updateUIDBoundingBox(uid, true);
			volumeData.setVoxelCount(uid, m_redoInfo.vecCount[i * 3 + j], false);
			volumeData.setTAState(uid, false);
			WIN_MANAGER->applyVoxelToUI(uid);
		}

		volumeData.forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///  End ActionGMMExtractMaskAdd class
//////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * \brief
 *
 * \param pMaskInfo
 * \param pVolumeData
*/
ActionGMMExtractMaskNew::ActionGMMExtractMaskNew(
	std::vector<MaskInfo*>& vecMaskInfo,
	MaskInfo* pVMMaskInfo,
	VOLUME_DATA* pVolumeData,
	int nCluster
)
	: m_vecMaskInfo(vecMaskInfo)
	, m_VMMaskInfo(pVMMaskInfo)
	, m_pVolumeData(pVolumeData)
	, m_nCluster(nCluster)
{

}

/**
 * \brief
 *
*/
void ActionGMMExtractMaskNew::threadRun()
{
	emit progress(5);

	doGMM();

	emit progress(50);

	//calcBorderValue();
	calcBorderValueNew();

	emit progress(60);

	initvecGMMInfo();

	emit progress(70);

	//extractMask();
	extractMaskNew();

	emit progress(90);

	saveCSV();

	emit finished();
}

void ActionGMMExtractMaskNew::doGMM()
{
	mint16* pHUData = m_pVolumeData->getHUDataPoint();

	mask8 maskBit = m_pVolumeData->getMask(m_VMMaskInfo->uid);
	int byteIdx = m_pVolumeData->GetMaskByteIndex(m_VMMaskInfo->uid);

	mask8* pMaskData = m_pVolumeData->getMaskDataPoint(byteIdx);

	muint32 cx = m_pVolumeData->getCX();
	muint32 cy = m_pVolumeData->getCY();
	muint32 cz = m_pVolumeData->getCZ();

	m_vecGMMData.clear();
	getGMMOverlapValue(pMaskData, pHUData, cx, cy, cz, maskBit, m_nCluster, m_vecGMMData);
}

void ActionGMMExtractMaskNew::initvecGMMInfo()
{
	int nMask = (int)m_vecMaskInfo.size();

	m_vecGMMInfo.clear();
	m_vecGMMInfo.resize(nMask);

	for (int i = 0; i < nMask; ++i)
	{
		m_vecGMMInfo[i].vecGMMDatas = m_vecGMMData;

		m_vecGMMInfo[i].orgMaskInfo = m_vecMaskInfo[i];

		m_vecGMMInfo[i].vecBorderValNew = m_vecBorderVal;
	}
}

void ActionGMMExtractMaskNew::calcBorderValueNew()
{
	m_vecBorderVal.clear();

	for (int i = 0; i < m_nCluster; ++i)
	{
		double& mean = m_vecGMMData[i].MeanCluster;
		double& sd = m_vecGMMData[i].SD;

		if (i == 0)
		{
			m_vecBorderVal.push_back(mean + sd);
		}
		else
		{
			m_vecBorderVal.push_back(mean - sd);
		}
	}

	// check border value
	for (int i = 0; i < m_nCluster - 1; ++i)
	{
		if (m_vecBorderVal[i] > m_vecBorderVal[i + 1])
		{
			std::swap(m_vecBorderVal[i], m_vecBorderVal[i + 1]);
		}
	}


}

void ActionGMMExtractMaskNew::extractMaskNew()
{
	const int	nHeight = m_pVolumeData->getCY();
	const int	nWidth = m_pVolumeData->getCX();
	const int	nSlice = m_pVolumeData->getCZ();

	const float spaceX = m_pVolumeData->getSpaceX(true);
	const float spaceY = m_pVolumeData->getSpaceY(true);
	const float spaceZ = m_pVolumeData->getSpaceZ(true);

	float spaceXYZ = spaceX * spaceY * spaceZ;

	for (auto it = m_vecGMMInfo.begin(); it != m_vecGMMInfo.end(); ++it)
	{
		it->vecNewMaskInfo.clear();
		it->vecNewMaskByteIdx.clear();
		it->vecNewMaskBit.clear();

		QString name = QString::fromWCharArray(it->orgMaskInfo->maskName);

		int size = 3; // Myocardium, partial volume, cavity
		int n_cluster = (int)it->vecGMMDatas.size();

		// 3구역 경계구분
		for (int j = 0; j < size; ++j)
		{
			bool bResult = m_pVolumeData->createMaskInfo(false, false);

			MaskInfo* new_mask_Info = m_pVolumeData->getCurrentMaskInfo();

			int maskByteIdx = new_mask_Info->uid >= MASK_SECOND_MAX ? (new_mask_Info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			uchar maskBit = maskByteIdx == 0 ? new_mask_Info->mask_id : new_mask_Info->mask_id2;

			it->vecNewMaskInfo.push_back(new_mask_Info);
			it->vecNewMaskByteIdx.push_back(maskByteIdx);
			it->vecNewMaskBit.push_back(maskBit);
		}

		// Origin Mask bit와 GMM 경계값 비교 통한 마스크 추출
		int			orgMaskByteIdx = it->orgMaskInfo->uid >= MASK_SECOND_MAX ? (it->orgMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		uchar		orgMaskBit = orgMaskByteIdx == 0 ? it->orgMaskInfo->mask_id : it->orgMaskInfo->mask_id2;

		int sz_border = (int)it->vecBorderVal.size();

		it->vecCount.clear();
		it->vecCount.resize(size, 0);

		it->Sum = 0;
		it->Min = INT_MAX;
		it->Max = -INT_MAX;

		int lowThreshold = -DBL_MAX;
		int highThreshold = -DBL_MAX;

		// cluster 2인 경우만 적용
		if (m_nCluster == 2)
		{
			lowThreshold = int(std::round(it->vecBorderValNew[0]));
			highThreshold = int(std::round(it->vecBorderValNew[1]));
		}
		else if (m_nCluster == 3)
		{
			if (!name.compare("Lt. VM"))
			{
				lowThreshold = int(std::round(it->vecBorderValNew[0]));
				highThreshold = int(std::round(it->vecBorderValNew[1]));
			}
			else // Rt. VM
			{
				lowThreshold = int(std::round(it->vecBorderValNew[0]));
				highThreshold = int(std::round(it->vecBorderValNew[2]));
			}
		}

		int tot_count = 0;
		for (int z = 0; z < nSlice; ++z)
		{
			for (int y = 0; y < nHeight; ++y)
			{
				for (int x = 0; x < nWidth; ++x)
				{
					// Check Origin Mask bit 
					if (m_pVolumeData->isMaskBit(x, y, z, orgMaskBit, orgMaskByteIdx))
					{
						tot_count++;

						// GMM 경계값 비교를 통한 
						mint16 huVal = m_pVolumeData->getData(x, y, z);

						if (lowThreshold > huVal)
						{
							m_pVolumeData->setMaskBit(x, y, z, it->vecNewMaskBit[0], it->vecNewMaskByteIdx[0]);
							it->vecCount[0]++;
						}
						else if ((lowThreshold <= huVal) && (highThreshold >= huVal))
						{
							m_pVolumeData->setMaskBit(x, y, z, it->vecNewMaskBit[1], it->vecNewMaskByteIdx[1]);
							it->vecCount[1]++;

							it->Sum += huVal;

							if (it->Min > huVal)
							{
								it->Min = huVal;
							}

							if (it->Max < huVal)
							{
								it->Max = huVal;
							}
						}
						else
						{
							m_pVolumeData->setMaskBit(x, y, z, it->vecNewMaskBit[2], it->vecNewMaskByteIdx[2]);
							it->vecCount[2]++;
						}
					}
				}
			}
		}

		it->Mean = float(it->Sum) / float(it->vecCount[1]);

		it->MeanFraction = (it->Mean - it->Min) / (it->Max - it->Min);

		float cavity = it->vecCount[1] * it->MeanFraction;
		float myocardium = it->vecCount[1] - cavity;

		it->Myocardium = spaceXYZ * (it->vecCount[0] + myocardium) * 0.001f;
		it->Cavity = spaceXYZ * (it->vecCount[2] + cavity) * 0.001f;

		int sum_count = std::accumulate(it->vecCount.begin(), it->vecCount.end(), 0);

		for (int j = 0; j < 3; ++j)
		{
			int uid = it->vecNewMaskInfo[j]->uid;

			QString new_name;

			if (j == 0) new_name = name + "_myocardium";
			if (j == 1) new_name = name + "_partial";
			if (j == 2) new_name = name + "_cavity";

			::StringCbPrintf(it->vecNewMaskInfo[j]->maskName, MASKINFO_TEXT_LENGTH_MAX * sizeof(WCHAR), new_name.toStdWString().c_str());

			wprintf_s(L"maskName : %s\n", it->vecNewMaskInfo[j]->maskName);

			m_pVolumeData->updateUIDBoundingBox(uid, true);
			m_pVolumeData->setVoxelCount(uid, it->vecCount[j], false);
			m_pVolumeData->setTAState(uid, false);
		}
	}
}

void ActionGMMExtractMaskNew::checkFolder(QString& path)
{
	QDir dir(path);
	if (!dir.exists())
	{
		dir.mkdir(path);
	}
}

void ActionGMMExtractMaskNew::saveCSV()
{
	QString filePath = STRING_MANAGER->m_strTempFilePath + "SolutionResult";
	QString fileName = "Result.csv";

	QFileInfo info(filePath);

	checkFolder(STRING_MANAGER->m_strTempFilePath);
	checkFolder(filePath);

	QString fullName = filePath + '/' + fileName;

	QFile newFile(fullName);

	if (newFile.open(QIODevice::WriteOnly | QFile::Truncate | QIODevice::Append))
	{
		QTextStream out(&newFile);

		out << "\n";

		const int	nHeight = m_pVolumeData->getCY();
		const int	nWidth = m_pVolumeData->getCX();
		const int	nSlice = m_pVolumeData->getCZ();

		const float spaceX = m_pVolumeData->getSpaceX(true);
		const float spaceY = m_pVolumeData->getSpaceY(true);
		const float spaceZ = m_pVolumeData->getSpaceZ(true);

		float spaceXYZ = spaceX * spaceY * spaceZ;

		out << "," << "Info" << "," << "width" << "," << nWidth << '\n';
		out << "," << "," << "height" << "," << nHeight << '\n';
		out << "," << "," << "slice" << "," << nSlice << '\n';

		out << "," << "Spacing" << "," << "x" << "," << spaceX << '\n';
		out << "," << "," << "y" << "," << spaceY << '\n';
		out << "," << "," << "z" << "," << spaceZ << '\n';

		out << "\n";
		// LV
		out << "," << "LV (Voxel)" << "," << "Myocardium" << "," << m_vecGMMInfo[1].vecCount[0] << '\n';
		out << "," << "," << "Partial Volume" << "," << m_vecGMMInfo[1].vecCount[1] << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[1].vecCount[2] << '\n';

		out << "," << "LV (cm3)" << "," << "Myocardium" << "," << m_vecGMMInfo[1].vecCount[0] * spaceXYZ * 0.001f << '\n';
		out << "," << "," << "Partial Volume" << "," << m_vecGMMInfo[1].vecCount[1] * spaceXYZ * 0.001f << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[1].vecCount[2] * spaceXYZ * 0.001f << '\n';

		out << "," << "LV Partial volume" << "," << "min" << "," << m_vecGMMInfo[1].Min << '\n';
		out << "," << "," << "max" << "," << m_vecGMMInfo[1].Max << '\n';
		out << "," << "," << "count" << "," << m_vecGMMInfo[1].vecCount[1] << '\n';
		out << "," << "," << "sum" << "," << m_vecGMMInfo[1].Sum << '\n';
		out << "," << "," << "mean" << "," << m_vecGMMInfo[1].Mean << '\n';

		out << "," << "Result(cm3)" << "," << "Mean fraction" << "," << m_vecGMMInfo[1].MeanFraction << '\n';
		out << "," << "," << "Myocardium" << "," << m_vecGMMInfo[1].Myocardium << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[1].Cavity << '\n';

		out << "\n";
		// RV
		out << "," << "RV (Voxel)" << "," << "Myocardium" << "," << m_vecGMMInfo[0].vecCount[0] << '\n';
		out << "," << "," << "Partial Volume" << "," << m_vecGMMInfo[0].vecCount[1] << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[0].vecCount[2] << '\n';

		out << "," << "RV (cm3)" << "," << "Myocardium" << "," << m_vecGMMInfo[0].vecCount[0] * spaceXYZ * 0.001f << '\n';
		out << "," << "," << "Partial Volume" << "," << m_vecGMMInfo[0].vecCount[1] * spaceXYZ * 0.001f << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[0].vecCount[2] * spaceXYZ * 0.001f << '\n';

		out << "," << "RV Partial volume" << "," << "min" << "," << m_vecGMMInfo[0].Min << '\n';
		out << "," << "," << "max" << "," << m_vecGMMInfo[0].Max << '\n';
		out << "," << "," << "count" << "," << m_vecGMMInfo[0].vecCount[1] << '\n';
		out << "," << "," << "sum" << "," << m_vecGMMInfo[0].Sum << '\n';
		out << "," << "," << "mean" << "," << m_vecGMMInfo[0].Mean << '\n';

		out << "," << "Result(cm3)" << "," << "Mean fraction" << "," << m_vecGMMInfo[0].MeanFraction << '\n';
		out << "," << "," << "Myocardium" << "," << m_vecGMMInfo[0].Myocardium << '\n';
		out << "," << "," << "Cavity" << "," << m_vecGMMInfo[0].Cavity << '\n';

		// Info(width, height, slice)

		// Spacing(x,y,z)

		// LV
		// LV(voxel) -> Mayocardium, Partial volume, Cavity
		// LV(cm3) -> Mayocardium, Partial volume, Cavity
		// LV(partial vol) attenuation -> min, max, count, sum, mean
		// Result(Mean fraction / Myocardium / Cavity) 

		// RV
		// RV(voxel) -> Mayocardium, Partial volume, Cavity
		// RV(cm3) -> Mayocardium, Partial volume, Cavity
		// RV(partial vol) attenuation -> min, max, count, sum, mean
		// Result(Mean fraction / Myocardium / Cavity)

		newFile.close();
	}

	QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode));
}

