#include "stdafx.h"
#include "ActionPredictAddEx.h"
#include "Actions\ActionCuda.h"
#include "Windows\windowManager.h"
#include "Windows\Tabwindow.h"
#include "System\stringManager.h"
#include "Actions\ActionManager.h"
#include "algorithm\Radiomics.h"

#include "DeepInsthink.h"
#include "Definitions.h"
#include "Metadata.h"
#include "Network/Network.h"
#include "LicenseManager.h"

ActionPredictAddEx::ActionPredictAddEx(
	VOLUME_DATA* pVolumeData,
	ProductManager* pProductManager,
	int start, 
	int end, 
	int outClassCnt, 
	int nThreshold,
	AIClassNameList vecAIName,
	AIClassResultList vecAIresult,
	bool isGMMAutoApply,
	int nWeightType, 
	QUndoCommand* parent) : 
	QUndoCommand(parent),
	m_pVolumeData(pVolumeData),
	m_pProductManager(pProductManager),
	m_isGMMAutoApply(isGMMAutoApply),
	m_nThresholdValue(nThreshold)
{
	static int s_id = ACT_ID_DD_PREDICT;
	m_id = s_id++;
	m_nResultClassCnt = outClassCnt;
	m_vUndoRedoMaskByteIndex[1] = m_vUndoRedoMaskByteIndex[0] = m_pVolumeData->GetMaskPointCount();
	m_nWeightType = nWeightType;

	m_vNewMaskInfo.reserve(outClassCnt);
	m_vNewMaskUID.reserve(outClassCnt);
	m_vNewMaskVoxelCnt.reserve(outClassCnt);
	m_vNewMaskBoundingBox.reserve(outClassCnt);
	m_vecAIresult = std::move(vecAIresult);
	m_vecAIName = std::move(vecAIName);
}

void ActionPredictAddEx::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		bool res;
		std::vector<UIDMask> vec;
		m_pVolumeData->delMaskInfos(m_vNewMaskUID, vec, res);
		m_pVolumeData->setCurrentMaskIndex(0);
		MaskInfo* info = m_pVolumeData->getMaskInfo(0);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_vUndoRedoMaskByteIndex[0]; i++)
			{
				m_pVolumeData->createTempMaskData();
				file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
				m_pVolumeData->applyTempMask(i);
			}
			file.close();

			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(true);
		}
		for (int i = 0; i < m_nResultClassCnt; i++)
		{
			int uid = m_vNewMaskInfo[i].uid;

			m_pVolumeData->setBoundingBox(uid, BoundingBoxI(), true);
			m_pVolumeData->setVoxelCount(uid, 0);
			m_pVolumeData->setTAState(uid, false);
		}

		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionPredictAddEx::redo()
{
	//const QString outPath = STRING_MANAGER->AISegmentationPath + "/predict/result";

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_vUndoRedoMaskByteIndex[1]; i++)
			{
				m_pVolumeData->createTempMaskData();
				file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
				m_pVolumeData->applyTempMask(i);
			}

			file.close();

			for (int i = 0; i < m_nResultClassCnt; i++)
			{
				m_pVolumeData->insertMaskInfo(m_vNewMaskUID[i], m_vNewMaskInfo[i]);
			}
		}
		// thread 결과
		else
		{

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_vUndoRedoMaskByteIndex[0]; i++)
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				file.close();
			}


			m_pVolumeData->createTempMaskData();

			mask* _out = m_pVolumeData->getMaskTempDataPoint();

			muint32 cx, cy, cz;

			m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

			QString FilePath;
			for (int i = 0; i < m_nResultClassCnt; i++)
			{
				bool bResult = m_pVolumeData->createMaskInfo();

				if (bResult)
				{
					MaskInfo* tempInfo = m_pVolumeData->getCurrentMaskInfo();
					m_vNewMaskInfo.push_back(*tempInfo);
					m_vNewMaskUID.push_back(m_vNewMaskInfo[i].uid);

					int maskByteIdx = m_vNewMaskInfo[i].uid >= MASK_SECOND_MAX ? (m_vNewMaskInfo[i].uid - MASK_SECOND_MAX) / 8 + 1 : 0;
					mask maskBit = maskByteIdx == 0 ? m_vNewMaskInfo[i].mask_id : m_vNewMaskInfo[i].mask_id2;

					// result
					m_pVolumeData->m_vecAIResultData.emplace_back(m_vNewMaskInfo[i].uid, m_vecAIresult[i]);
					/*int nVoxelCnt = m_pVolumeData->applyTempMaskBitOutset(m_vecAIresult[i], maskBit, maskByteIdx, m_nThresholdValue);*/

					int nOutsetVal = 256 / 2 - 1;
					if (m_isGMMAutoApply)
					{
						// gmm 연산						
						std::vector<mask8> vecAIMaskData(m_pVolumeData->getVolumeDataLength(), 0);
						std::vector<mint16> vecAIProbabilityData(m_pVolumeData->getVolumeDataLength(), 0);

						for (int j = 0; j < m_vecAIresult[i].size(); ++j)
						{
							if (m_vecAIresult[i][j] > 0)
							{
								vecAIMaskData[j] |= maskBit;
							}
							vecAIProbabilityData[j] = m_vecAIresult[i][j];
						}

						muint32 cx = m_pVolumeData->getCX();
						muint32 cy = m_pVolumeData->getCY();
						muint32 cz = m_pVolumeData->getCZ();

						int num = 2;
						std::vector<GMMOverlapedData> GMMDatas;
						getGMMOverlapValue(&vecAIMaskData[0], &vecAIProbabilityData[0], cx, cy, cz, maskBit, num, GMMDatas);

						double gmmMid = 256.0 / 2.0;
						if (GMMDatas.size() >= num)
						{
							double sum = GMMDatas[num - 1].MeanCluster + GMMDatas[num - 2].MeanCluster;
							gmmMid = sum / 2.0;
						}
						nOutsetVal = (int)round(gmmMid);
					}

					int nVoxelCnt = m_pVolumeData->applyTempMaskBitOutset(m_vecAIresult[i], maskBit, maskByteIdx, nOutsetVal);
					// 각 layer의 outset 값 저장
					m_pVolumeData->m_vecAIOutset.emplace_back(m_vNewMaskInfo[i].uid, nOutsetVal);	// 0~255

					m_vNewMaskVoxelCnt.push_back(nVoxelCnt);
					m_pVolumeData->updateUIDBoundingBox(m_vNewMaskInfo[i].uid, true);
					m_vNewMaskBoundingBox.push_back(m_pVolumeData->getBoundingBox(m_vNewMaskInfo[i].uid));

					/////////////////////
					if (m_vecAIName.size() > i)
					{
						QString layerName = QString::fromUtf8(m_vecAIName[i].c_str());
						//////// 이름 중복 처리						
						m_pVolumeData->setMaskName(layerName, tempInfo->uid, true); // layer naming

						DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();
						if (pPredictedInfo)
						{
							pPredictedInfo->mapDeepCatchPredictResultRoi.insert(layerName, tempInfo->uid);
						}

						if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch))
						{
							if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN))
							{
								tempInfo->color = COLOR(5, 144, 189, 255);
								tempInfo->layerAlpha = 50;
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE))
							{
								tempInfo->color = COLOR(232, 232, 232, 255);
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE))
							{
								tempInfo->color = COLOR(227, 78, 85, 255);
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF))
							{
								tempInfo->color = COLOR(104, 178, 160, 255);
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
							{
								tempInfo->color = COLOR(255, 255, 36, 255);
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO))
							{
								tempInfo->color = COLOR(99, 0, 132, 255);
							}
							else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
							{
								tempInfo->color = COLOR(236, 194, 191, 255);
							}
						}

						if (m_nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
						{
							tempInfo->bIsUseCredit = true;
						}
					}
				}
			}


			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				m_vUndoRedoMaskByteIndex[1] = m_pVolumeData->GetMaskPointCount();
				for (int i = 0; i < m_vUndoRedoMaskByteIndex[1]; i++)
				{
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				}
				file.close();
			}

		}

		for (int i = 0; i < m_nResultClassCnt; i++)
		{
			int uid = m_vNewMaskInfo[i].uid;

			m_pVolumeData->setBoundingBox(uid, m_vNewMaskBoundingBox[i]);
			m_pVolumeData->setVoxelCount(uid, m_vNewMaskVoxelCnt[i], false);
			m_pVolumeData->setTAState(uid, false);
			WIN_MANAGER->applyVoxelToUI(uid);
		}

		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionPredictAddEx::Do()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);

		m_pVolumeData->createTempMaskData();

		mask* _out = m_pVolumeData->getMaskTempDataPoint();

		muint32 cx, cy, cz;

		m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

		QString FilePath;
		for (int i = 0; i < m_nResultClassCnt; i++)
		{
			bool bResult = m_pVolumeData->createMaskInfo();

			if (bResult)
			{
				MaskInfo* tempInfo = m_pVolumeData->getCurrentMaskInfo();
				m_vNewMaskInfo.push_back(*tempInfo);
				m_vNewMaskUID.push_back(m_vNewMaskInfo[i].uid);

				int maskByteIdx = m_vNewMaskInfo[i].uid >= MASK_SECOND_MAX ? (m_vNewMaskInfo[i].uid - MASK_SECOND_MAX) / 8 + 1 : 0;
				mask maskBit = maskByteIdx == 0 ? m_vNewMaskInfo[i].mask_id : m_vNewMaskInfo[i].mask_id2;

				// result
				m_pVolumeData->m_vecAIResultData.emplace_back(m_vNewMaskInfo[i].uid, m_vecAIresult[i]);
				/*int nVoxelCnt = m_pVolumeData->applyTempMaskBitOutset(m_vecAIresult[i], maskBit, maskByteIdx, m_nThresholdValue);*/

				int nOutsetVal = 256 / 2 - 1;
				if (m_isGMMAutoApply)
				{
					// gmm 연산						
					std::vector<mask8> vecAIMaskData(m_pVolumeData->getVolumeDataLength(), 0);
					std::vector<mint16> vecAIProbabilityData(m_pVolumeData->getVolumeDataLength(), 0);

					for (int j = 0; j < m_vecAIresult[i].size(); ++j)
					{
						if (m_vecAIresult[i][j] > 0)
						{
							vecAIMaskData[j] |= maskBit;
						}
						vecAIProbabilityData[j] = m_vecAIresult[i][j];
					}

					muint32 cx = m_pVolumeData->getCX();
					muint32 cy = m_pVolumeData->getCY();
					muint32 cz = m_pVolumeData->getCZ();

					int num = 2;
					std::vector<GMMOverlapedData> GMMDatas;
					getGMMOverlapValue(&vecAIMaskData[0], &vecAIProbabilityData[0], cx, cy, cz, maskBit, num, GMMDatas);

					double gmmMid = 256.0 / 2.0;
					if (GMMDatas.size() >= num)
					{
						double sum = GMMDatas[num - 1].MeanCluster + GMMDatas[num - 2].MeanCluster;
						gmmMid = sum / 2.0;
					}
					nOutsetVal = (int)round(gmmMid);
				}

				int nVoxelCnt = m_pVolumeData->applyTempMaskBitOutset(m_vecAIresult[i], maskBit, maskByteIdx, nOutsetVal);
				// 각 layer의 outset 값 저장
				m_pVolumeData->m_vecAIOutset.emplace_back(m_vNewMaskInfo[i].uid, nOutsetVal);	// 0~255

				m_vNewMaskVoxelCnt.push_back(nVoxelCnt);
				m_pVolumeData->updateUIDBoundingBox(m_vNewMaskInfo[i].uid, true);
				m_vNewMaskBoundingBox.push_back(m_pVolumeData->getBoundingBox(m_vNewMaskInfo[i].uid));

				/////////////////////
				if (m_vecAIName.size() > i)
				{
					QString layerName = QString::fromUtf8(m_vecAIName[i].c_str());
					//////// 이름 중복 처리						
					m_pVolumeData->setMaskName(layerName, tempInfo->uid, true); // layer naming

					DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();
					if (pPredictedInfo)
					{
						pPredictedInfo->mapDeepCatchPredictResultRoi.insert(layerName, tempInfo->uid);
					}

					if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch))
					{
						if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN))
						{
							tempInfo->color = COLOR(5, 144, 189, 255);
							tempInfo->layerAlpha = 50;
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE))
						{
							tempInfo->color = COLOR(232, 232, 232, 255);
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE))
						{
							tempInfo->color = COLOR(227, 78, 85, 255);
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF))
						{
							tempInfo->color = COLOR(104, 178, 160, 255);
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
						{
							tempInfo->color = COLOR(255, 255, 36, 255);
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO))
						{
							tempInfo->color = COLOR(99, 0, 132, 255);
						}
						else if (!layerName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
						{
							tempInfo->color = COLOR(236, 194, 191, 255);
						}
					}

					if (m_nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
					{
						tempInfo->bIsUseCredit = true;
					}
				}
			}
		}

		for (int i = 0; i < m_nResultClassCnt; i++)
		{
			int uid = m_vNewMaskInfo[i].uid;

			m_pVolumeData->setBoundingBox(uid, m_vNewMaskBoundingBox[i]);
			m_pVolumeData->setVoxelCount(uid, m_vNewMaskVoxelCnt[i], false);
			m_pVolumeData->setTAState(uid, false);
			WIN_MANAGER->applyVoxelToUI(uid);
		}

		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionPredictAddEx::mergeWith(const QUndoCommand* command)
{
	return false;
}

bool ActionPredictAddEx::GetIndexByAIName(int* pOutIndex, const std::string name)
{
	for (int i = 0; i < m_vecAIName.size(); ++i)
	{
		if (name == m_vecAIName[i])
		{
			*pOutIndex = i;
			return true;
		}
	}
	return false;
}

