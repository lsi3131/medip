#include "stdafx.h"
#include "volumedata.h"
#include "Windows/windowManager.h"
#include "Actions/ActionManager.h"
#include "Windows/Tabwindow.h"
#include "Strsafe.h"

#include "System/stringManager.h"
#include "UI/CustomHistogram.h"
#include "UI/AlphaColorMap.h"

#include "MeshEdit/CManipulator.h"
#include "Windows/Tab/MeshTab.h"

VOLUME_DATA::VOLUME_DATA()
{
	for (int i = 0; i < 4; i++)
	{
		pData3D_Mask[i] = 0;
		if (i != 3)
		{
			pData3D_Mask_Temp_2[i] = 0;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		axialPlaneData[i] = 0;
		coronalPlaneData[i] = 0;
		sagittalPlaneData[i] = 0;
	}

	memset(m_voxelCount, 0, sizeof(m_voxelCount));
	memset(m_seedVoxel, 0, sizeof(m_seedVoxel));

	fillMaskCount = 0;
	pData3D_HU = 0;
	pData3D_HU_Flip = 0;
	dataCX = 0;
	dataCY = 0;
	dataCZ = 0;
	HUMin = HUMax = 0;
	HUMax++;

	for (int i = 0; i < 2; i++)
		seedBoundingbox[i].reset(true);

	for (int i = 0; i < MASK_MAX; i++)
	{
		bLatestTA[i] = false;
		m_boundingBoxROI[i].reset(true);
	}
	threadStop = 0;
	pData3D_HU_Temp = 0;
	pData3D_Mask_Temp = 0;
	pData2D_Mask_Temp_SliceIndex = 0;
	pData2D_Mask_Temp_SliceData = 0;
	pData3D_HU_Temp_D1 = 0;
	pData3D_HU_Temp_D2 = 0;
	m_currentMaskInfoIndex = 0;
	//m_currentMeshInfoIndex = m_currentMaskInfoIndex = 0;
	m_useMaskInfoBit = 0;
	//useMeshInfoBit = 0;
	bRedrawMask = false;

	bUpdateVolume = false;
	bUpdateMPR = false;
	color2DTable.reserve(256);

	axialDepth = -1;
	coronalDepth = -1;
	sagittalDepth = -1;
}

VOLUME_DATA::~VOLUME_DATA()
{
	//	clear(true, false);	//called before program closeevent
}

void VOLUME_DATA::clear(bool delMask, bool delMaskList)
{
	std::vector<muint32>().swap(m_vecMultiSelectedMaskUID);

	for (int i = 0; i < 3; i++)
	{
		SAFE_DELETES(axialPlaneData[i]);
		SAFE_DELETES(coronalPlaneData[i]);
		SAFE_DELETES(sagittalPlaneData[i]);
	}

	SAFE_DELETES(pData3D_HU_Temp_D1);
	SAFE_DELETES(pData3D_HU_Temp_D2);

	SAFE_DELETES(pData3D_HU_Temp);
	SAFE_DELETES(pData3D_Mask_Temp);
	SAFE_DELETES(pData2D_Mask_Temp_SliceIndex);
	SAFE_DELETES(pData2D_Mask_Temp_SliceData);

	for (int i = 0; i < 4; i++)
	{
		SAFE_DELETES(pData3D_Mask[i]);
		if (i != 3)
		{
			SAFE_DELETES(pData3D_Mask_Temp_2[i]);
		}
	}

	SAFE_DELETES(pData3D_HU);
	SAFE_DELETES(pData3D_HU_Flip);

	dataCX = 0;
	dataCY = 0;
	dataCZ = 0;
	m_boundingBox.reset();
	m_boundingBox3D.reset();

	//ClearMeshInfo();

	if (delMask)
	{
		clearMaskInfo();

		for (int i = 0; i < 2; i++)
		{
			seedBoundingbox[i].reset(true);
		}

		for (int i = 0; i < MASK_MAX; i++)
		{
			m_boundingBoxROI[i].reset(true);
			bLatestTA[i] = false;
		}

		m_useMaskInfoBit = 0;

		if (delMaskList)
		{
			if (WIN_MANAGER->GetTab())
			{
				{
					ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();

					/* 21.03.04 이상일 대리 : VolumeData Mask Info가 Clear되어 있기 때문에
						Clear된 Mask로 Update 진행(원복) */
					if (tab)
						tab->ClearROIList();
					//if (tab)
					//	tab->updateROIList();	
				}
				{
					MeshTab* meshTab = WIN_MANAGER->GetTab()->getMeshTab();

					/* 21.03.04 이상일 대리 : VolumeData Mask Info가 Clear되어 있기 때문에
						Clear된 Mask로 Update 진행(원복) */
					if (meshTab)
					{
						meshTab->Clear();
					}

					VisualPrintMeshTab* pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
					if (pVisualPrintMeshTab)
					{
						pVisualPrintMeshTab->Clear();
					}
				}
			}
		}
	}
}

void VOLUME_DATA::clearMaskInfo()
{
	for (auto mask = m_vecMaskInfoList.begin(); mask != m_vecMaskInfoList.end(); ++mask)
	{
		SAFE_DELETE(*mask);
	}
	m_vecMaskInfoList.clear();
	memset(m_voxelCount, 0, sizeof(m_voxelCount));
	memset(m_seedVoxel, 0, sizeof(m_seedVoxel));
}

void VOLUME_DATA::clearTempData()
{
	SAFE_DELETES(pData3D_HU_Temp_D1);
	SAFE_DELETES(pData3D_HU_Temp_D2);

	SAFE_DELETES(pData3D_HU_Temp);
	SAFE_DELETES(pData3D_Mask_Temp);
	SAFE_DELETES(pData2D_Mask_Temp_SliceIndex);
	SAFE_DELETES(pData2D_Mask_Temp_SliceData);

	for (int i = 0; i < 3; i++)
	{
		SAFE_DELETES(pData3D_Mask_Temp_2[i]);
	}
}



void VOLUME_DATA::clearCacheData()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
		dir.removeRecursively();

	dir.mkdir(STRING_MANAGER->cacheFilePath);
}

void VOLUME_DATA::clearOnly3DHU()
{
	SAFE_DELETES(pData3D_HU);
}

void VOLUME_DATA::clearTempMaskSlice()
{
	SAFE_DELETES(pData2D_Mask_Temp_SliceIndex);
	SAFE_DELETES(pData2D_Mask_Temp_SliceData);
}


void VOLUME_DATA::forceUpdateVolume()
{
	bUpdateVolume = true;
	WIN_MANAGER->forceUpdate2DViewData(true, false);
}

void VOLUME_DATA::forceUpdateMaskVolume()
{
	bRedrawMask = true;

	WIN_MANAGER->forceUpdate2DViewData(false, true);
}

QColor VOLUME_DATA::get2Dcolor(int index)
{
	if (index < 0)
		index = 0;

	if (index >= color2DTable.size())
	{
		if (index > 255)
			index = 255;

		return QColor(index, index, index, 255);
	}

	return color2DTable.at(index);
}

void VOLUME_DATA::updateVoxelCount(int uid, mask maskBitFlag, int maskByteIndex)
{
	int minX = m_boundingBoxROI[uid].minX;
	int minY = m_boundingBoxROI[uid].minY;
	int minZ = m_boundingBoxROI[uid].minZ;

	int maxX = m_boundingBoxROI[uid].maxX;
	int maxY = m_boundingBoxROI[uid].maxY;
	int maxZ = m_boundingBoxROI[uid].maxZ;

	int count = 0;
	for (int x = minX; x <= maxX; ++x)
	{
		for (int y = minY; y <= maxY; ++y)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (isMaskBit(x, y, z, maskBitFlag, maskByteIndex))
				{
					count++;
				}
			}
		}
	}
	setVoxelCount(uid, count);
}

void VOLUME_DATA::setVoxelCount(int uid, int count, bool total, bool seed)
{
	if (seed)
	{
		if (uid < 0 || uid > 1)
			return;

		if (total)
			m_seedVoxel[uid] = count;
		else
			m_seedVoxel[uid] += count;
	}
	else
	{
		if (uid < 0 || uid >= MASK_MAX)
			return;

		if (total)
			m_voxelCount[uid] = count;
		else
			m_voxelCount[uid] += count;

		if (m_voxelCount[uid] < 0)
			m_voxelCount[uid] = 0;
	}
}

int VOLUME_DATA::getVoxelCount(int uid, bool seed)
{
	// 단위 mm
	if (seed)
	{
		if (uid < 0 || uid > 1)
		{
			return 0;
		}

		return m_seedVoxel[uid];
	}
	else
	{
		if (uid < 0 || uid >= MASK_MAX)
		{
			return 0;
		}

		return m_voxelCount[uid];
	}
}

float VOLUME_DATA::getBurdenVal(int uid)
{
	return m_dBrudenValue[uid];
}

void VOLUME_DATA::setBurdenVal(int uid, int value)
{
	if (uid < 0 || uid >= MASK_MAX)
	{
		return;
	}

	m_dBrudenValue[uid] = value;
}

void VOLUME_DATA::setBoundingBox(int uid, BoundingBoxI box, bool reset, bool seed)
{
	if (seed)
	{
		if (uid < 0 || uid > 1)
		{
			return;
		}

		if (reset)
			seedBoundingbox[uid].reset(true);
		else
			seedBoundingbox[uid] = box;
	}
	else
	{
		if (uid < 0 || uid >= MASK_MAX)
		{
			return;
		}

		if (reset)
			m_boundingBoxROI[uid].reset(true);
		else
			m_boundingBoxROI[uid] = box;
	}
}

BoundingBoxI VOLUME_DATA::getBoundingBox(int uid, bool seed)
{
	if (seed)
	{
		if (uid < 0 || uid > 1)
		{
			return BoundingBoxI();
		}

		return seedBoundingbox[uid];
	}
	else
	{
		if (uid < 0 || uid >= MASK_MAX)
		{
			return BoundingBoxI();
		}

		return m_boundingBoxROI[uid];
	}
}

void VOLUME_DATA::setTAState(int uid, bool val, bool _drawcut)
{
	if (_drawcut)
	{
		return;
	}

	if (uid < 0 || uid >= MASK_MAX)
	{
		return;
	}

	if (bLatestTA[uid] != val)
	{
		bLatestTA[uid] = val;
		WIN_MANAGER->updateTAState(!bLatestTA[uid], uid);
	}
}

bool VOLUME_DATA::getTAState(int uid, bool _drawcut)
{
	if (_drawcut)
	{
		return false;
	}

	if (uid < 0 || uid >= MASK_MAX)
	{
		return false;
	}

	return bLatestTA[uid];
}

int VOLUME_DATA::GetMaskPointCount()
{
	int count = 0;

	for (; count < 4; count++)
	{
		if (!CheckDataPoint(count, true))
		{
			break;
		}
	}

	return count;
}

mip::VECTOR3 VOLUME_DATA::toWorldPosition(mip::VECTOR3& pos)
{
	mip::VECTOR3 real(
		pos.x * getSpaceX(true),
		pos.y * getSpaceY(true),
		pos.z * getSpaceZ(true)
	);

	return getImgPos() + real;
}

bool VOLUME_DATA::IsAIMaskByIndex(int index, int* outValue)
{
	MaskInfo* info = getMaskInfoByIndex(index);
	if (info == nullptr)
	{
		return false;
	}

	for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
	{
		if (it->first == info->uid)
		{
			if (outValue)
			{
				*outValue = it->second;
			}
			return true;
		}
	}

	return false;
}

bool VOLUME_DATA::IsAIMaskByUID(int UID, int* outValue)
{
	MaskInfo* info = getMaskInfoByUID(UID);
	if (info == nullptr)
	{
		return false;
	}

	for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
	{
		if (it->first == info->uid)
		{
			if (outValue)
			{
				*outValue = it->second;
			}
			return true;
		}
	}

	return false;
}

bool VOLUME_DATA::FindAIDataIndexByUID(int* pOutIndex, int UID)
{
	for (int i = 0; i < m_vecAIOutset.size(); ++i)
	{
		if (m_vecAIOutset[i].first == UID)
		{
			*pOutIndex = i;
			return true;
		}
	}
	return false;
}

void VOLUME_DATA::DeleteAIDataByUID(int uid)
{
	for (auto it = m_vecAIResultData.begin(); it != m_vecAIResultData.end(); ++it)
	{
		if (it->first == uid)
		{
			m_vecAIResultData.erase(it);
			break;
		}
	}

	for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
	{
		if (it->first == uid)
		{
			m_vecAIOutset.erase(it);
			break;
		}
	}
}

bool VOLUME_DATA::SetAIDataByUID(int uid, const std::vector<unsigned char>& AIResult, int AIOutset)
{
	MaskInfo* pInfo = getMaskInfoByUID(uid);
	if (pInfo == nullptr)
	{
		return false;
	}

	if (AIResult.empty())
	{
		return false;
	}

	int index = 0;
	if (FindAIDataIndexByUID(&index, uid))
	{
		m_vecAIResultData[index].second = AIResult;
		m_vecAIOutset[index].second = AIOutset;
	}
	else
	{
		m_vecAIResultData.push_back(std::make_pair(uid, AIResult));
		m_vecAIOutset.push_back(std::make_pair(uid, AIOutset));
	}

	return true;
}

QVector<mask> VOLUME_DATA::GetMaskBitFlagList(int maskByteIndex)
{
	if (maskByteIndex >= 4 || maskByteIndex < 0)
	{
		return QVector<mask>();
	}

	return m_maskIDList[maskByteIndex];
}

mask* VOLUME_DATA::GetMaskDataPtr(int maskByteIndex)
{
	if (maskByteIndex >= 4 || maskByteIndex < 0)
	{
		return nullptr;
	}

	return pData3D_Mask[maskByteIndex];
}

muint32 VOLUME_DATA::GetUsedMaskUIDBitArray()
{
	return m_useMaskInfoBit;
}

bool VOLUME_DATA::IsMaskUIDUsed(int uid)
{
	return ((m_useMaskInfoBit & (1 << uid)) > 0);
}

bool VOLUME_DATA::setHUData(mint16* data, muint32 cx, muint32 cy, muint32 cz)
{
	if (data == nullptr)
	{
		return false;
	}

	if (getCX() != cx)
	{
		return false;
	}

	if (getCY() != cy)
	{
		return false;
	}

	if (getCZ() != cz)
	{
		return false;
	}

	memcpy(pData3D_HU, data, sizeof(mint16) * dataLenth);
	return true;
}

bool VOLUME_DATA::createData(mint16* data, muint32 cx, muint32 cy, muint32 cz, float x, float y, float z, bool delMask)
{
	bool r = createData(cx, cy, cz, x, y, z, delMask);

	if (r == false)
	{
		return false;
	}

	memcpy(pData3D_HU, data, sizeof(mint16) * dataLenth);

	updateVolumeMinMax();

	updateMPRData();

	return true;
}

bool VOLUME_DATA::createData(muint32 cx, muint32 cy, muint32 cz, float x, float y, float z, bool delMask, bool delMaskList)
{
	if (cx == 0 && cy == 0 && cz == 0)
	{
		return false;
	}

	clear(delMask, delMaskList);

	dataCX = cx;
	dataCY = cy;
	dataCZ = cz;
	dataLenth = dataCX * dataCY * dataCZ;

	setSpace(x, y, z);

	try
	{
		pData3D_HU = new mint16[dataLenth];
		pData3D_Mask[0] = new mask[dataLenth];
	}
	catch (...)
	{
		SAFE_DELETES(pData3D_HU);
		SAFE_DELETES(pData3D_Mask[0]);
		return false;
	}

	for (int i = 0; i < 3; i++)
	{
		axialPlaneData[i] = new mint16[dataCX * dataCY];
		memset(axialPlaneData[i], 0, sizeof(mint16) * dataCX * dataCY);
		coronalPlaneData[i] = new mint16[dataCX * dataCZ];
		memset(coronalPlaneData[i], 0, sizeof(mint16) * dataCX * dataCZ);
		sagittalPlaneData[i] = new mint16[dataCY * dataCZ];
		memset(sagittalPlaneData[i], 0, sizeof(mint16) * dataCY * dataCZ);
	}

	m_bCreate = true;
	bRedrawMask = false;
	memset(pData3D_HU, 0, dataLenth * sizeof(mint16));
	memset(pData3D_Mask[0], 0, dataLenth);

	if (delMask)
	{
		for (int i = 0; i < MASK_MAX; i++)
		{
			m_boundingBoxROI[i].reset(true);
			bLatestTA[i] = false;
		}
	}
	m_boundingBox.reset(dataCX, dataCY, dataCZ);
	m_boundingBox3D.reset(dataCX, dataCY, dataCZ);

	init3DPlanes();
	WIN_MANAGER->volume_renderer.initVolumeSizeInfo(spaceX, spaceY, spaceZ, dataCX, dataCY, dataCZ);

	AIKernelConversionTab* pKernelConversiontab = nullptr;
	AILowdoseCTReconstuctionTab* pLowdoseCTReconTab = nullptr;

	if (WIN_MANAGER->GetTab())
	{
		pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
	}

	if (pKernelConversiontab)
	{
		pKernelConversiontab->SetVisibleInterpolation(false);
	}

	if (pLowdoseCTReconTab)
	{
		pLowdoseCTReconTab->SetVisibleInterpolation(false);
	}

	return true;
}

bool VOLUME_DATA::create3DHUData(muint32 cx, muint32 cy, muint32 cz, float pixelSpacingX, float pixelSpacingY, float pixelSpacingZ)
{
	/* TODO : 다른 create함수에서 OR 연산으로 범위 체크 변경할 것 */
	if (cx == 0 || cy == 0 || cz == 0)
	{
		return false;
	}

	dataCX = cx;
	dataCY = cy;
	dataCZ = cz;
	dataLenth = dataCX * dataCY * dataCZ;

	setSpace(pixelSpacingX, pixelSpacingY, pixelSpacingZ);

	try
	{
		pData3D_HU = new mint16[dataLenth];
	}
	catch (...)
	{
		SAFE_DELETES(pData3D_HU);
		return false;
	}

	m_bCreate = true;
	memset(pData3D_HU, 0, dataLenth * sizeof(mint16));

	m_boundingBox.reset(dataCX, dataCY, dataCZ);
	m_boundingBox3D.reset(dataCX, dataCY, dataCZ);

	return true;
}

bool VOLUME_DATA::createMaskData(mask* data, int mI)
{
	SAFE_DELETES(pData3D_Mask[mI]);

	try
	{
		pData3D_Mask[mI] = new mask[dataLenth];
	}
	catch (...)
	{
		SAFE_DELETES(pData3D_Mask[mI]);
		return false;
	}

	memcpy(pData3D_Mask[mI], data, dataLenth);
	return true;
}

void VOLUME_DATA::createTestTexture()//not used
{
	int height = getCY();
	int width = getCX();
	int cnt = getCZ();
	int index = 0;
	bool check;
	int value = 0;

	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				index = (z)*width * height + y * width + x;
				value = (x % 2 + y % 2) % 2;
				if (z % 2 == 0)
					check = value < 1;
				else
					check = value >= 1;

				if (check == false)
					pData3D_HU[index] = -5000;
				else
					pData3D_HU[index] = 100;
			}
		}
	}
}

void VOLUME_DATA::updateMPRData()
{
	for (int y = 0; y < getCY(); y++)
		for (int x = 0; x < getCX(); x++)
			axialPlaneData[1][y * getCX() + x] = getData(x, y, getCZ() / 2);

	for (int y = 0; y < getCZ(); y++)
		for (int x = 0; x < getCX(); x++)
			coronalPlaneData[1][y * getCX() + x] = getData(x, getCY() / 2, y);

	for (int y = 0; y < getCZ(); y++)
		for (int x = 0; x < getCY(); x++)
			sagittalPlaneData[1][y * getCY() + x] = getData(getCX() / 2, x, y);

	forceUpdateMPR();
}

void VOLUME_DATA::updateColorTable()
{
	int cusIndex = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
	SLICE_PRESET index = WIN_MANAGER->getSelectedPreset();

	color2DTable = AlphaColorMap::getColorTable(index, CL_2D, cusIndex);
}


bool VOLUME_DATA::connectCheckAll()
{
	if (!isValidate())
	{
		return false;
	}

	bool res = true;

	for (int i = 0; i < m_vecMaskInfoList.size(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList.at(i);

		if (info)
			res = info->meshConnected;

		if (!res)
			break;
	}

	return res;
}

//check : roi list update, roi del, roi create, roi column click
/*@function visibleCheckAll
 *@brief ROI Header에 대한 visible all check (layer/surface)
 *@param type false:layer(default), true:surface
 *@return 다 show 상태이면 true, 아니면 false
*/
bool VOLUME_DATA::visibleCheckAll(bool type)
{
	if (!isValidate())
		return false;

	bool res = true;

	if (!type)
	{
		for (int i = 0; i < m_vecMaskInfoList.size(); i++)
		{
			MaskInfo* info = m_vecMaskInfoList.at(i);

			if (info)
				res = info->show;

			if (!res)
				break;
		}
	}

	return res;
}

void VOLUME_DATA::updateVolumeMinMax()
{
	if (pData3D_HU == NULL)
	{
		return;
	}

	mint16 min = INT16_MAX;
	mint16 max = INT16_MIN;

	for (muint32 n = 0; n < dataLenth; n++)
	{
		if (pData3D_HU[n] > max)
		{
			max = pData3D_HU[n];
		}

		if (pData3D_HU[n] < min)
		{
			min = pData3D_HU[n];
		}
	}

	HUMin = min;
	HUMax = max;
}


void VOLUME_DATA::addMaskBitFlagToList(MaskInfo* pInfo)
{
	if (pInfo->uid >= MASK_SECOND_MAX)
	{
		int maskByteIndex = GetMaskByteIndex(pInfo);
		m_maskIDList[maskByteIndex].append(pInfo->mask_id2);
	}
	else
	{
		m_maskIDList[0].append(pInfo->mask_id);
	}
}

bool VOLUME_DATA::createNewMaskInfoAndInitData(const QString& layerName, bool isCopy, bool b_update_ui)
{
	MaskInfo* pNewMaskInfo = new MaskInfo;
	int maskIndex = 0;

	m_vecMaskInfoList.push_back(pNewMaskInfo);
	pNewMaskInfo->uid = indexGenForMask();
	pNewMaskInfo->color = colorGen(pNewMaskInfo->uid);
	if (m_vecMaskInfoList.size() <= MASK_SECOND_MAX)
	{
		pNewMaskInfo->mask_id = VM_MASK0 << pNewMaskInfo->uid + 2;
		pNewMaskInfo->mask_id2 = 0;
		if (!m_maskIDList[maskIndex].contains(pNewMaskInfo->mask_id))
			m_maskIDList[maskIndex].append(pNewMaskInfo->mask_id);
	}
	else
	{
		maskIndex = 1 + (pNewMaskInfo->uid - MASK_SECOND_MAX) / 8;
		pNewMaskInfo->mask_id = VM_MASK0 << MASK_SECOND_MAX + 1;
		pNewMaskInfo->mask_id2 = VM_MASK0 << ((pNewMaskInfo->uid - (MASK_SECOND_MAX)) % 8);

		if (!m_maskIDList[maskIndex].contains(pNewMaskInfo->mask_id2))
			m_maskIDList[maskIndex].append(pNewMaskInfo->mask_id2);
	}
	pNewMaskInfo->show = true;
	pNewMaskInfo->meshConnected = true;
	if (!isCopy)
	{
		setMaskName(layerName, pNewMaskInfo->uid, true);
	}

	setUsedMaskUIDBitOn(pNewMaskInfo->uid);
	//m_useMaskInfoBit |= (1 << pNewMaskInfo->uid);

	clearMaskData(maskIndex == 0 ? pNewMaskInfo->mask_id : pNewMaskInfo->mask_id2, maskIndex);
	pNewMaskInfo->layerAlpha = 255;
	m_voxelCount[pNewMaskInfo->uid] = 0;
	m_boundingBoxROI[pNewMaskInfo->uid].reset(true);

	if (b_update_ui)
	{
		WIN_MANAGER->applyVoxelToUI(pNewMaskInfo->uid);
	}

	return true;
}

bool VOLUME_DATA::insertNewMaskInfoAndInitData(int index, const QString& layerName)
{
	if (index < 0 || index > m_vecMaskInfoList.size())
	{
		return false;
	}

	MaskInfo* pNewMaskInfo = new MaskInfo;
	int maskByteIndex = 0;

	m_vecMaskInfoList.insert(m_vecMaskInfoList.begin() + index, pNewMaskInfo);

	pNewMaskInfo->uid = indexGenForMask();
	pNewMaskInfo->color = colorGen(pNewMaskInfo->uid);
	if (m_vecMaskInfoList.size() <= MASK_SECOND_MAX)
	{
		pNewMaskInfo->mask_id = VM_MASK0 << pNewMaskInfo->uid + 2;
		pNewMaskInfo->mask_id2 = 0;
		if (!m_maskIDList[maskByteIndex].contains(pNewMaskInfo->mask_id))
		{
			m_maskIDList[maskByteIndex].append(pNewMaskInfo->mask_id);
		}
	}
	else
	{
		maskByteIndex = 1 + (pNewMaskInfo->uid - MASK_SECOND_MAX) / 8;
		pNewMaskInfo->mask_id = VM_MASK0 << MASK_SECOND_MAX + 1;
		pNewMaskInfo->mask_id2 = VM_MASK0 << ((pNewMaskInfo->uid - (MASK_SECOND_MAX)) % 8);

		if (!m_maskIDList[maskByteIndex].contains(pNewMaskInfo->mask_id2))
		{
			m_maskIDList[maskByteIndex].append(pNewMaskInfo->mask_id2);
		}
	}
	pNewMaskInfo->show = true;
	pNewMaskInfo->meshConnected = true;

	setMaskName(layerName, pNewMaskInfo->uid, true);

	setUsedMaskUIDBitOn(pNewMaskInfo->uid);
	//m_useMaskInfoBit |= (1 << pNewMaskInfo->uid);

	clearMaskDataByInfo(pNewMaskInfo);

	pNewMaskInfo->layerAlpha = 255;
	m_voxelCount[pNewMaskInfo->uid] = 0;
	m_boundingBoxROI[pNewMaskInfo->uid].reset(true);

	return true;
}

bool VOLUME_DATA::CheckDataPoint(int Maskindex, bool CheckOnly)
{
	bool exist = true;

	if (Maskindex != 0 && (!pData3D_Mask[Maskindex]))
	{
		exist = false;
		if (CheckOnly)
			return exist;

		pData3D_Mask[Maskindex] = new mask[dataLenth];

		memset(pData3D_Mask[Maskindex], 0, dataLenth);
	}

	return exist;
}

void VOLUME_DATA::changeUID(mint32 pre_mask_uid, mint32 mask_uid)
{
#ifdef SEGMENTATION_COLOR_CODING
	for (auto it = m_vecAIResultData.begin(); it != m_vecAIResultData.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}

	for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}
#endif
	//ChangeLUID(pre_mask_uid, mask_uid);
}

void VOLUME_DATA::deleteMaskBitFlagFromList(int maskByteIndex, mask maskBitFlag)
{
	for (auto it = m_maskIDList[maskByteIndex].begin(); it != m_maskIDList[maskByteIndex].end(); ++it)
	{
		if (*it == maskBitFlag)
		{
			m_maskIDList[maskByteIndex].erase(it);
			break;
		}
	}
}

void VOLUME_DATA::setUsedMaskUIDBitOn(int uid)
{
	m_useMaskInfoBit |= (1 << uid);
}

void VOLUME_DATA::setUsedMaskUIDBitOff(int uid)
{
	m_useMaskInfoBit &= ~(1 << uid);
}

void VOLUME_DATA::clearVoxelCountAndBoundingBox(int uid)
{
	m_voxelCount[uid] = 0;
	m_boundingBoxROI[uid].reset(true);
}

int VOLUME_DATA::GetMaskByteIndex(MaskInfo* pInfo)
{
	return pInfo->uid >= MASK_SECOND_MAX ? (pInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
}

mask VOLUME_DATA::GetMaskBitFlag(MaskInfo* pInfo)
{
	int maskByteIdx = GetMaskByteIndex(pInfo);
	mask maskBitFlag = (maskByteIdx == 0) ? pInfo->mask_id : pInfo->mask_id2;
	return maskBitFlag;
}

std::vector<unsigned char> VOLUME_DATA::GetAIResultByUID(int uid)
{
	for (auto it = m_vecAIResultData.begin(); it != m_vecAIResultData.end(); ++it)
	{
		if (it->first == uid)
		{
			return it->second;
		}
	}

	return std::vector<unsigned char>();
}

int VOLUME_DATA::GetAIOutsetByUID(int uid)
{
	for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
	{
		if (it->first == uid)
		{
			return it->second;
		}
	}

	return -1;
}

mip::VECTOR3 VOLUME_DATA::GetOffsetCenter()
{
	mip::VECTOR3 offsetCenter(-getSizeX() * 0.5f, -getSizeY() * 0.5f, -getSizeZ() * 0.5f);
	return offsetCenter;
}

void VOLUME_DATA::eraseMaskInfoByIndexAndSetCurrentIndex(int index)
{
	SAFE_DELETE(m_vecMaskInfoList[index]);
	m_vecMaskInfoList.erase(m_vecMaskInfoList.begin() + index);

	if (index >= m_vecMaskInfoList.size())
	{
		setCurrentMaskIndex(m_vecMaskInfoList.size() - 1);
	}
	else
	{
		setCurrentMaskIndex(index);
	}
}

void VOLUME_DATA::applyTempMask(int Maskindex)
{
	CheckDataPoint(Maskindex);

	memcpy(pData3D_Mask[Maskindex], pData3D_Mask_Temp, getVolumeDataLength());
	SAFE_DELETES(pData3D_Mask_Temp);
}

void VOLUME_DATA::applyTempMaskBitAdd(mask _m, int Maskindex)
{
	CheckDataPoint(Maskindex);

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;
	for (int n = 0; n < length; ++n)
		if ((!(pData3D_Mask[Maskindex][n] & _m)) && (pData3D_Mask_Temp[n] & _m))
		{
			fillMaskCount++;
			pData3D_Mask[Maskindex][n] |= _m;
		}

	SAFE_DELETES(pData3D_Mask_Temp);
}

int VOLUME_DATA::SplitTempMaskBit(int _uid, mask _m, int _mI)
{
	CheckDataPoint(_mI);

	mint32 length = getVolumeDataLength();

	uint cx, cy;

	cx = getCX();
	cy = getCY();

	fillMaskCount = 0;

	if (_uid < 0 || _uid >= MASK_MAX)
		return -1;

	BoundingBoxI box = m_boundingBoxROI[_uid];
	int nStart = box.minZ * cx * cy + box.minY * cx + box.minX;
	int nEnd = (box.maxZ) * cx * cy + (box.maxY) * cx + box.maxX;

	if ((nStart < 0 || nStart >= length) || (nEnd < 0 || nEnd > length))
		return -1;

	//mask mVal = VM_MASK0;

	QMap<mask, mask> Mapping;
	QMap<mask, mask>::iterator iter;
	Mapping.clear();

	bool res = createTempMaskData(true, -1, 1);

	if (res)
	{
		for (; nStart <= nEnd; nStart++)
		{
			if ((pData3D_Mask[_mI][nStart] & _m))
			{
				mask Val;
				if (Mapping.isEmpty() || (Mapping.end() == (iter = Mapping.find(pData3D_Mask_Temp[nStart]))))
				{
					//Val = mVal;
					Val = pData3D_Mask_Temp[nStart];
					Mapping.insert(pData3D_Mask_Temp[nStart], Val);
					//mVal <<= 1;
				}
				else
					Val = iter.value();

				pData3D_Mask_Temp_2[0][nStart] = Val;
			}
		}

		memcpy_s(pData3D_Mask_Temp, length, pData3D_Mask_Temp_2[0], length);

		SAFE_DELETES(pData3D_Mask_Temp_2[0]);
	}

	return Mapping.count(); //split count
}



void VOLUME_DATA::applyTempMaskSlice(int Maskindex, muint32 slicesize)
{
	if (pData2D_Mask_Temp_SliceData == 0 || pData2D_Mask_Temp_SliceIndex == 0) return;

	CheckDataPoint(Maskindex);

	mask* mask_data = pData3D_Mask[Maskindex];
	muint32 index = 0;

	for (int n = 0; n < slicesize; n++)
	{
		index = pData2D_Mask_Temp_SliceIndex[n];

		if (index == 0) continue;
		if (index >= getVolumeDataLength()) continue;

		mask_data[index] = pData2D_Mask_Temp_SliceData[n];
	}
}

void VOLUME_DATA::applyTempMaskSliceAdd(WINDOW_TYPE type, mask _m, int Maskindex, muint32 slicesize)
{
	if (pData2D_Mask_Temp_SliceData == 0 || pData2D_Mask_Temp_SliceIndex == 0) return;

	CheckDataPoint(Maskindex);

	fillMaskCount = 0;
	muint32 incex = 0;
	mask* mask_data = pData3D_Mask[Maskindex];

	for (int n = 0; n < slicesize; n++)
	{
		incex = pData2D_Mask_Temp_SliceIndex[n];

		if (incex == 0) continue;
		if (incex >= getVolumeDataLength()) continue;

		if (!(mask_data[incex] & _m))
		{
			fillMaskCount++;
			mask_data[incex] |= _m;
		}

		pData2D_Mask_Temp_SliceData[n] = mask_data[incex];
	}
}


void VOLUME_DATA::applyTempMaskSliceDel(WINDOW_TYPE type, mask _m, int Maskindex, muint32 slicesize)
{
	if (pData2D_Mask_Temp_SliceData == 0 || pData2D_Mask_Temp_SliceIndex == 0) return;

	CheckDataPoint(Maskindex);

	fillMaskCount = 0;
	muint32 index = 0;
	mask* mask_data = pData3D_Mask[Maskindex];

	for (int n = 0; n < slicesize; n++)
	{
		index = pData2D_Mask_Temp_SliceIndex[n];

		if (index == 0) continue;
		if (index >= getVolumeDataLength()) continue;

		if ((mask_data[index] & _m))
		{
			fillMaskCount--;
			mask_data[index] &= ~_m;
		}

		pData2D_Mask_Temp_SliceData[n] = mask_data[index];
	}
}

void VOLUME_DATA::applyTempMaskBitChange(mask _m, int Maskindex, mask _chm, int tempMaskIndex, bool _del)
{
	CheckDataPoint(Maskindex);

	if (0 == _chm) _chm = _m;

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;

	if (tempMaskIndex == 0)
	{
		for (int n = 0; n < length; ++n)
		{
			if (pData3D_Mask_Temp[n] & _chm)
			{
				if (!(pData3D_Mask[Maskindex][n] & _m))
				{
					fillMaskCount++;
					pData3D_Mask[Maskindex][n] |= _m;
				}
			}
			else
			{
				if ((pData3D_Mask[Maskindex][n] & _m))
				{
					fillMaskCount--;
					pData3D_Mask[Maskindex][n] &= ~_m;
				}
			}
		}
		if (_del)
			SAFE_DELETES(pData3D_Mask_Temp);
	}
	else
	{
		for (int n = 0; n < length; ++n)
		{
			if (pData3D_Mask_Temp_2[tempMaskIndex - 1][n] & _chm)
			{
				if (!(pData3D_Mask[Maskindex][n] & _m))
				{
					fillMaskCount++;
					pData3D_Mask[Maskindex][n] |= _m;
				}

			}
			else
			{
				if ((pData3D_Mask[Maskindex][n] & _m))
				{
					fillMaskCount--;
					pData3D_Mask[Maskindex][n] &= ~_m;
				}
			}
		}
		if (_del)
			SAFE_DELETES(pData3D_Mask_Temp_2[tempMaskIndex - 1]);
	}
}

int VOLUME_DATA::applyTempMaskBitOutset(const std::vector<unsigned char>& vClassData, mask maskBit, int nMaskByteIdx, unsigned char nThreshold)
{
	CheckDataPoint(nMaskByteIdx);

	//if (NULL == pData3D_Mask_Temp) return;
	if (vClassData.empty())
		return 0;

	const int len = getVolumeDataLength();

	int nMaskCount = 0;

	for (int i = 0; i < len; i++)
	{
		//  pData3D_Mask_Temp: mask bit 데이터 아님, 0~255 이미지 데이터
		//if (vClassData[i] > nThreshold || vClassData[i] == 255)
		if (vClassData[i] >= nThreshold)
		{
			//if (!(pData3D_Mask[nMaskByteIdx][i] & maskBit))
			{
				nMaskCount++;
				pData3D_Mask[nMaskByteIdx][i] |= maskBit;
			}
		}
		else
		{
			//if ((pData3D_Mask[nMaskByteIdx][i] & maskBit))
			{
				//nMaskCount--;
				pData3D_Mask[nMaskByteIdx][i] &= ~maskBit;
			}
		}
	}
	return nMaskCount;

	// 	if (del)
	// 		SAFE_DELETES(pData3D_Mask_Temp);

		/////////////////////////////////////////////
	// 	CheckDataPoint(_mI);
	// 
	// 	if (NULL == pData3D_Mask_Temp) return;
	// 
	// 	const int len = getLength();
	// 
	// 	fillMaskCount = 0;
	// 
	// 	for (int i = 0; i < len; i++)
	// 	{
	// 		//  pData3D_Mask_Temp: mask bit 데이터 아님, 0~255 이미지 데이터
	// 		if (pData3D_Mask_Temp[i] >= outset)
	// 		{
	// 			if (!(pData3D_Mask[_mI][i] & _m))
	// 			{
	// 				fillMaskCount++;
	// 				pData3D_Mask[_mI][i] |= _m;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			if ((pData3D_Mask[_mI][i] & _m))
	// 			{
	// 				fillMaskCount--;
	// 				pData3D_Mask[_mI][i] &= ~_m;
	// 			}
	// 		}
	// 	}
	// 
	// 	if (del)
	// 		SAFE_DELETES(pData3D_Mask_Temp);

}

void VOLUME_DATA::applyTempMaskBitChangeLoadROI(mask _m, QString& filename, int Maskindex /*= 0*/)
{
	int height = getCY();
	int width = getCX();
	int cnt = getCZ();

	unsigned char* mask_buffer = new unsigned char[cnt * height * width];

	bool r = WIN_MANAGER->readROIFile(mask_buffer, filename);

	if (r)
	{
		mask _mask = 0;

		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
#if 1
					_mask = mask_buffer[z * width * height + (y)*width + (x)];
					if (_mask != 0)
					{
						pData3D_Mask_Temp[z * width * height + y * width + x] = _m;
					}
#else
					_mask = mask_buffer[(z)*width * height + (reverse ? height - y - 1 : y) * width + (x)];
					if (_mask != 0)
					{
						pData3D_Mask_Temp[z * width * height + y * width + x] = _m;
					}
#endif
				}
			}
		}

		CheckDataPoint(Maskindex);

		mint32 length = getVolumeDataLength();
		fillMaskCount = 0;
		for (int n = 0; n < length; ++n)
		{
			pData3D_Mask[Maskindex][n] &= ~_m;
			if (pData3D_Mask_Temp[n] & _m)
			{
				pData3D_Mask[Maskindex][n] |= _m;
				fillMaskCount++;
			}
		}
	}

	SAFE_DELETES(mask_buffer);
	SAFE_DELETES(pData3D_Mask_Temp);
}

void VOLUME_DATA::applyTempMaskBitChangeLoadTXT(unsigned char* mask_buffer, mask _m, QString& filename, int Maskindex)
{
	int height = getCY();
	int width = getCX();
	int cnt = getCZ();

	mask _mask = 0;

	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				_mask = mask_buffer[z * width * height + (y)*width + (x)];
				if (_mask != 0)
				{
					pData3D_Mask_Temp[z * width * height + y * width + x] = _m;
				}

			}
		}
	}

	CheckDataPoint(Maskindex);

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;
	for (int n = 0; n < length; ++n)
	{
		pData3D_Mask[Maskindex][n] &= ~_m;
		if (pData3D_Mask_Temp[n] & _m)
		{
			pData3D_Mask[Maskindex][n] |= _m;
			fillMaskCount++;
		}
	}

	SAFE_DELETES(mask_buffer);
	SAFE_DELETES(pData3D_Mask_Temp);
}

void VOLUME_DATA::applyTempMaskBitDel(mask _m, int Maskindex, mask _chm, int tempMaskIndex, bool _del)
{
	CheckDataPoint(Maskindex);

	if (0 == _chm) _chm = _m;

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;

	if (tempMaskIndex == 0)
	{
		for (int n = 0; n < length; ++n)
		{
			if ((pData3D_Mask[Maskindex][n] & _m) && pData3D_Mask_Temp[n] & _m)
			{
				pData3D_Mask[Maskindex][n] &= ~_m;
				fillMaskCount--;
			}
		}
		if (_del)
			SAFE_DELETES(pData3D_Mask_Temp);
	}
	else
	{
		for (int n = 0; n < length; ++n)
		{
			if ((pData3D_Mask[Maskindex][n] & _m) && pData3D_Mask_Temp_2[tempMaskIndex - 1][n] & _chm)
			{
				pData3D_Mask[Maskindex][n] &= ~_m;
				fillMaskCount--;

			}
		}
		if (_del)
			SAFE_DELETES(pData3D_Mask_Temp_2[tempMaskIndex - 1]);
	}

}

bool VOLUME_DATA::createTempMaskData(bool init, int newdatalen, int mI)
{
	if (0 == mI)
	{
		SAFE_DELETES(pData3D_Mask_Temp);
		int len = newdatalen <= 0 ? dataLenth : newdatalen;

		try
		{
			pData3D_Mask_Temp = new unsigned char[len];
		}
		catch (...)
		{
			SAFE_DELETES(pData3D_Mask_Temp);
			return false;
		}
		if (init) memset(pData3D_Mask_Temp, 0, len);
	}
	else
	{
		SAFE_DELETES(pData3D_Mask_Temp_2[mI - 1]);
		int len = newdatalen <= 0 ? dataLenth : newdatalen;
		try
		{
			pData3D_Mask_Temp_2[mI - 1] = new unsigned char[len];
		}
		catch (...)
		{
			SAFE_DELETES(pData3D_Mask_Temp_2[mI - 1]);
			return false;
		}

		if (init) memset(pData3D_Mask_Temp_2[mI - 1], 0, len);
	}

	return true;
}

muint32 VOLUME_DATA::createTempMaskSlice(WINDOW_TYPE type)
{
	clearTempMaskSlice();

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	getLengthForScreen(type, c_x, c_y, c_depth);

	if (c_x <= 0 || c_y <= 0) return 0;

	pData2D_Mask_Temp_SliceIndex = new muint32[c_x * c_y];
	pData2D_Mask_Temp_SliceData = new mask[c_x * c_y];

	memset(pData2D_Mask_Temp_SliceIndex, 0, sizeof(muint32) * c_x * c_y);
	memset(pData2D_Mask_Temp_SliceData, 0, c_x * c_y);

	return c_x * c_y;
}

void VOLUME_DATA::setTempMaskBitFromData(mask _srcm, int _srcmI, mask _dstm, int _dstmI)
{
	CheckDataPoint(_srcmI);

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;
	mask* dstTemp = getMaskTempDataPoint(_dstmI);

	for (int n = 0; n < length; ++n)
	{
		dstTemp[n] &= ~_srcm;
		if (pData3D_Mask[_srcmI][n] & _srcm)
		{
			dstTemp[n] |= _dstm;
		}
	}
}

mint16* VOLUME_DATA::getHUFlipDataPoint()
{
	if (!pData3D_HU_Flip)
		pData3D_HU_Flip = new mint16[dataLenth];

	return pData3D_HU_Flip;
}

mask* VOLUME_DATA::getMaskDataPoint(int n3DMaskByteIdx/*= 0*/)
{
	CheckDataPoint(n3DMaskByteIdx);

	return pData3D_Mask[n3DMaskByteIdx];
}

mask** VOLUME_DATA::getAllMaskDataPoint(void)
{
	if (pData3D_Mask)
		return pData3D_Mask;
	return nullptr;
}

mask* VOLUME_DATA::getMaskTempDataPoint(int _mI)
{
	if (0 == _mI)
		return pData3D_Mask_Temp;
	else
		return pData3D_Mask_Temp_2[_mI - 1];
}

bool VOLUME_DATA::createTempHUData2(bool init)
{
	SAFE_DELETES(pData3D_HU_Temp_D1);
	SAFE_DELETES(pData3D_HU_Temp_D2);

	try
	{
		pData3D_HU_Temp_D1 = new double[dataLenth];
		pData3D_HU_Temp_D2 = new double[dataLenth];
	}
	catch (...)
	{
		SAFE_DELETES(pData3D_HU_Temp_D1);
		SAFE_DELETES(pData3D_HU_Temp_D2);
		return false;
	}

	if (init)
	{
		memset(pData3D_HU_Temp_D1, 0, getVolumeDataLength() * sizeof(double));
		memset(pData3D_HU_Temp_D2, 0, getVolumeDataLength() * sizeof(double));
	}

	return true;
}

bool VOLUME_DATA::createTempHUData(bool init, int newdatalen)
{
	SAFE_DELETES(pData3D_HU_Temp);

	int len = newdatalen < 0 ? dataLenth : newdatalen;

	try
	{
		pData3D_HU_Temp = new mint16[len];
	}
	catch (...)
	{
		SAFE_DELETES(pData3D_HU_Temp);
		return false;
	}

	if (init) memset(pData3D_HU_Temp, 0, len * sizeof(mint16));

	return true;
}

bool VOLUME_DATA::getLengthForScreen(WINDOW_TYPE type, muint32& c_x, muint32& c_y, muint32& c_z)
{
	unsigned int cx = getCX();
	unsigned int cy = getCY();
	unsigned int cz = getCZ();

	switch (type)
	{
	case WT_CORONAL:
		c_x = cx;
		c_y = cz;
		c_z = cy;
		break;
	case WT_SAGITTAL:
		c_x = cy;
		c_y = cz;
		c_z = cx;
		break;
	case WT_AXIAL:
	default:
		c_x = cx;
		c_y = cy;
		c_z = cz;
		break;
	}

	return true;
}

BoundingBoxI VOLUME_DATA::getBoxForScreen(WINDOW_TYPE type, const BoundingBoxI& box)
{
	BoundingBoxI rBox;

	switch (type)
	{
	case WT_CORONAL:
		rBox.minX = box.minX;
		rBox.maxX = box.maxX;
		rBox.minY = box.minZ;
		rBox.maxY = box.maxZ;
		rBox.minZ = box.minY;
		rBox.maxZ = box.maxY;
		break;
	case WT_SAGITTAL:
		rBox.minX = box.minY;
		rBox.maxX = box.maxY;
		rBox.minY = box.minZ;
		rBox.maxY = box.maxZ;
		rBox.minZ = box.minX;
		rBox.maxZ = box.maxX;
		break;
	case WT_AXIAL:
	default:
		rBox.minX = box.minX;
		rBox.maxX = box.maxX;
		rBox.minY = box.minY;
		rBox.maxY = box.maxY;
		rBox.minZ = box.minZ;
		rBox.maxZ = box.maxZ;
		break;
	}

	return rBox;
}

bool VOLUME_DATA::getSpacingForScreen(WINDOW_TYPE type, float& c_x, float& c_y, float& c_z, bool isMM)
{
	float cx = getSpaceX(isMM);
	float cy = getSpaceY(isMM);
	float cz = getSpaceZ(isMM);

	switch (type)
	{
	case WT_CORONAL:
		c_x = cx;
		c_y = cz;
		c_z = cy;
		break;
	case WT_SAGITTAL:
		c_x = cy;
		c_y = cz;
		c_z = cx;
		break;
	case WT_AXIAL:
	default:
		c_x = cx;
		c_y = cy;
		c_z = cz;
		break;
	}

	return true;
}

BoundingBoxI VOLUME_DATA::getLayerBoundingBoxForScreen(WINDOW_TYPE type, int uid, bool isDrawcut)
{
	if (!isDrawcut)
	{
		if (uid == MASK_MAX)
		{
			BoundingBoxI defaultBox, r;
			defaultBox.reset(getCX(), getCY(), getCZ());
			switch (type)
			{
			case WT_CORONAL:
				r.reset(getCX(), getCZ(), getCY());
				r.minX = defaultBox.minX;
				r.maxX = defaultBox.maxX;
				r.minY = defaultBox.minZ;
				r.maxY = defaultBox.maxZ;
				r.minZ = defaultBox.minY;
				r.maxZ = defaultBox.maxY;
				break;
			case WT_SAGITTAL:
				r.reset(getCY(), getCZ(), getCX());
				r.minX = defaultBox.minY;
				r.maxX = defaultBox.maxY;
				r.minY = defaultBox.minZ;
				r.maxY = defaultBox.maxZ;
				r.minZ = defaultBox.minX;
				r.maxZ = defaultBox.maxX;
				break;
			case WT_AXIAL:
			default:
				r = defaultBox;
				break;
			}

			return r;
		}


		BoundingBoxI r = m_boundingBoxROI[uid];
		switch (type)
		{
		case WT_CORONAL:
			r.reset(m_boundingBoxROI[uid].getLimitX(), m_boundingBoxROI[uid].getLimitZ(), m_boundingBoxROI[uid].getLimitY());
			if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION ||
				WIN_MANAGER->mainTabType == MAINTAB_TA)
			{
				r.minX = m_boundingBoxROI[uid].minX;
				r.maxX = m_boundingBoxROI[uid].maxX;
				r.minY = m_boundingBoxROI[uid].minZ;
				r.maxY = m_boundingBoxROI[uid].maxZ;
				r.minZ = m_boundingBoxROI[uid].minY;
				r.maxZ = m_boundingBoxROI[uid].maxY;
			}

			break;
		case WT_SAGITTAL:
			r.reset(m_boundingBoxROI[uid].getLimitY(), m_boundingBoxROI[uid].getLimitZ(), m_boundingBoxROI[uid].getLimitX());
			if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION ||
				WIN_MANAGER->mainTabType == MAINTAB_TA)
			{
				r.minX = m_boundingBoxROI[uid].minY;
				r.maxX = m_boundingBoxROI[uid].maxY;
				r.minY = m_boundingBoxROI[uid].minZ;
				r.maxY = m_boundingBoxROI[uid].maxZ;
				r.minZ = m_boundingBoxROI[uid].minX;
				r.maxZ = m_boundingBoxROI[uid].maxX;
			}
			break;
		case WT_AXIAL:
		default:
			r.reset(m_boundingBoxROI[uid].getLimitX(), m_boundingBoxROI[uid].getLimitY(), m_boundingBoxROI[uid].getLimitZ());
			if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION ||
				WIN_MANAGER->mainTabType == MAINTAB_TA)
			{
				r.minX = m_boundingBoxROI[uid].minX;
				r.maxX = m_boundingBoxROI[uid].maxX;
				r.minY = m_boundingBoxROI[uid].minY;
				r.maxY = m_boundingBoxROI[uid].maxY;
				r.minZ = m_boundingBoxROI[uid].minZ;
				r.maxZ = m_boundingBoxROI[uid].maxZ;
			}
			break;
		}

		return r;
	}
	else
	{
		BoundingBoxI defaultBox, r;
		defaultBox.reset(getCX(), getCY(), getCZ());
		r.reset(getCX(), getCY(), getCZ());
		switch (type)
		{
		case WT_CORONAL:
			r.minX = defaultBox.minX;
			r.maxX = defaultBox.maxX;
			r.minY = defaultBox.minZ;
			r.maxY = defaultBox.maxZ;
			r.minZ = defaultBox.minY;
			r.maxZ = defaultBox.maxY;
			break;
		case WT_SAGITTAL:
			r.minX = defaultBox.minY;
			r.maxX = defaultBox.maxY;
			r.minY = defaultBox.minZ;
			r.maxY = defaultBox.maxZ;
			r.minZ = defaultBox.minX;
			r.maxZ = defaultBox.maxX;
			break;
		case WT_AXIAL:
		default:
			r.minX = defaultBox.minX;
			r.maxX = defaultBox.maxX;
			r.minY = defaultBox.minY;
			r.maxY = defaultBox.maxY;
			r.minZ = defaultBox.minZ;
			r.maxZ = defaultBox.maxZ;
			break;
		}

		return r;
	}
}

BoundingBoxI VOLUME_DATA::getBoundingBoxForScreen(WINDOW_TYPE type)
{
	BoundingBoxI r = m_boundingBox;
	switch (type)
	{
	case WT_CORONAL:
		r.reset(m_boundingBox.getLimitX(), m_boundingBox.getLimitZ(), m_boundingBox.getLimitY());
		if (WIN_MANAGER->mainTabType != MAINTAB_SEGMENTATION) break;
		r.minX = m_boundingBox.minX;
		r.maxX = m_boundingBox.maxX;
		r.minY = m_boundingBox.minZ;
		r.maxY = m_boundingBox.maxZ;
		r.minZ = m_boundingBox.minY;
		r.maxZ = m_boundingBox.maxY;
		break;
	case WT_SAGITTAL:
		r.reset(m_boundingBox.getLimitY(), m_boundingBox.getLimitZ(), m_boundingBox.getLimitX());
		if (WIN_MANAGER->mainTabType != MAINTAB_SEGMENTATION) break;
		r.minX = m_boundingBox.minY;
		r.maxX = m_boundingBox.maxY;
		r.minY = m_boundingBox.minZ;
		r.maxY = m_boundingBox.maxZ;
		r.minZ = m_boundingBox.minX;
		r.maxZ = m_boundingBox.maxX;
		break;
	case WT_AXIAL:
	default:
		r.reset(m_boundingBox.getLimitX(), m_boundingBox.getLimitY(), m_boundingBox.getLimitZ());
		if (WIN_MANAGER->mainTabType != MAINTAB_SEGMENTATION) break;
		r.minX = m_boundingBox.minX;
		r.maxX = m_boundingBox.maxX;
		r.minY = m_boundingBox.minY;
		r.maxY = m_boundingBox.maxY;
		r.minZ = m_boundingBox.minZ;
		r.maxZ = m_boundingBox.maxZ;
		break;
	}

	return r;
}

void VOLUME_DATA::setData(muint32 index, mint16 data)
{
	if (index > (dataLenth - 1)) return;

	pData3D_HU[index] = data;

	if (m_bCreate)
	{
		m_bCreate = false;
		HUMin = data;
		HUMax = data;
	}
	else
	{
		if (HUMin > data)
			HUMin = data;
		else if (HUMax < data)
			HUMax = data;
	}
}

void VOLUME_DATA::setData(muint32 x, muint32 y, muint32 z, mint16 data)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ) return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1)) return;

	pData3D_HU[index] = data;
	if (m_bCreate)
	{
		m_bCreate = false;
		HUMin = data;
		HUMax = data;
	}
	else
	{
		if (HUMin > data)
			HUMin = data;
		else if (HUMax < data)
			HUMax = data;
	}
}

float VOLUME_DATA::getSpaceX(bool isMM)
{
	if (!isMM)
		return spaceX;

	return spaceX * 10;
}

float VOLUME_DATA::getSpaceY(bool isMM)
{
	if (!isMM)
		return spaceY;
	return spaceY * 10;
}

float VOLUME_DATA::getSpaceZ(bool isMM)
{
	if (!isMM)
		return spaceZ;
	return spaceZ * 10;
}

float VOLUME_DATA::getSizeX(bool isMM /*= false*/)
{
	float _size = getSpaceX(isMM);

	return _size * float(dataCX);
}

float VOLUME_DATA::getSizeY(bool isMM /*= false*/)
{
	float _size = getSpaceY(isMM);

	return _size * float(dataCY);
}

float VOLUME_DATA::getSizeZ(bool isMM)
{
	float _size = getSpaceZ(isMM);

	return _size * float(dataCZ);
}

float VOLUME_DATA::getSpace3D(bool isMM /*= false*/)
{
	if (!isMM)
	{
		return spaceX * spaceY * spaceZ;
	}
	return (spaceX * 10) * (spaceY * 10) * (spaceZ * 10);
}

float VOLUME_DATA::getSpace2D(bool isMM /*= false*/)
{
	if (!isMM)
	{
		return spaceX * spaceY;
	}
	return (spaceX * 10) * (spaceY * 10);
}

float VOLUME_DATA::getMinSpace(bool isMM /*= false*/)
{
	float compareXY = spaceX >= spaceY ? spaceY : spaceX;
	float compareXYZ = compareXY >= spaceZ ? spaceZ : compareXY;

	if (!isMM)
	{
		return compareXYZ;
	}
	return compareXYZ * 10;
}

bool VOLUME_DATA::checkMaskUsed(mask _mask, int maskIndex)
{
	BoundingBoxI box = getBoundingBox();

	for (int z = box.minZ; z <= box.maxZ; z++)
	{
		for (int y = box.minY; y <= box.maxY; y++)
		{
			for (int x = box.minX; x <= box.maxX; x++)
			{
				if (isMaskBit(x, y, z, _mask, maskIndex)) return true;
			}
		}
	}

	return false;
}

void VOLUME_DATA::setMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return;

	CheckDataPoint(Maskindex);

	if (!(pData3D_Mask[Maskindex][index] & data))
	{
		pData3D_Mask[Maskindex][index] |= data;
		fillMaskCount++;
	}
}

mint16 VOLUME_DATA::getData(muint32 x, muint32 y, muint32 z)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return 0;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return 0;

	return pData3D_HU[index];
}

mint16 VOLUME_DATA::getData(muint32 index)
{
	if (index > (dataLenth - 1))
		return 0;

	return pData3D_HU[index];
}

void VOLUME_DATA::delMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return;

	CheckDataPoint(Maskindex);

	if (pData3D_Mask[Maskindex][index] & data)
	{
		pData3D_Mask[Maskindex][index] &= ~data;
		fillMaskCount--;
	}
}

muint8 VOLUME_DATA::getMaskData(muint32 x, muint32 y, muint32 z, int MaskByteIndex)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return 0;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return 0;

	return pData3D_Mask[MaskByteIndex][index];
}

void VOLUME_DATA::setMaskData(muint32 x, muint32 y, muint32 z, mask value, int MaskByteIndex)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;
	if (index > (dataLenth - 1))
		return;

	CheckDataPoint(MaskByteIndex);
	pData3D_Mask[MaskByteIndex][index] = value;
}

void  VOLUME_DATA::setMaskData(muint32 index, mask value, int Maskindex)
{
	if (index > (dataLenth - 1))
		return;

	CheckDataPoint(Maskindex);
	pData3D_Mask[Maskindex][index] = value;
}

mask VOLUME_DATA::getMaskData(mint32 index, int MaskByteIndex)
{
	if (index > (dataLenth - 1))
	{
		return 0;
	}

	if (!pData3D_Mask[MaskByteIndex])
	{
		return 0;
	}

	return pData3D_Mask[MaskByteIndex][index];
}

bool VOLUME_DATA::isMaskBit(muint32 index, mask data, int Maskindex)
{
	if (index > (dataLenth - 1)) return false;

	return ((pData3D_Mask[Maskindex][index] & data) != 0);
}

bool VOLUME_DATA::isMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex)
{
	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return false;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return false;

	return ((pData3D_Mask[Maskindex][index] & data) != 0);
}

bool VOLUME_DATA::isMaskBit(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data, int Maskindex)
{
	switch (type)
	{
	case WT_CORONAL:
		return isMaskBit(x, depth, y, data, Maskindex);
	case WT_SAGITTAL:
		return isMaskBit(depth, x, y, data, Maskindex);
	case WT_AXIAL:
		return isMaskBit(x, y, depth, data, Maskindex);
	default:
		return false;
	}
}

void  VOLUME_DATA::applyMaskCopy(mask source, mask dest, int Maskindex)
{
	CheckDataPoint(Maskindex);

	mint32 length = getVolumeDataLength();
	for (int n = 0; n < length; ++n)
	{
		pData3D_Mask[Maskindex][n] &= ~dest;

		if (pData3D_Mask[Maskindex][n] & source)
			pData3D_Mask[Maskindex][n] |= dest;
	}
}

void  VOLUME_DATA::applyMaskAdd(mask source, mask dest, int Maskindex, int destMaskindex)
{
	CheckDataPoint(Maskindex);
	CheckDataPoint(destMaskindex);

	mint32 length = getVolumeDataLength();
	fillMaskCount = 0;
	for (int n = 0; n < length; ++n)
	{
		if (pData3D_Mask[Maskindex][n] & source &&
			!(pData3D_Mask[destMaskindex][n] & dest))
		{
			pData3D_Mask[destMaskindex][n] |= dest;
			fillMaskCount++;
		}
	}
}

COLOR VOLUME_DATA::getGrayColorData(muint32 x, muint32 y, muint32 z, mint32 window_level, mint32 window_width, bool color_mode, bool invert_mode)
{
	mint16 HU = getData(x, y, z); // 2017.08.24 이두희 팀장 수정 muint16 → mint16
	muint8 color = 0;

	if (HU <= (window_level - window_width / 2))
		color = 0;
	else if (HU >= (window_level + window_width / 2))
		color = 255;  // 2017.08.24 이두희 팀장 수정 0 → 255
	else
		color = 255 * (HU - (window_level - window_width / 2)) / window_width;

	QColor _color = get2Dcolor(color);

	COLOR result;
	if (color_mode)
	{
		result.r = _color.red();
		result.g = _color.green();
		result.b = _color.blue();
	}
	else if (invert_mode)
	{
		result.r = 255 - color;
		result.g = 255 - color;
		result.b = 255 - color;
	}
	else
	{
		result.r = color;
		result.g = color;
		result.b = color;
	}
	result.a = 255;
	return result;
}

void VOLUME_DATA::setMaskBitList(WINDOW_TYPE type, muint32 x, muint32 y, muint32 z, std::vector<QPoint>* list, mask data, int Maskindex)
{
	muint32 index = 0;
	for (auto a = list->begin(); a != list->end(); ++a)
	{
		switch (type)
		{
		case WT_CORONAL:
			setMaskBit(x + a->x(), y, z + a->y(), data, Maskindex);
			break;
		case WT_SAGITTAL:
			setMaskBit(x, y + a->x(), z + a->y(), data, Maskindex);
			break;
		case WT_AXIAL:
			setMaskBit(x + a->x(), y + a->y(), z, data, Maskindex);
			break;
		default:
			continue;
			break;
		}
	}
}

void VOLUME_DATA::setMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask data, int Maskindex)
{
	muint32 index = 0;
	fillMaskCount = 0;
	for (auto a = list->begin(); a != list->end(); ++a)
	{
		switch (type)
		{
		case WT_CORONAL:
			setMaskBit(a->x(), depth, a->y(), data, Maskindex);
			break;
		case WT_SAGITTAL:
			setMaskBit(depth, a->x(), a->y(), data, Maskindex);
			break;
		case WT_AXIAL:
			setMaskBit(a->x(), a->y(), depth, data, Maskindex);
			break;
		default:
			continue;
			break;
		}
	}
}

void VOLUME_DATA::delMaskBitList(WINDOW_TYPE type, muint32 x, muint32 y, muint32 z, std::vector<QPoint>* list, mask data, int Maskindex)
{
	muint32 index = 0;
	for (auto a = list->begin(); a != list->end(); ++a)
	{
		switch (type)
		{
		case WT_CORONAL:
			delMaskBit(x + a->x(), y, z + a->y(), data, Maskindex);
			break;
		case WT_SAGITTAL:
			delMaskBit(x, y + a->x(), z + a->y(), data, Maskindex);
			break;
		case WT_AXIAL:
			delMaskBit(x + a->x(), y + a->y(), z, data, Maskindex);
			break;
		default:
			continue;
			break;
		}
	}
}

void VOLUME_DATA::delMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask data, int Maskindex)
{
	muint32 index = 0;
	fillMaskCount = 0;
	for (auto a = list->begin(); a != list->end(); ++a)
	{
		switch (type)
		{
		case WT_CORONAL:
			delMaskBit(a->x(), depth, a->y(), data, Maskindex);
			break;
		case WT_SAGITTAL:
			delMaskBit(depth, a->x(), a->y(), data, Maskindex);
			break;
		case WT_AXIAL:
			delMaskBit(a->x(), a->y(), depth, data, Maskindex);
			break;
		default:
			continue;
			break;
		}
	}
}

void VOLUME_DATA::setTempMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask data)
{
	muint32 index = 0;
	for (auto a = list->begin(); a != list->end(); ++a)
	{
		switch (type)
		{
		case WT_CORONAL:
			setTempMaskBit(a->x(), depth, a->y(), data);
			break;
		case WT_SAGITTAL:
			setTempMaskBit(depth, a->x(), a->y(), data);
			break;
		case WT_AXIAL:
			setTempMaskBit(a->x(), a->y(), depth, data);
			break;
		default:
			continue;
			break;
		}
	}
}

void VOLUME_DATA::setTempMaskBit(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data)
{
	muint32 index = 0;
	switch (type)
	{
	case WT_CORONAL:
		delTempMaskBit(x, depth, y, data);
		break;
	case WT_SAGITTAL:
		delTempMaskBit(depth, x, y, data);
		break;
	case WT_AXIAL:
		delTempMaskBit(x, y, depth, data);
		break;
	default:
		break;
	}
}

muint32 VOLUME_DATA::getVolumeIndex(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth)
{
	muint32 _x, _y, _z;
	switch (type)
	{
	case WT_CORONAL:
		_x = x;
		_y = depth;
		_z = y;
		break;
	case WT_SAGITTAL:
		_x = depth;
		_y = x;
		_z = y;
		break;
	case WT_AXIAL:
		_x = x;
		_y = y;
		_z = depth;
		break;
	default:break;
	}

	if (_x >= dataCX || _y >= dataCY || _z >= dataCZ)
		return 0;

	return (_z * dataCX * dataCY) + (_y * dataCX) + _x;
}

void VOLUME_DATA::setTempMaskBit(muint32 x, muint32 y, muint32 z, mask data)
{
	if (pData3D_Mask_Temp == NULL)
		return;

	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return;

	pData3D_Mask_Temp[index] |= data;
}

void VOLUME_DATA::delTempMaskBit(muint32 x, muint32 y, muint32 z, mask data)
{
	if (pData3D_Mask_Temp == NULL)
		return;

	if (x >= dataCX || y >= dataCY || z >= dataCZ)
		return;

	muint32 index = (z * dataCX * dataCY) + (y * dataCX) + x;

	if (index > (dataLenth - 1))
		return;

	pData3D_Mask_Temp[index] &= ~data;
}

void VOLUME_DATA::setTempMaskBitAuto(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data)
{
	switch (type)
	{
	case WT_CORONAL:
		setTempMaskBit(x, depth, y, data);
		break;
	case WT_SAGITTAL:
		setTempMaskBit(depth, x, y, data);
		break;
	case WT_AXIAL:
		setTempMaskBit(x, y, depth, data);
		break;
	default:break;
	}
}

void VOLUME_DATA::delTempMaskBitAuto(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data)
{
	switch (type)
	{
	case WT_CORONAL:
		delTempMaskBit(x, depth, y, data);
		break;
	case WT_SAGITTAL:
		delTempMaskBit(depth, x, y, data);
		break;
	case WT_AXIAL:
		delTempMaskBit(x, y, depth, data);
		break;
	default:
		break;
	}
}

void VOLUME_DATA::moveMaskToTempMask(mask dest_mask, mask source_mask, int dest_Maskindex, int source_Maskindex)
{
	if (pData3D_Mask_Temp == NULL)
		return;

	for (int n = 0; n < getVolumeDataLength(); n++)
	{
		if (pData3D_Mask[source_Maskindex][n] & source_mask)
		{
			pData3D_Mask_Temp[n] |= dest_mask;
			if (source_Maskindex == dest_Maskindex)
				pData3D_Mask[dest_Maskindex][n] &= ~source_mask;
			else
				pData3D_Mask[source_Maskindex][n] &= ~source_mask;
		}
	}
}

void VOLUME_DATA::AutoWindowing(int iNumBins, float fAutoScaleSlope, float fRescaleIntercept, double& dWindowWidth, double& dWindowLevel)
{
	int i, j;
	int iMinThreshold = 65535;
	int iMaxThreshold = 0;
	int z = dataCZ / 2;
	short* spBuffer = new short[dataCX * dataCY];

	for (i = 0; i < dataCY; i++)
	{
		for (j = 0; j < dataCX; j++)
		{
			spBuffer[i * dataCX + j] = pData3D_HU[i * dataCX + j + dataCX * dataCY * z];
			if (iMinThreshold > spBuffer[i * dataCX + j])
			{
				iMinThreshold = spBuffer[i * dataCX + j];
			}
			if (iMaxThreshold < spBuffer[i * dataCX + j])
			{
				iMaxThreshold = spBuffer[i * dataCX + j];
			}
		}
	}

	int iHistWidth = iMaxThreshold - iMinThreshold;
	float fDivideVal = 1.0;
	fDivideVal = ((float)(iHistWidth) / (float)iNumBins + 0.25);

	int* piHist = new int[iNumBins];
	memset(piHist, 0x00, sizeof(int) * iNumBins);

	int iPixelNum = 0;

	for (i = 0; i < dataCY; i++)
	{
		for (j = 0; j < dataCX; j++)
		{
			if (iMinThreshold < 0)
			{
				int histIdx = spBuffer[i * dataCX + j] + abs(iMinThreshold);
				histIdx = histIdx / fDivideVal;
				piHist[histIdx]++;
				iPixelNum++;
			}
			else
			{
				int histIdx = spBuffer[i * dataCX + j] - (iMinThreshold);
				histIdx = histIdx / fDivideVal;
				piHist[histIdx]++;
				iPixelNum++;
			}
		}
	}

	int iMaxHist = 0;
	for (i = 0; i < iNumBins; i++)
	{
		if (iMaxHist < piHist[i])
		{
			iMaxHist = piHist[i];
		}
	}

	int iAutoThreshold = iHistWidth / 2;
	int iThreshold = iPixelNum / iAutoThreshold;
	int iLimit = iPixelNum / 10;
	int iCount;
	bool bFound;
	int hmin = 0;
	int hmax = 0;
	i = -1;
	do {
		i++;
		iCount = piHist[i];
		if (iCount > iLimit) iCount = 0;
		bFound = iCount > iThreshold;
	} while (!bFound && i < 255);
	hmin = i;
	i = iNumBins;
	do {
		i--;
		iCount = piHist[i];
		if (iCount > iLimit) iCount = 0;
		bFound = iCount > iThreshold;
	} while (!bFound && i > 0);
	hmax = i;
	int	currentMin = iMinThreshold;
	int currentMax = iMaxThreshold;

	double binSize = (double)(iMaxThreshold - iMinThreshold) / (double)iNumBins;
	if (hmax >= hmin) {
		currentMin = (double)hmin * binSize;
		currentMax = (double)hmax * binSize;
		if (currentMin == currentMax) {
			currentMin = iMinThreshold;
			currentMax = iMaxThreshold;
		}
	}
	double currentWindow = currentMax - currentMin;
	double currentLevel = currentMin + (.5 * currentWindow);

	//change
	double newWindow = currentWindow + 0;
	double newLevel = currentLevel + 0;

	if (newWindow < 0)
		newWindow = 0;
	if (newLevel < 0)
		newLevel = 0;

	double rescaleSlope = fAutoScaleSlope; // Default 1.0
	double rescaleIntercept = fRescaleIntercept; // Default -1000

	dWindowWidth = newWindow * rescaleSlope;
	dWindowLevel = (newLevel - currentMin) * rescaleSlope + rescaleIntercept;

	delete[] piHist;
	delete[] spBuffer;
}

MaskInfo* VOLUME_DATA::getCurrentMaskInfo()
{
	if (m_vecMaskInfoList.size() == 0)
	{
		createMaskInfo();
	}

	if (m_vecMaskInfoList.size() <= m_currentMaskInfoIndex)
		return m_vecMaskInfoList[0];

	return m_vecMaskInfoList[m_currentMaskInfoIndex];
}

int VOLUME_DATA::getCurrentMaskIndex()
{
	MaskInfo* info = getCurrentMaskInfo();

	if (info && (info->uid >= MASK_SECOND_MAX))
		return (info->uid - MASK_SECOND_MAX) / 8 + 1;

	return 0;
}

bool VOLUME_DATA::isEmptyMaskVoxel(int index, bool seed, bool bUID)
{
	if (seed)
	{
		if (m_seedVoxel[index] <= 0)
			return true;
	}
	else
	{
		MaskInfo* info = getMaskInfo(index, bUID);

		if (info != nullptr)
		{
			if (m_voxelCount[info->uid] <= 0)
				return true;
		}
	}
	return false;
}

void VOLUME_DATA::setMaskName(QString maskName, int index, bool bUID)
{
	QString strName;
	if (isDuplicateMaskName(maskName))
	{
		static int sn_num = 0;
		strName = maskName + QString("-dup%1").arg(sn_num++);
	}
	else
	{
		strName = maskName;
	}

	if (!bUID)
	{
		if (m_vecMaskInfoList.size() <= index)
			return;

		::StringCbPrintf(m_vecMaskInfoList[index]->maskName, MASKINFO_TEXT_LENGTH_MAX * sizeof(WCHAR), strName.toStdWString().c_str());
	}
	else
	{
		int index2 = 0;
		for (auto mask = m_vecMaskInfoList.begin(); mask != m_vecMaskInfoList.end(); ++mask)
		{
			if ((*mask)->uid == index)
				break;
			index2++;
		}
		if (m_vecMaskInfoList.size() <= index2)
			return;

		::StringCbPrintf(m_vecMaskInfoList[index2]->maskName, MASKINFO_TEXT_LENGTH_MAX * sizeof(WCHAR), strName.toStdWString().c_str());
	}
}

bool VOLUME_DATA::isDuplicateMaskName(QString changedMaskName)
{
	bool bDuplicate = false;
	MaskInfo* info = nullptr;
	for (int i = 0; i < m_vecMaskInfoList.size(); i++)
	{
		info = m_vecMaskInfoList[i];
		if (!info)
			continue;

		QString curMaskName = QString::fromWCharArray(info->maskName);

		if (!curMaskName.compare(changedMaskName))
		{
			bDuplicate = true;
			break;
		}
	}

	return bDuplicate;
}

QString VOLUME_DATA::makeUniqueMaskNameWithSuffixNumber(QString basicMaskName, int curSufficNumber)
{
	QString uniqueMaskName = "";
	if (!curSufficNumber)
		uniqueMaskName = basicMaskName;
	else
		uniqueMaskName = basicMaskName + QString::number(curSufficNumber);

	while (isDuplicateMaskName(uniqueMaskName))
		uniqueMaskName = basicMaskName + QString::number(++curSufficNumber);
	return uniqueMaskName;
}

QString VOLUME_DATA::makeUniqueMaskNameWithCopyText(QString basicMaskName)
{
	QString uniqueMaskName = basicMaskName;

	int curSuffixNumber = 1;
	while (isDuplicateMaskName(uniqueMaskName))
	{
		if (curSuffixNumber == 1)
		{
			uniqueMaskName = QString("%1 copy").arg(basicMaskName);
		}
		else
		{
			uniqueMaskName = QString("%1 copy (%2)").arg(basicMaskName).arg(curSuffixNumber);
		}
		curSuffixNumber++;
	}

	return uniqueMaskName;
}

QString VOLUME_DATA::getMaskName(int index, bool bUID)
{
	if (!bUID)
		return QString((QChar*)m_vecMaskInfoList[index]->maskName);
	else
	{
		int index2 = 0;
		for (auto mask = m_vecMaskInfoList.begin(); mask != m_vecMaskInfoList.end(); ++mask)
		{
			if ((*mask)->uid == index)
				return QString((QChar*)m_vecMaskInfoList[index2]->maskName);

			index2++;
		}
		return QString("");
	}
}

MaskInfo* VOLUME_DATA::getMaskInfo(muint32 index, bool bUID)
{
	if (!bUID)
	{
		if (m_vecMaskInfoList.size() <= index || index < 0)
			return NULL;

		return m_vecMaskInfoList[index];
	}

	for (int i = 0; i < getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList[i];
		if (info->uid == index)
		{
			return m_vecMaskInfoList[i];
		}
	}

	return NULL;
}

MaskInfo* VOLUME_DATA::getMaskInfoByIndex(muint32 layerIndex)
{
	if (m_vecMaskInfoList.size() <= layerIndex || layerIndex < 0)
	{
		return nullptr;
	}

	return m_vecMaskInfoList[layerIndex];
}

MaskInfo* VOLUME_DATA::getMaskInfoByUID(muint32 uid)
{
	for (int i = 0; i < getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList[i];
		if (info->uid == uid)
		{
			return m_vecMaskInfoList[i];
		}
	}

	return nullptr;
}

mask VOLUME_DATA::getMask(muint32 uid)
{
	const MaskInfo* info = getMaskInfo(uid, true);

	if (info == nullptr)
		return VM_MASK2;

	if (info->uid >= MASK_SECOND_MAX)
		return info->mask_id2;

	return info->mask_id;
}

int VOLUME_DATA::GetMaskByteIndex(muint32 uid)
{
	MaskInfo* info = getMaskInfo(uid, true);

	if (info == 0)
		return 0;

	if (info->uid >= MASK_SECOND_MAX)
		return (info->uid - MASK_SECOND_MAX) / 8 + 1;

	return 0;
}

MaskInfo* VOLUME_DATA::getAtLastMaskInfo(int* pOutIndex)
{
	if (m_vecMaskInfoList.size() < 1)
		createMaskInfo();

	muint32 lastUID = 0;
	int index = 0;
	for (int i = 0; i < getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList[i];

		if (info->uid > lastUID)
		{
			index = i;
			lastUID = info->uid;
		}
	}

	if (pOutIndex)
	{
		*pOutIndex = index;
	}

	return m_vecMaskInfoList[index];
}

MaskInfo* VOLUME_DATA::findMaskInfo(int _mI, mask _m)
{
	int chkIndex = 0;
	mask chkM = VM_MASK0;
	for (int i = 0; i < getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList[i];
		chkIndex = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		chkM = chkIndex == 0 ? info->mask_id : info->mask_id2;
		if (chkIndex == _mI &&
			chkM == _m)
		{
			return info;
		}
	}

	return nullptr;
}

MaskInfo* VOLUME_DATA::findMaskInfo(QString strMaskName, bool isSearchOrder)
{
	// false:foreward, true:backward
	int nStart, nEnd, nStep;
	if (isSearchOrder == false)
	{
		nStart = 0;
		nEnd = getMaskInfoListCnt();
		nStep = 1;
	}
	else
	{
		nStart = getMaskInfoListCnt() - 1;
		nEnd = -1;
		nStep = -1;
	}

	for (int i = nStart; i != nEnd; i += nStep)
	{
		MaskInfo* info = m_vecMaskInfoList[i];
		if (!info)
			return nullptr;

		QString strName = QString::fromWCharArray(info->maskName);

		if (!strName.compare(strMaskName))
		{
			return info;
		}
	}

	return nullptr;

}

void VOLUME_DATA::setMultiSelectUIDByMaskBit(std::vector<std::pair<int, mask8 >> vecMultiMask)
{
	m_vecMultiSelectedMaskUID.clear();

	for (auto i = 0; i < vecMultiMask.size(); ++i)
	{
		MaskInfo* tmpMaskInfo = findMaskInfo(vecMultiMask[i].first, vecMultiMask[i].second);
		if (tmpMaskInfo)
			m_vecMultiSelectedMaskUID.push_back(tmpMaskInfo->uid);
	}
}

void VOLUME_DATA::setMultiSelectUID(const std::vector<muint32>& vecMaskUID)
{
	m_vecMultiSelectedMaskUID.clear();
	m_vecMultiSelectedMaskUID = vecMaskUID;
}

bool VOLUME_DATA::isMaskEditable(void)
{
	bool bEditEnable = false;
	MaskInfo* pMaskInfo = getCurrentMaskInfo();

	if (pMaskInfo != nullptr)
	{
		if (pMaskInfo->bIsUseCredit == false)
			bEditEnable = true;
	}

	return bEditEnable;
}

bool VOLUME_DATA::isMaskEditable(int nIdx)
{
	bool bEditEnable = false;
	MaskInfo* pMaskInfo = getMaskInfo(nIdx);

	if (pMaskInfo != nullptr)
	{
		if (pMaskInfo->bIsUseCredit == false)
			bEditEnable = true;
	}

	return bEditEnable;
}

int VOLUME_DATA::getMaskIndex(int UID)
{
	for (int i = 0; i < getMaskInfoListCnt(); i++)
	{
		MaskInfo* info = m_vecMaskInfoList[i];
		if (info->uid == UID)
		{
			return i;
		}
	}

	return -1;
}

int VOLUME_DATA::getMaskIndexCnt(void)
{
	int nMaskCnt = getMaskInfoListCnt();
	return nMaskCnt > MASK_SECOND_MAX ? (nMaskCnt - MASK_SECOND_MAX - 1) / 8 + 2 : 1;
}

int VOLUME_DATA::getMaskIndexByMaskInfoPtr(MaskInfo* pMaskInfo)
{
	for (int i = 0; i < m_vecMaskInfoList.size(); i++)
	{
		if (pMaskInfo == m_vecMaskInfoList[i])
		{
			return i;
		}
	}

	return -1;
}

bool VOLUME_DATA::createMaskInfo(bool isCopy, bool b_update_ui)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
	{
		return false;
	}

	m_currentMaskInfoIndex = m_vecMaskInfoList.size();

	static int nameIndex = m_currentMaskInfoIndex + 1;

	if (m_bCreate)
		nameIndex = m_currentMaskInfoIndex + 1;

	QString layerName = makeUniqueMaskNameWithSuffixNumber(QString("Layer "), nameIndex++);
	return createNewMaskInfoAndInitData(layerName, isCopy, b_update_ui);
}

bool VOLUME_DATA::createMaskInfoWithName(const QString& name, bool isCopy, bool b_update_ui)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
	{
		return false;
	}

	m_currentMaskInfoIndex = m_vecMaskInfoList.size();

	QString layerName = makeUniqueMaskNameWithCopyText(name);
	return createNewMaskInfoAndInitData(layerName, isCopy, b_update_ui);
}

muint32 VOLUME_DATA::indexGenForMask()
{
	muint32 index = 0;
	for (; index < MASK_MAX; index++)
	{
		if (!(m_useMaskInfoBit & (1 << index)))
			break;
	}
	return index;
}

COLOR VOLUME_DATA::colorGen(muint32 index)
{
	COLOR colorTable[] = { COLOR(170,0,0,255), COLOR(66,255,255,255), COLOR(59,255,56,255),
							COLOR(255,33,244,255), COLOR(165,127,231,255), COLOR(214,117,130,255),
							COLOR(51,135,214,255), COLOR(214,168,127,255) };

	if (index >= 8)
		index = index % 8;

	return colorTable[index];
}

QColor VOLUME_DATA::getMaskColor(muint32 index, bool bUID)
{
	MaskInfo* info = nullptr;
	if (bUID)
	{
		for (int i = 0; i < m_vecMaskInfoList.size(); i++)
		{
			info = m_vecMaskInfoList.at(i);
			if (info)
			{
				if (info->uid == index)
				{
					index = i;
					break;
				}
			}
			info = nullptr;
		}
	}
	else
	{
		if (index >= m_vecMaskInfoList.size())
			return QColor(255, 0, 0);

		info = m_vecMaskInfoList.at(index);
	}

	if (info)
	{
		QColor col(info->color.r, info->color.g, info->color.b);

		return col;
	}

	return QColor(255, 0, 0);
}

bool VOLUME_DATA::delMaskInfo(muint32 index, DeletedMaskInfo* pOutDeleteMaskResultInfo)
{
	if (m_vecMaskInfoList.size() == 1)
	{
		return false;
	}

	if (index < 0 || index >= m_vecMaskInfoList.size())
	{
		m_currentMaskInfoIndex = 0;
		return false;
	}

	MaskInfo* pDeleteInfo = m_vecMaskInfoList[index];
	int deleteMaskByteIndex = GetMaskByteIndex(pDeleteInfo);
	int deleteVoxelCount = getVoxelCount(pDeleteInfo->uid);
	BoundingBoxI deleteBoundingBox = getBoundingBox(pDeleteInfo->uid);
	std::vector<unsigned char> deleteAIResult = GetAIResultByUID(pDeleteInfo->uid);
	int deleteAIOutset = GetAIOutsetByUID(pDeleteInfo->uid);

	int lastMaskInfoListIndex = 0;
	MaskInfo* pLastInfo = getAtLastMaskInfo(&lastMaskInfoListIndex);
	int lastMaskByteIndex = GetMaskByteIndex(pLastInfo);
	int lastVoxelCount = getVoxelCount(pLastInfo->uid);
	BoundingBoxI lastBoundingBox = getBoundingBox(pLastInfo->uid);
	std::vector<unsigned char> lastAIResult = GetAIResultByUID(pLastInfo->uid);
	int lastAIOutset = GetAIOutsetByUID(pLastInfo->uid);

	if (pOutDeleteMaskResultInfo)
	{
		pOutDeleteMaskResultInfo->SetMovedMaskInfo(*pLastInfo, lastMaskInfoListIndex, lastVoxelCount, lastBoundingBox, lastAIResult, lastAIOutset);
		pOutDeleteMaskResultInfo->SetDeletedMaskInfo(*pDeleteInfo, index, deleteVoxelCount, deleteBoundingBox, deleteAIResult, deleteAIOutset);
	}

	clearMaskDataByInfo(pDeleteInfo);

#ifdef SEGMENTATION_COLOR_CODING
	DeleteAIDataByUID(pDeleteInfo->uid);
#endif

	bool isMaskMoved = false;
	if (lastMaskByteIndex > deleteMaskByteIndex)
	{
		moveMaskInfoAndData(pLastInfo, pDeleteInfo);
		isMaskMoved = true;
	}
	else
	{
		deleteMaskInfoAndData(pDeleteInfo);
		isMaskMoved = false;
	}

	if (pOutDeleteMaskResultInfo)
	{
		pOutDeleteMaskResultInfo->SetMoved(isMaskMoved);
	}

	eraseMaskInfoByIndexAndSetCurrentIndex(index);

	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView(true);
	return true;
}

bool VOLUME_DATA::delMaskInfoList(std::vector<int> layerIndexList, std::vector<DeletedMaskInfo>* pOutDeleteMaskResultInfoList)
{
	if (layerIndexList.empty())
	{
		return false;
	}

	if (pOutDeleteMaskResultInfoList)
	{
		pOutDeleteMaskResultInfoList->clear();
	}

	std::vector<MaskInfo*> layerMaskInfoList;
	for (muint32 layerIndex : layerIndexList)
	{
		layerMaskInfoList.push_back(getMaskInfoByIndex(layerIndex));
	}

	bool result = false;
	for (MaskInfo* pInfo : layerMaskInfoList)
	{
		int layerIndex = getMaskIndexByMaskInfoPtr(pInfo);
		if (layerIndex != -1)
		{
			DeletedMaskInfo deleteMaskInfo;
			result = delMaskInfo(layerIndex, &deleteMaskInfo);
			if (pOutDeleteMaskResultInfoList)
			{
				pOutDeleteMaskResultInfoList->push_back(deleteMaskInfo);
			}
		}
	}

	return result;
}

bool VOLUME_DATA::delMaskInfos(std::vector<muint32>& uid_list, std::vector<UIDMask>& mv_vector, bool& existSurface, bool bDeleteAll)
{
	if (!bDeleteAll)
	{
		if (m_vecMaskInfoList.size() == 1)
			return false;
	}
	std::sort(uid_list.begin(), uid_list.end(), std::less<unsigned int>());//내림차순 정렬

	std::vector<MaskInfo*> del_list;
	std::vector<int> del_iter;
	del_list.clear();
	del_list.reserve(uid_list.size());

	int idx = 0;
	for (int i = 0; i < uid_list.size(); i++)
	{
		idx = 0;
		int uid = uid_list[i];

		for (auto delInfo = m_vecMaskInfoList.begin(); delInfo != m_vecMaskInfoList.end(); ++delInfo)
		{
			MaskInfo* _info = (*delInfo);

			if (uid == _info->uid)
			{
				//if (WIN_MANAGER->deleteMesh(uid))
				//	existSurface = true;
				del_list.push_back(_info);
				del_iter.push_back(idx);
#ifdef MULTI_DRAWCUT_MODE
				DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

				if (tab)
					tab->DelSeed(uid);
#endif
				break;
			}
			idx++;
		}
	}

	std::sort(del_iter.begin(), del_iter.end(), std::greater<int>());

	for (int i = 0; i < del_iter.size(); i++)
	{
		auto delInfo = m_vecMaskInfoList.begin();
		delInfo += del_iter.at(i);

		m_vecMaskInfoList.erase(delInfo);
	}

	for (int i = 0; i < del_list.size(); i++)
	{
		MaskInfo* del_info = del_list.at(i);

		setTAState(del_info->uid, false);

		idx = del_info->uid;

		int newMaskIndex = 0;
		int oldMaskIndex = 0;
		if (idx >= MASK_SECOND_MAX)
			newMaskIndex = (idx - MASK_SECOND_MAX) / 8 + 1;
		uchar newMaskID = newMaskIndex == 0 ? del_info->mask_id : del_info->mask_id2;
		clearMaskData(newMaskID, newMaskIndex);

		//1. 가장 뒤에 위치한 MaskInfo 가져오기
		MaskInfo* pLastInfo = getAtLastMaskInfo();
		//2. maskindex 비교하기 (newmaskindex가 클 경우에만 동작)
		if (pLastInfo->uid >= MASK_SECOND_MAX)
			oldMaskIndex = (pLastInfo->uid - MASK_SECOND_MAX) / 8 + 1;

#ifdef SEGMENTATION_COLOR_CODING
		for (auto it = m_vecAIResultData.begin(); it != m_vecAIResultData.end(); ++it)
		{
			if (it->first == del_info->uid)
			{
				m_vecAIResultData.erase(it);
				break;
			}
		}

		for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
		{
			if (it->first == del_info->uid)
			{
				m_vecAIOutset.erase(it);
				break;
			}
		}
#endif

		if (oldMaskIndex > newMaskIndex)
		{
			m_voxelCount[del_info->uid] = m_voxelCount[pLastInfo->uid];
			m_boundingBoxROI[del_info->uid] = m_boundingBoxROI[pLastInfo->uid];
			m_voxelCount[pLastInfo->uid] = 0;
			m_boundingBoxROI[pLastInfo->uid].reset(true);
			//2-2. maskinfo move
			moveMaskBitData(oldMaskIndex, newMaskIndex, pLastInfo->mask_id2, newMaskID);

			for (auto eraseMask = m_maskIDList[oldMaskIndex].begin(); eraseMask != m_maskIDList[oldMaskIndex].end(); ++eraseMask)
			{
				if (*eraseMask == pLastInfo->mask_id2)
				{
					m_maskIDList[oldMaskIndex].erase(eraseMask);
					break;
				}
			}

			muint32 preUid = pLastInfo->uid;

			UIDMask _pair;

			_pair.uid = pLastInfo->uid;
			_pair._m = pLastInfo->mask_id2;

			mv_vector.push_back(_pair);

			m_useMaskInfoBit &= ~(1 << pLastInfo->uid);
			pLastInfo->mask_id = del_info->mask_id;
			pLastInfo->mask_id2 = del_info->mask_id2;
			pLastInfo->uid = del_info->uid;

			WIN_MANAGER->ChangeUID(preUid, pLastInfo->uid);

			//2-3. vector에 저장된 old mask id 삭제 후 vector 비어있으면 해당 pdata 삭제
			if (m_maskIDList[oldMaskIndex].count() == 0)
				SAFE_DELETES(pData3D_Mask[oldMaskIndex]);
		}
		else
		{
			m_voxelCount[del_info->uid] = 0;
			m_boundingBoxROI[del_info->uid].reset(true);
			m_useMaskInfoBit &= ~(1 << del_info->uid);

			for (auto eraseMask = m_maskIDList[newMaskIndex].begin(); eraseMask != m_maskIDList[newMaskIndex].end(); ++eraseMask)
			{
				if (*eraseMask == (newMaskIndex == 0 ? (del_info->mask_id) : (del_info->mask_id2)))
				{

#ifdef SEGMENTATION_COLOR_CODING
					for (auto it = m_vecAIResultData.begin(); it != m_vecAIResultData.end(); ++it)
					{
						if (it->first == (del_info)->uid)
						{
							m_vecAIResultData.erase(it);
							break;
						}
					}
					for (auto it = m_vecAIOutset.begin(); it != m_vecAIOutset.end(); ++it)
					{
						if (it->first == (del_info)->uid)
						{
							m_vecAIOutset.erase(it);
							break;
						}
					}
#endif
					m_maskIDList[newMaskIndex].erase(eraseMask);
					break;
				}
			}
		}

		SAFE_DELETE(del_info);

	}

	for (int i = 1; i < 4; i++)
	{
		if (m_maskIDList[i].count() == 0)
			SAFE_DELETES(pData3D_Mask[i]);
	}


	setCurrentMaskIndex(0);
	MaskInfo* info = getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	setMultiSelectUID(vecSelect);
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView(true);
	m_currentMaskInfoIndex = 0;

	return true;
}

void VOLUME_DATA::deleteMaskInfoAndData(MaskInfo* pDeleteInfo)
{
	int deleteMaskByteIndex = GetMaskByteIndex(pDeleteInfo);
	mask deleteMaskBitFlag = GetMaskBitFlag(pDeleteInfo);

	clearVoxelCountAndBoundingBox(pDeleteInfo->uid);

	setUsedMaskUIDBitOff(pDeleteInfo->uid);

	deleteMaskBitFlagFromList(deleteMaskByteIndex, deleteMaskBitFlag);

	if (deleteMaskByteIndex > 0)
	{
		if (m_maskIDList[deleteMaskByteIndex].isEmpty())
		{
			SAFE_DELETES(pData3D_Mask[deleteMaskByteIndex]);
		}
	}
}

bool VOLUME_DATA::takMaskInfo(muint32 index, MaskInfo& info /*out*/)
{
	if (m_vecMaskInfoList.size() == 1)
		return false;

	int id = 0;
	for (auto mask = m_vecMaskInfoList.begin(); mask != m_vecMaskInfoList.end(); ++mask)
	{
		if (index == id)
		{
			m_useMaskInfoBit &= ~(1 << (*mask)->uid);
			info = *(*mask);
			SAFE_DELETE((*mask));
			m_vecMaskInfoList.erase(mask);

			return true;
		}
		id++;
	}

	m_currentMaskInfoIndex = 0;
	return false;
}

void VOLUME_DATA::moveMaskInfoAndData(MaskInfo* pFrom, MaskInfo* pTo)
{
	int fromMaskByteIndex = GetMaskByteIndex(pFrom);
	mask fromMaskBitFlag = GetMaskBitFlag(pFrom);
	int toMaskByteIndex = GetMaskByteIndex(pTo);
	mask toMaskBitFlag = GetMaskBitFlag(pTo);

	moveMaskInfo_ex(pFrom, pTo);

	moveMaskBitData(fromMaskByteIndex, toMaskByteIndex, fromMaskBitFlag, toMaskBitFlag);

	deleteMaskBitFlagFromList(fromMaskByteIndex, fromMaskBitFlag);

	setUsedMaskUIDBitOff(pFrom->uid);

	WIN_MANAGER->ChangeUID(pFrom->uid, pTo->uid);
#ifdef USE_UNIT_TEST
	changeUID(pFrom->uid, pTo->uid);
#endif

	if (m_maskIDList[fromMaskByteIndex].isEmpty())
	{
		SAFE_DELETES(pData3D_Mask[fromMaskByteIndex]);
}

	pFrom->uid = pTo->uid;
	pFrom->mask_id = pTo->mask_id;
	pFrom->mask_id2 = pTo->mask_id2;
}

bool VOLUME_DATA::insertMaskInfo(muint32 index, const MaskInfo& info)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
		return false;

	if (index >= MASK_MAX)
		return false;

	int id = 0;
	MaskInfo* newInfo = new MaskInfo(info);
	setUsedMaskUIDBitOn(newInfo->uid);
	//m_useMaskInfoBit |= (1 << newInfo->uid);

	int MaskIndex = 0;

	if (newInfo->uid >= MASK_SECOND_MAX)
	{
		MaskIndex = (newInfo->uid - MASK_SECOND_MAX) / 8 + 1;
		m_maskIDList[MaskIndex].append(newInfo->mask_id2);
	}
	else
	{
		m_maskIDList[MaskIndex].append(newInfo->mask_id);
	}

	for (auto mask = m_vecMaskInfoList.begin(); mask != m_vecMaskInfoList.end(); ++mask)
	{
		if (index == id)
		{
			m_vecMaskInfoList.insert(mask, newInfo);
			return true;
		}
		id++;
	}
	m_vecMaskInfoList.push_back(newInfo);
	m_currentMaskInfoIndex = index;

	return true;
}

bool VOLUME_DATA::insertNewMaskInfo(muint32 index)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
	{
		return false;
	}

	m_currentMaskInfoIndex = m_vecMaskInfoList.size();

	static int nameIndex = m_currentMaskInfoIndex + 1;

	if (m_bCreate)
	{
		nameIndex = m_currentMaskInfoIndex + 1;
	}

	QString layerName = makeUniqueMaskNameWithSuffixNumber(QString("Layer "), nameIndex++);
	return insertNewMaskInfoAndInitData(index, layerName);
}

bool VOLUME_DATA::resetMaskInfo(muint32 index, const MaskInfo& info)
{
	MaskInfo* pTargetMaskInfo = getMaskInfoByIndex(index);
	if (pTargetMaskInfo == nullptr)
	{
		return false;
	}

	info.Copy(pTargetMaskInfo);
	setUsedMaskUIDBitOn(pTargetMaskInfo->uid);

	addMaskBitFlagToList(pTargetMaskInfo);

	return true;
}

bool VOLUME_DATA::moveMaskInfo(muint32 source_index, muint32 dest_index)
{
	muint32 cnt = getMaskInfoListCnt();
	if (cnt == 1 ||
		source_index == dest_index ||
		source_index >= cnt ||
		dest_index >= cnt)
		return false;

	MaskInfo info;
	if (!takMaskInfo(source_index, info))
		return false;

	insertMaskInfo(dest_index, info);

	return true;
}

bool VOLUME_DATA::moveMaskBitData(int oldMaskByteIndex, int newMaskByteIndex, mask oldMaskBitFlag, mask newMaskBitFlag)
{
	muint32 length = this->getVolumeDataLength();

	if (!pData3D_Mask[oldMaskByteIndex])
		return true;

	CheckDataPoint(newMaskByteIndex);

	for (int n = 0; n < length; ++n)
	{
		if (pData3D_Mask[oldMaskByteIndex][n] & oldMaskBitFlag)
		{
			pData3D_Mask[oldMaskByteIndex][n] &= ~oldMaskBitFlag;
			pData3D_Mask[newMaskByteIndex][n] |= newMaskBitFlag;
		}
	}

	return true;
}

bool VOLUME_DATA::moveMaskInfo_ex(MaskInfo* pFrom, MaskInfo* pTo)
{
	m_voxelCount[pTo->uid] = m_voxelCount[pFrom->uid];
	m_boundingBoxROI[pTo->uid] = m_boundingBoxROI[pFrom->uid];

	clearVoxelCountAndBoundingBox(pFrom->uid);

	return true;
}

void VOLUME_DATA::clearMaskData(mask m_, int Maskindex)
{
	if (Maskindex != 0 && (NULL == pData3D_Mask[Maskindex]))
	{
		pData3D_Mask[Maskindex] = new mask[dataLenth];

		memset(pData3D_Mask[Maskindex], 0, dataLenth);
	}

	if (pData3D_Mask[Maskindex])
	{
		muint32 length = this->getVolumeDataLength();
		for (int n = 0; n < length; ++n)
			pData3D_Mask[Maskindex][n] &= ~m_;
	}
}

void VOLUME_DATA::clearMaskDataByInfo(MaskInfo* pInfo)
{
	int maskByteIndex = GetMaskByteIndex(pInfo);
	mask maskBitFlag = GetMaskBitFlag(pInfo);

	clearMaskData(maskBitFlag, maskByteIndex);
}

bool VOLUME_DATA::createMaskInfoFromCopyByIndex(muint32 layerIndex)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
		return false;

	if (layerIndex >= m_vecMaskInfoList.size())
		return false;

	MaskInfo* info = getMaskInfoByIndex(layerIndex);

	return createMaskInfoFromCopyByInfo(info);
}

bool VOLUME_DATA::createMaskInfoFromCopyByUID(int UID)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
		return false;

	MaskInfo* info = getMaskInfoByUID(UID);
	if (info == nullptr)
	{
		return false;
	}

	return createMaskInfoFromCopyByInfo(info);
}

bool VOLUME_DATA::createMaskInfoFromCopyByInfo(const MaskInfo* info)
{
	mask mid = info->uid >= MASK_SECOND_MAX ? info->mask_id2 : info->mask_id;
	int Maskindex = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	if (createMaskInfo(true) == false)
	{
		return false;
	}

	MaskInfo* newinfo = getCurrentMaskInfo();
	newinfo->color = info->color;
	newinfo->bIsUseCredit = info->bIsUseCredit;

	// jhc[2020.10.29] mask name 중복 할당 방지
//	setMaskName(QString("%1 copy").arg(getMaskName(info->uid, true)), newinfo->uid, true);
	setMaskName(makeUniqueMaskNameWithSuffixNumber(QString("%1 copy").arg(getMaskName(info->uid, true))), newinfo->uid, true);

	mask newmid = newinfo->uid >= MASK_SECOND_MAX ? newinfo->mask_id2 : newinfo->mask_id;

	int newMaskindex = newinfo->uid >= MASK_SECOND_MAX ? (newinfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	muint32 length = this->getVolumeDataLength();

	if (NULL == pData3D_Mask[Maskindex])
	{
		pData3D_Mask[Maskindex] = new mask[length];

		memset(pData3D_Mask, 0, sizeof(mask) * length);
	}

	int count = 0;
	for (int n = 0; n < length; ++n)
	{
		if (pData3D_Mask[Maskindex][n] & mid)
		{
			pData3D_Mask[newMaskindex][n] |= newmid;
			count++;
		}
	}

	printf_s("VOLUME_DATA::createMaskInfoFromCopyByIndex >> [%d %d] [%d %d]\n", count, length, mid, newmid);

	m_voxelCount[newinfo->uid] = m_voxelCount[info->uid];
	m_boundingBoxROI[newinfo->uid] = m_boundingBoxROI[info->uid];
	WIN_MANAGER->applyVoxelToUI(newinfo->uid);
	return true;
}

bool VOLUME_DATA::createMaskInfoFromCopy_2(muint32 layerIndex)
{
	if (m_vecMaskInfoList.size() >= MASK_MAX)
		return false;

	if (layerIndex >= m_vecMaskInfoList.size())
		return false;

	MaskInfo* originInfo = getMaskInfo(layerIndex);
	mask mid = originInfo->uid >= MASK_SECOND_MAX ? originInfo->mask_id2 : originInfo->mask_id;
	int Maskindex = originInfo->uid >= MASK_SECOND_MAX ? (originInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	bool isCopy = true;
	createMaskInfo(isCopy);

	MaskInfo* newinfo = getCurrentMaskInfo();
	newinfo->color = originInfo->color;
	newinfo->bIsUseCredit = originInfo->bIsUseCredit;

	return true;
}

void VOLUME_DATA::setCurrentMaskIndex(muint32 index)
{
	if (index >= m_vecMaskInfoList.size())
	{
		return;
	}

	m_currentMaskInfoIndex = index;
}

mip::AABB VOLUME_DATA::getAABB()
{
	mip::AABB box;
	box.set(0, 0, 0, mip::VECTOR3(getSizeX(), getSizeY(), getSizeZ()));
	return box;
}

mip::AABB VOLUME_DATA::getBoundingBoxAABB()
{
	mip::AABB box;
	BoundingBoxI boundingBox = getBoundingBox();
	box.min.set(getSpaceX() * boundingBox.getMinX() - getSizeX() * 0.5f, getSpaceY() * boundingBox.getMinY() - getSizeY() * 0.5f, getSpaceZ() * boundingBox.getMinZ() - getSizeZ() * 0.5f);
	box.max.set(getSpaceX() * (boundingBox.getMaxX()) - getSizeX() * 0.5f, getSpaceY() * (boundingBox.getMaxY()) - getSizeY() * 0.5f, getSpaceZ() * (boundingBox.getMaxZ()) - getSizeZ() * 0.5f);
	return box;
}

mip::AABB VOLUME_DATA::getBoundingBox3DAABB()
{
	mip::AABB box;
	BoundingBoxI boundingBox = getBoundingBox3D();
	box.min.set(getSpaceX() * boundingBox.getMinX() - getSizeX() * 0.5f
		, getSpaceY() * boundingBox.getMinY() - getSizeY() * 0.5f
		, getSpaceZ() * boundingBox.getMinZ() - getSizeZ() * 0.5f);

	box.max.set(getSpaceX() * (boundingBox.getMaxX()) - getSizeX() * 0.5f
		, getSpaceY() * (boundingBox.getMaxY()) - getSizeY() * 0.5f
		, getSpaceZ() * (boundingBox.getMaxZ()) - getSizeZ() * 0.5f);
	return box;
}

mip::AABB VOLUME_DATA::getAABB(BoundingBoxI& box)
{
	mip::AABB rbox;
	rbox.min.set(getSpaceX() * box.getMinX() - getSizeX() * 0.5f, getSpaceY() * box.getMinY() - getSizeY() * 0.5f, getSpaceZ() * box.getMinZ() - getSizeZ() * 0.5f);
	rbox.max.set(getSpaceX() * box.getMaxX() - getSizeX() * 0.5f, getSpaceY() * box.getMaxY() - getSizeY() * 0.5f, getSpaceZ() * box.getMaxZ() - getSizeZ() * 0.5f);
	return rbox;
}

void VOLUME_DATA::init3DPlanes()
{
	WIN_MANAGER->resetLowSpecTextures();

	mip::AABB box = getBoundingBox3DAABB();
	int cx, cy, cz;

	cx = getCX();
	cy = getCY();
	cz = getCZ();

	float centrX, centrY, centrZ;
	centrX = box.min.x + getSpaceX() * (cx / 2);
	centrY = box.min.y + getSpaceY() * (cy / 2);
	centrZ = box.min.z + getSpaceZ() * (cz / 2);

#if 0
	pointsXPlane[0].set(0, box.min.y, box.min.z);
	pointsXPlane[1].set(0, box.min.y, box.max.z);
	pointsXPlane[2].set(0, box.max.y, box.max.z);
	pointsXPlane[3].set(0, box.max.y, box.min.z);

	pointsZPlane[0].set(box.min.x, box.min.y, 0);
	pointsZPlane[1].set(box.min.x, box.max.y, 0);
	pointsZPlane[2].set(box.max.x, box.max.y, 0);
	pointsZPlane[3].set(box.max.x, box.min.y, 0);

	pointsYPlane[0].set(box.min.x, 0, box.min.z);
	pointsYPlane[1].set(box.min.x, 0, box.max.z);
	pointsYPlane[2].set(box.max.x, 0, box.max.z);
	pointsYPlane[3].set(box.max.x, 0, box.min.z);

	saggitalPPlane[0].set(0, box.min.y, box.min.z);
	saggitalPPlane[1].set(0, box.min.y, box.max.z);
	saggitalPPlane[2].set(0, box.max.y, box.max.z);
	saggitalPPlane[3].set(0, box.max.y, box.min.z);

	//axialPPlane[0].set(box.min.x, box.min.y, 0);
	//axialPPlane[1].set(box.min.x, box.max.y, 0);
	//axialPPlane[2].set(box.max.x, box.max.y, 0);
	//axialPPlane[3].set(box.max.x, box.min.y, 0);

	// rotate x axis 180
	axialPPlane[0].set(box.min.x, box.max.y, 0);
	axialPPlane[1].set(box.min.x, box.min.y, 0);
	axialPPlane[2].set(box.max.x, box.min.y, 0);
	axialPPlane[3].set(box.max.x, box.max.y, 0);

	coronalPPlane[0].set(box.min.x, 0, box.min.z);
	coronalPPlane[1].set(box.min.x, 0, box.max.z);
	coronalPPlane[2].set(box.max.x, 0, box.max.z);
	coronalPPlane[3].set(box.max.x, 0, box.min.z);

	planeVertex[WT_AXIAL][0].set(box.min.x, box.max.y, 0);
	planeVertex[WT_AXIAL][1].set(box.min.x, box.min.y, 0);
	planeVertex[WT_AXIAL][2].set(box.max.x, box.min.y, 0);
	planeVertex[WT_AXIAL][3].set(box.max.x, box.max.y, 0);

	planeVertex[WT_CORONAL][0].set(box.min.x, 0, box.min.z);
	planeVertex[WT_CORONAL][1].set(box.min.x, 0, box.max.z);
	planeVertex[WT_CORONAL][2].set(box.max.x, 0, box.max.z);
	planeVertex[WT_CORONAL][3].set(box.max.x, 0, box.min.z);

	planeVertex[WT_SAGGITAL][0].set(0, box.min.y, box.min.z);
	planeVertex[WT_SAGGITAL][1].set(0, box.min.y, box.max.z);
	planeVertex[WT_SAGGITAL][2].set(0, box.max.y, box.max.z);
	planeVertex[WT_SAGGITAL][3].set(0, box.max.y, box.min.z);
#else
	pointsXPlane[0].set(centrX, box.min.y, box.min.z);
	pointsXPlane[1].set(centrX, box.min.y, box.max.z);
	pointsXPlane[2].set(centrX, box.max.y, box.max.z);
	pointsXPlane[3].set(centrX, box.max.y, box.min.z);

	pointsZPlane[0].set(box.min.x, box.min.y, centrZ);
	pointsZPlane[1].set(box.min.x, box.max.y, centrZ);
	pointsZPlane[2].set(box.max.x, box.max.y, centrZ);
	pointsZPlane[3].set(box.max.x, box.min.y, centrZ);

	pointsYPlane[0].set(box.min.x, centrY, box.min.z);
	pointsYPlane[1].set(box.min.x, centrY, box.max.z);
	pointsYPlane[2].set(box.max.x, centrY, box.max.z);
	pointsYPlane[3].set(box.max.x, centrY, box.min.z);

	saggitalPPlane[0].set(centrX, box.min.y, box.min.z);
	saggitalPPlane[1].set(centrX, box.min.y, box.max.z);
	saggitalPPlane[2].set(centrX, box.max.y, box.max.z);
	saggitalPPlane[3].set(centrX, box.max.y, box.min.z);

	//axialPPlane[0].set(box.min.x, box.min.y, 0);
	//axialPPlane[1].set(box.min.x, box.max.y, 0);
	//axialPPlane[2].set(box.max.x, box.max.y, 0);
	//axialPPlane[3].set(box.max.x, box.min.y, 0);

	// rotate x axis 180
	axialPPlane[0].set(box.min.x, box.max.y, centrZ);
	axialPPlane[1].set(box.min.x, box.min.y, centrZ);
	axialPPlane[2].set(box.max.x, box.min.y, centrZ);
	axialPPlane[3].set(box.max.x, box.max.y, centrZ);

	coronalPPlane[0].set(box.min.x, centrY, box.min.z);
	coronalPPlane[1].set(box.min.x, centrY, box.max.z);
	coronalPPlane[2].set(box.max.x, centrY, box.max.z);
	coronalPPlane[3].set(box.max.x, centrY, box.min.z);

	planeVertex[WT_AXIAL][0].set(box.min.x, box.max.y, centrZ);
	planeVertex[WT_AXIAL][1].set(box.min.x, box.min.y, centrZ);
	planeVertex[WT_AXIAL][2].set(box.max.x, box.min.y, centrZ);
	planeVertex[WT_AXIAL][3].set(box.max.x, box.max.y, centrZ);

	planeVertex[WT_CORONAL][0].set(box.min.x, centrY, box.min.z);
	planeVertex[WT_CORONAL][1].set(box.min.x, centrY, box.max.z);
	planeVertex[WT_CORONAL][2].set(box.max.x, centrY, box.max.z);
	planeVertex[WT_CORONAL][3].set(box.max.x, centrY, box.min.z);

	planeVertex[WT_SAGITTAL][0].set(centrX, box.min.y, box.min.z);
	planeVertex[WT_SAGITTAL][1].set(centrX, box.min.y, box.max.z);
	planeVertex[WT_SAGITTAL][2].set(centrX, box.max.y, box.max.z);
	planeVertex[WT_SAGITTAL][3].set(centrX, box.max.y, box.min.z);

	axialPPlane_Base[0].set(box.min.x, box.max.y, centrZ);
	axialPPlane_Base[1].set(box.min.x, box.min.y, centrZ);
	axialPPlane_Base[2].set(box.max.x, box.min.y, centrZ);
	axialPPlane_Base[3].set(box.max.x, box.max.y, centrZ);

	saggitalPPlane_Base[0].set(centrX, box.min.y, box.min.z);
	saggitalPPlane_Base[1].set(centrX, box.min.y, box.max.z);
	saggitalPPlane_Base[2].set(centrX, box.max.y, box.max.z);
	saggitalPPlane_Base[3].set(centrX, box.max.y, box.min.z);

	coronalPPlane_Base[0].set(box.min.x, centrY, box.min.z);
	coronalPPlane_Base[1].set(box.min.x, centrY, box.max.z);
	coronalPPlane_Base[2].set(box.max.x, centrY, box.max.z);
	coronalPPlane_Base[3].set(box.max.x, centrY, box.min.z);

#endif
}

bool VOLUME_DATA::getAnal3DPlanes(WINDOW_TYPE type, mip::VECTOR3* point, bool init)
{
	if (init)
	{
		mip::AABB box = getBoundingBox3DAABB();

		int cx, cy, cz;

		cx = getCX();
		cy = getCY();
		cz = getCZ();

		float centrX, centrY, centrZ;
		centrX = box.min.x + getSpaceX() * (cx / 2);
		centrY = box.min.y + getSpaceY() * (cy / 2);
		centrZ = box.min.z + getSpaceZ() * (cz / 2);

		switch (type)
		{
		case WT_AXIAL:
			point[0].set(box.min.x, box.max.y, centrZ);
			point[1].set(box.min.x, box.min.y, centrZ);
			point[2].set(box.max.x, box.min.y, centrZ);
			point[3].set(box.max.x, box.max.y, centrZ);
			for (int i = 0; i < 4; i++)
				axialInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
		case WT_CORONAL:
			point[0].set(box.min.x, centrY, box.min.z);
			point[1].set(box.min.x, centrY, box.max.z);
			point[2].set(box.max.x, centrY, box.max.z);
			point[3].set(box.max.x, centrY, box.min.z);
			for (int i = 0; i < 4; i++)
				coronalInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
		case WT_SAGITTAL:
			point[0].set(centrX, box.min.y, box.min.z);
			point[1].set(centrX, box.min.y, box.max.z);
			point[2].set(centrX, box.max.y, box.max.z);
			point[3].set(centrX, box.max.y, box.min.z);
			for (int i = 0; i < 4; i++)
				saggitalInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
		default:
			return false;
		}
	}
	else
	{
		switch (type)
		{
		case WT_AXIAL:
			for (int i = 0; i < 4; i++)
				point[i].set(axialInitPlane[i].x, axialInitPlane[i].y, axialInitPlane[i].z);
			break;
		case WT_CORONAL:
			for (int i = 0; i < 4; i++)
				point[i].set(coronalInitPlane[i].x, coronalInitPlane[i].y, coronalInitPlane[i].z);
			break;
		case WT_SAGITTAL:
			for (int i = 0; i < 4; i++)
				point[i].set(saggitalInitPlane[i].x, saggitalInitPlane[i].y, saggitalInitPlane[i].z);
			break;
		default:
			return false;
		}
	}
	return true;
}

std::vector<mip::VECTOR3> VOLUME_DATA::getAnal3DPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, bool init, bool _getInit)
{
	point.clear();

	mip::AABB box = getBoundingBox3DAABB();

	int cx, cy, cz;

	cx = getCX();
	cy = getCY();
	cz = getCZ();

	float centrX, centrY, centrZ;
	centrX = box.min.x + getSpaceX() * (cx / 2);
	centrY = box.min.y + getSpaceY() * (cy / 2);
	centrZ = box.min.z + getSpaceZ() * (cz / 2);

	if (init)
	{
		switch (type)
		{
		case WT_AXIAL:
			point.push_back(mip::VECTOR3(box.min.x, box.max.y, centrZ));
			point.push_back(mip::VECTOR3(box.min.x, box.min.y, centrZ));
			point.push_back(mip::VECTOR3(box.max.x, box.min.y, centrZ));
			point.push_back(mip::VECTOR3(box.max.x, box.max.y, centrZ));
			for (int i = 0; i < 4; i++)
				axialInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
		case WT_CORONAL:
			point.push_back(mip::VECTOR3(box.min.x, centrY, box.min.z));
			point.push_back(mip::VECTOR3(box.min.x, centrY, box.max.z));
			point.push_back(mip::VECTOR3(box.max.x, centrY, box.max.z));
			point.push_back(mip::VECTOR3(box.max.x, centrY, box.min.z));
			for (int i = 0; i < 4; i++)
				coronalInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
		case WT_SAGITTAL:
			point.push_back(mip::VECTOR3(centrX, box.min.y, box.min.z));
			point.push_back(mip::VECTOR3(centrX, box.min.y, box.max.z));
			point.push_back(mip::VECTOR3(centrX, box.max.y, box.max.z));
			point.push_back(mip::VECTOR3(centrX, box.max.y, box.min.z));
			for (int i = 0; i < 4; i++)
				saggitalInitPlane[i].set(point[i].x, point[i].y, point[i].z);
			break;
			//	default:
				//	return false;
		}
	}
	else
	{
		switch (type)
		{
		case WT_AXIAL:

			if (_getInit)
			{
				point.push_back(mip::VECTOR3(box.min.x, box.max.y, centrZ));
				point.push_back(mip::VECTOR3(box.min.x, box.min.y, centrZ));
				point.push_back(mip::VECTOR3(box.max.x, box.min.y, centrZ));
				point.push_back(mip::VECTOR3(box.max.x, box.max.y, centrZ));
			}
			else
			{
				for (int i = 0; i < 4; i++)
					point.push_back(mip::VECTOR3(axialInitPlane[i].x, axialInitPlane[i].y, axialInitPlane[i].z));
			}
			break;
		case WT_CORONAL:
			if (_getInit)
			{
				point.push_back(mip::VECTOR3(box.min.x, centrY, box.min.z));
				point.push_back(mip::VECTOR3(box.min.x, centrY, box.max.z));
				point.push_back(mip::VECTOR3(box.max.x, centrY, box.max.z));
				point.push_back(mip::VECTOR3(box.max.x, centrY, box.min.z));
			}
			else
			{
				for (int i = 0; i < 4; i++)
					point.push_back(mip::VECTOR3(coronalInitPlane[i].x, coronalInitPlane[i].y, coronalInitPlane[i].z));
			}
			break;
		case WT_SAGITTAL:
			if (_getInit)
			{
				point.push_back(mip::VECTOR3(centrX, box.min.y, box.min.z));
				point.push_back(mip::VECTOR3(centrX, box.min.y, box.max.z));
				point.push_back(mip::VECTOR3(centrX, box.max.y, box.max.z));
				point.push_back(mip::VECTOR3(centrX, box.max.y, box.min.z));
			}
			else
			{
				for (int i = 0; i < 4; i++)
					point.push_back(mip::VECTOR3(saggitalInitPlane[i].x, saggitalInitPlane[i].y, saggitalInitPlane[i].z));
			}
			break;
			//	default:
			//		return false;
		}
	}
	return point;
}

bool VOLUME_DATA::GetZeroBasePlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point)
{
	point.clear();
	point.resize(4);

	for (int i = 0; i < 4; i++)
	{
		if (type == WT_SAGITTAL)
		{
			point[i] = saggitalPPlane_Base[i];
		}
		else if (type == WT_CORONAL)
		{
			point[i] = coronalPPlane_Base[i];
		}
		else if (type == WT_AXIAL)
		{
			point[i] = axialPPlane_Base[i];
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool VOLUME_DATA::checkRotatePlanes(WINDOW_TYPE type, bool isSlider)
{
	QVector<mip::VECTOR3> initPlanes;
	mip::VECTOR3* checkPlanes = NULL;

	mip::AABB box = getBoundingBox3DAABB();

	int cx, cy, cz;

	cx = getCX();
	cy = getCY();
	cz = getCZ();

	float centrX, centrY, centrZ;
	centrX = box.min.x + getSpaceX() * (cx / 2);
	centrY = box.min.y + getSpaceY() * (cy / 2);
	centrZ = box.min.z + getSpaceZ() * (cz / 2);

	initPlanes.clear();

	switch (type)
	{
	case WT_AXIAL:
		initPlanes.push_back(mip::VECTOR3(box.min.x, box.max.y, centrZ));
		initPlanes.push_back(mip::VECTOR3(box.min.x, box.min.y, centrZ));
		initPlanes.push_back(mip::VECTOR3(box.max.x, box.min.y, centrZ));
		initPlanes.push_back(mip::VECTOR3(box.max.x, box.max.y, centrZ));

		if (isSlider)
			checkPlanes = axialInitPlane;
		else
			checkPlanes = axialPPlane;
		for (int i = 0; i < 4; i++)
		{
			if (!isSlider)
				initPlanes[i].z = checkPlanes[i].z;

			if (initPlanes[i] != checkPlanes[i])
				return true;
		}
		break;
	case WT_CORONAL:
		initPlanes.push_back(mip::VECTOR3(box.min.x, centrY, box.min.z));
		initPlanes.push_back(mip::VECTOR3(box.min.x, centrY, box.max.z));
		initPlanes.push_back(mip::VECTOR3(box.max.x, centrY, box.max.z));
		initPlanes.push_back(mip::VECTOR3(box.max.x, centrY, box.min.z));

		if (isSlider)
			checkPlanes = coronalInitPlane;
		else
			checkPlanes = coronalPPlane;
		for (int i = 0; i < 4; i++)
		{
			if (!isSlider)
				initPlanes[i].y = checkPlanes[i].y;

			if (initPlanes[i] != checkPlanes[i])
				return true;
		}
		break;
	case WT_SAGITTAL:
		initPlanes.push_back(mip::VECTOR3(centrX, box.min.y, box.min.z));
		initPlanes.push_back(mip::VECTOR3(centrX, box.min.y, box.max.z));
		initPlanes.push_back(mip::VECTOR3(centrX, box.max.y, box.max.z));
		initPlanes.push_back(mip::VECTOR3(centrX, box.max.y, box.min.z));
		if (isSlider)
			checkPlanes = saggitalInitPlane;
		else
			checkPlanes = saggitalPPlane;
		for (int i = 0; i < 4; i++)
		{
			if (!isSlider)
				initPlanes[i].x = checkPlanes[i].x;

			if (initPlanes[i] != checkPlanes[i])
				return true;
		}
		break;
	default:
		break;
	}

	return false;
}

void VOLUME_DATA::setAnal3DPlanes(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_AXIAL:
		for (int i = 0; i < 4; i++)
			axialInitPlane[i].set(axialPPlane[i].x, axialPPlane[i].y, axialPPlane[i].z);
		break;
	case WT_CORONAL:
		for (int i = 0; i < 4; i++)
			coronalInitPlane[i].set(coronalPPlane[i].x, coronalPPlane[i].y, coronalPPlane[i].z);
		break;
	case WT_SAGITTAL:
		for (int i = 0; i < 4; i++)
			saggitalInitPlane[i].set(saggitalPPlane[i].x, saggitalPPlane[i].y, saggitalPPlane[i].z);
		break;
	}
}

void VOLUME_DATA::setImgOrientation(bool isX, mip::VECTOR3 val, bool init)
{
	mip::VECTOR3* dt = isX ? &xAxis : &yAxis;

	if (!init)
		dt->set(val);
	else
		dt->set(0, 1, 0);
}

bool VOLUME_DATA::getImgOrientation(bool isX, float* dt)
{
	if (nullptr == dt)
		return false;

	mip::VECTOR3* val = isX ? &xAxis : &yAxis;

	dt[0] = val->x;
	dt[1] = val->y;
	dt[2] = val->z;

	return true;
}

bool VOLUME_DATA::getPlaneLine(WINDOW_TYPE type, std::vector<mip::VECTOR3>& list)
{
	switch (type)
	{
	case WINDOW_TYPE::WT_CORONAL:
		list.push_back(pointsYPlane[0]);
		list.push_back(pointsYPlane[1]);
		list.push_back(pointsYPlane[1]);
		list.push_back(pointsYPlane[2]);
		list.push_back(pointsYPlane[2]);
		list.push_back(pointsYPlane[3]);
		list.push_back(pointsYPlane[3]);
		list.push_back(pointsYPlane[0]);
		break;
	case WINDOW_TYPE::WT_SAGITTAL:
		list.push_back(pointsXPlane[0]);
		list.push_back(pointsXPlane[1]);
		list.push_back(pointsXPlane[1]);
		list.push_back(pointsXPlane[2]);
		list.push_back(pointsXPlane[2]);
		list.push_back(pointsXPlane[3]);
		list.push_back(pointsXPlane[3]);
		list.push_back(pointsXPlane[0]);
		break;
	case WINDOW_TYPE::WT_AXIAL:
	default:
		list.push_back(pointsZPlane[0]);
		list.push_back(pointsZPlane[1]);
		list.push_back(pointsZPlane[1]);
		list.push_back(pointsZPlane[2]);
		list.push_back(pointsZPlane[2]);
		list.push_back(pointsZPlane[3]);
		list.push_back(pointsZPlane[3]);
		list.push_back(pointsZPlane[0]);
		break;
	}

	return true;
}

muint32 VOLUME_DATA::getPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, mip::MATRIX44* trasform)
{
	mip::VECTOR3 v[4];
	muint32 count = 0;

	switch (type)
	{
	case WINDOW_TYPE::WT_CORONAL:
		memcpy(&v, &pointsYPlane[0], sizeof(mip::VECTOR3) * 4);
		break;
	case WINDOW_TYPE::WT_SAGITTAL:
		memcpy(&v, &pointsXPlane[0], sizeof(mip::VECTOR3) * 4);
		break;
	case WINDOW_TYPE::WT_AXIAL:
	default:
		memcpy(&v, &pointsZPlane[0], sizeof(mip::VECTOR3) * 4);
		break;
	}

	if (trasform)
	{
		for (int n = 0; n < 4; n++)
		{
			v[n].transform(*trasform);
		}
	}

	for (int n = 0; n < 4; n++)
	{
		point.push_back(v[n]);
		count++;
	}

	return count;
}

bool VOLUME_DATA::getPlaneSurfaceFromPlane(const std::vector<mip::VECTOR3>& v, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord)
{
	if (v.size() < 4)
		return false;

#ifdef USE_RIGHT_HAND
	point.push_back(v[0]);
	point.push_back(v[1]);
	point.push_back(v[2]);
	point.push_back(v[2]);
	point.push_back(v[3]);
	point.push_back(v[0]);

	texcoord.push_back(point2texcoord(v[0]));
	texcoord.push_back(point2texcoord(v[1]));
	texcoord.push_back(point2texcoord(v[2]));
	texcoord.push_back(point2texcoord(v[2]));
	texcoord.push_back(point2texcoord(v[3]));
	texcoord.push_back(point2texcoord(v[0]));
#else
	point.push_back(v[0]);
	point.push_back(v[1]);
	point.push_back(v[3]);
	point.push_back(v[3]);
	point.push_back(v[1]);
	point.push_back(v[2]);

	texcoord.push_back(point2texcoord(v[0]));
	texcoord.push_back(point2texcoord(v[1]));
	texcoord.push_back(point2texcoord(v[3]));
	texcoord.push_back(point2texcoord(v[3]));
	texcoord.push_back(point2texcoord(v[1]));
	texcoord.push_back(point2texcoord(v[2]));
#endif
	return true;
}

bool VOLUME_DATA::getPlaneSurfaceFromPlaneList(const std::vector<mip::VECTOR3>& v, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, float depth)
{
	if (v.size() < 4)
		return false;

	mip::VECTOR3 normal = ((v[0] - v[1]) ^ (v[2] - v[1])).normalize();

	std::vector< mip::VECTOR3 > plane_point;

	float space = min(min(spaceX, spaceY), spaceZ);
	float max_depth = depth * 0.5f;
	muint32 slice_count = 0;

	float dvalue = 0.0f;
	if (depth == 0.0f)
	{
		//dvalue = -space;
		//slice_count = 3;
		//max_depth = space;

		dvalue = 0;
		slice_count = 1;
		max_depth = space;
	}
	else
	{
		dvalue = -depth * 0.5f;
		slice_count = qRound(depth / space);
	}

	mip::AABB box3D = getBoundingBox3DAABB();

	for (int n = 0; n < slice_count; n++, dvalue += space)
	{
		if (dvalue > max_depth)
		{
			dvalue = max_depth;
			n = slice_count;
		}

		plane_point.clear();
		mip::VECTOR3 dir = normal * dvalue;

		for (auto vp = v.begin(); vp != v.end(); ++vp)
		{
			plane_point.push_back((*vp) + dir);
		}

		if (plane_point.size() <= 3)
			continue;

		bool res = true;

		for (int i = 0; i < 4; i++)
		{
			if (std::find(texcoord.begin(), texcoord.end(), point2texcoord(plane_point[i])) != texcoord.end())
			{
				res = false;
				break;
			}
		}

		if (res)
		{
			/*for (int i = 0; i < 4; i++)
			{
				res = box3D.checkPoint(plane_point[i]);

				if (res)
					break;
			}
			if(!res)
				continue;*/

#ifdef USE_RIGHT_HAND
			point.push_back(plane_point[0]);
			point.push_back(plane_point[1]);
			point.push_back(plane_point[2]);
			point.push_back(plane_point[2]);
			point.push_back(plane_point[3]);
			point.push_back(plane_point[0]);

			texcoord.push_back(point2texcoord(plane_point[0]));
			texcoord.push_back(point2texcoord(plane_point[1]));
			texcoord.push_back(point2texcoord(plane_point[2]));
			texcoord.push_back(point2texcoord(plane_point[2]));
			texcoord.push_back(point2texcoord(plane_point[3]));
			texcoord.push_back(point2texcoord(plane_point[0]));
#else
			point.push_back(plane_point[0]);
			point.push_back(plane_point[1]);
			point.push_back(plane_point[3]);
			point.push_back(plane_point[3]);
			point.push_back(plane_point[1]);
			point.push_back(plane_point[2]);

			texcoord.push_back(point2texcoord(plane_point[0]));
			texcoord.push_back(point2texcoord(plane_point[1]));
			texcoord.push_back(point2texcoord(plane_point[3]));
			texcoord.push_back(point2texcoord(plane_point[3]));
			texcoord.push_back(point2texcoord(plane_point[1]));
			texcoord.push_back(point2texcoord(plane_point[2]));
#endif
		}
	}
	return true;
}



bool VOLUME_DATA::getPlaneSurfaceFromPlaneList2D(const std::vector<mip::VECTOR3>& v, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, bool flip, float depth)
{
	if (v.size() < 4)
		return false;

#ifdef USE_RIGHT_HAND
	point.push_back(v[0]);
	point.push_back(v[1]);
	point.push_back(v[2]);
	point.push_back(v[2]);
	point.push_back(v[3]);
	point.push_back(v[0]);

	if (flip == false)
	{
		texcoord.push_back(mip::VECTOR2(0, 0));
		texcoord.push_back(mip::VECTOR2(0, 1));
		texcoord.push_back(mip::VECTOR2(1, 1));
		texcoord.push_back(mip::VECTOR2(1, 1));
		texcoord.push_back(mip::VECTOR2(1, 0));
		texcoord.push_back(mip::VECTOR2(0, 0));
	}
	else // point is fliped
	{
		texcoord.push_back(mip::VECTOR2(0, 1));
		texcoord.push_back(mip::VECTOR2(0, 0));
		texcoord.push_back(mip::VECTOR2(1, 0));
		texcoord.push_back(mip::VECTOR2(1, 0));
		texcoord.push_back(mip::VECTOR2(1, 1));
		texcoord.push_back(mip::VECTOR2(0, 1));
	}
#else
	point.push_back(plane_point[0]);
	point.push_back(plane_point[1]);
	point.push_back(plane_point[3]);
	point.push_back(plane_point[3]);
	point.push_back(plane_point[1]);
	point.push_back(plane_point[2]);

	texcoord.push_back(point2texcoord(plane_point[0]));
	texcoord.push_back(point2texcoord(plane_point[1]));
	texcoord.push_back(point2texcoord(plane_point[3]));
	texcoord.push_back(point2texcoord(plane_point[3]));
	texcoord.push_back(point2texcoord(plane_point[1]));
	texcoord.push_back(point2texcoord(plane_point[2]));
#endif

	return true;
}

bool VOLUME_DATA::getAllThickness(float _thick, float dt, WINDOW_TYPE _type, bool isSlider, bool sliderSub)
{
	std::vector<mip::VECTOR3> v;
	mip::VECTOR3 plane_point;

	if (getMPRPPlanes(_type, v, 0) == 0)
		return false;

	float space = min(min(spaceX, spaceY), spaceZ);
	float max_depth = _thick * 0.5f;
	muint32 slice_count = 0;

	float dvalue = 0.0f;
	if (_thick == 0.0f)
	{
		dvalue = 0;
		slice_count = 1;
		max_depth = space;
	}
	else
	{
		dvalue = -_thick * 0.5f;
		slice_count = qRound(_thick / space);
	}

	//	if (!isSlider)
	{
		bool chkDt = dt > 0;

		if (!isSlider)
			dt = chkDt ? 1.0f : -1.0f;
		else
			chkDt = sliderSub;

		if (chkDt && _type != WT_CORONAL)
			dvalue += (space * (slice_count - 1));
		else if (!chkDt && _type == WT_CORONAL)
			dvalue += (space * (slice_count - 1));
	}

	switch (_type)
	{
	case WT_AXIAL:
		dt *= getSpaceZ();
		break;
	case WT_CORONAL:
		dt *= getSpaceY();
		break;
	case WT_SAGITTAL:
		dt *= getSpaceX();
		break;
	}

	mip::AABB box3D = getBoundingBox3DAABB();

	mip::VECTOR3 normal = ((v[0] - v[1]) ^ (v[2] - v[1])).normalize();
	normal *= dt;

	for (int n = 0; n < 4; n++)
	{
		v[n] += normal;
	}

	normal = ((v[0] - v[1]) ^ (v[2] - v[1])).normalize();

	if (dvalue > max_depth)
		dvalue = max_depth;

	mip::VECTOR3 dir = normal * dvalue;
	auto vp = v.begin();
	plane_point = (*vp) + dir;

	//	float _depth = MIP_EPSILON2;
	float _depth = 0;

	switch (_type)
	{
	case WT_AXIAL:
		/*_depth += abs(plane_point.z);
		if (abs(box3D.min.z) < abs(_depth))
			return true;*/
		if (box3D.min.z > plane_point.z)
			return true;
		else if ((box3D.max.z - plane_point.z) < 0)
			return true;
		break;
	case WT_CORONAL:
		/*	_depth += abs(plane_point.y);
			if (abs(box3D.min.y) <= abs(_depth))
				return true;*/
		if (box3D.min.y > plane_point.y)
			return true;
		else if ((box3D.max.y - plane_point.y) < 0)
			return true;
		break;
	case WT_SAGITTAL:
		/*_depth += abs(plane_point.x);
		if (abs(box3D.min.x) <= abs(_depth))
			return true;*/
		if (box3D.min.x > plane_point.x)
			return true;
		else if ((box3D.max.x - plane_point.x) < 0)
			return true;
		break;
	}

	return false;
}

muint32 VOLUME_DATA::getMPRPPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, mip::MATRIX44* trasform)
{
	mip::VECTOR3 v[4];
	muint32 count = 0;

	bool bTAtab = false;

	if (WIN_MANAGER->mainTabType == MAINTAB_TA)
		bTAtab = true;

	switch (type)
	{
	case WT_CORONAL:
		if (bTAtab)
		{
			memcpy(&v, &coronalPPlane_Base[0], sizeof(mip::VECTOR3) * 4);
		}
		else
		{
			memcpy(&v, &coronalPPlane[0], sizeof(mip::VECTOR3) * 4);
		}
		break;
	case WT_SAGITTAL:
		if (bTAtab)
		{
			memcpy(&v, &saggitalPPlane_Base[0], sizeof(mip::VECTOR3) * 4);
		}
		else
		{
			memcpy(&v, &saggitalPPlane[0], sizeof(mip::VECTOR3) * 4);
		}
		break;
	case WT_AXIAL:
	default:
		if (bTAtab)
		{
			memcpy(&v, &axialPPlane_Base[0], sizeof(mip::VECTOR3) * 4);
		}
		else
		{
			memcpy(&v, &axialPPlane[0], sizeof(mip::VECTOR3) * 4);
		}

		if (getCZ() % 2 != 0)
		{
			mip::VECTOR3 base = mip::VECTOR3(0, 0, 0.5) * getSpaceZ();
			for (int n = 0; n < 4; n++)
			{
				v[n] += base;
			}
		}
		break;
	}

	if (trasform)
	{
		for (int n = 0; n < 4; n++)
		{
			v[n].transform(*trasform);
		}
	}

	for (int n = 0; n < 4; n++)
	{
		point.push_back(v[n]);
		count++;
	}

	return count;
}

bool VOLUME_DATA::getMPRPlaneSurface(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, mip::MATRIX44* trasform)
{
	std::vector<mip::VECTOR3> v;

	if (getMPRPPlanes(type, v, trasform) == 0) 
		return false;

	getPlaneSurfaceFromPlane(v, point, texcoord);
	return true;
}

bool VOLUME_DATA::getMPRPlaneSurfaceList(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, mip::MATRIX44* trasform, float depth)
{
	std::vector<mip::VECTOR3> v;

	if (getMPRPPlanes(type, v, trasform) == 0) 
		return false;

	getPlaneSurfaceFromPlaneList(v, point, texcoord, depth);
	return true;
}

bool VOLUME_DATA::getPlaneSurface(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord,
	mip::MATRIX44* trasform, bool reverse)
{
	std::vector<mip::VECTOR3> v;

	if (getPlanes(type, v, trasform) == 0)
		return false;

	getPlaneSurfaceFromPlane(v, point, texcoord);
	return true;
}

bool VOLUME_DATA::getMPRPlaneSurfaceList2D(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, mip::MATRIX44* trasform, float depth)
{
	std::vector<mip::VECTOR3> v;

	if (getMPRPPlanes(type, v, trasform) == 0) 
		return false;

	bool flip = false;
	if (type == WT_AXIAL)
	{
		flip = true;
	}

	getPlaneSurfaceFromPlaneList2D(v, point, texcoord, flip, depth);

	return true;
}

std::vector<mip::VECTOR3> VOLUME_DATA::GetMPRPPlanesByDepth(WINDOW_TYPE type, int depth, mip::MATRIX44* trasform)
{
	std::vector<mip::VECTOR3> resultPlanes;
	resultPlanes.resize(4);

	std::vector<mip::VECTOR3> zeroBasePlanes;
	zeroBasePlanes.resize(4);

	int direction = -1;
	float depth_real = depth;
	float depth_unit = 1.0;
	if (type == WT_AXIAL)
	{
		direction = -1;
		depth_unit = (float)getSpaceZ();
	}
	else if (type == WT_CORONAL)
	{
		direction = -1;
		depth_unit = (float)getSpaceY();
	}
	else if (type == WT_SAGITTAL)
	{
		direction = 1;
		depth_unit = (float)getSpaceX();
	}

	depth_real *= (depth_unit * direction);

	if (GetZeroBasePlanes(type, zeroBasePlanes) == false)
	{
		return resultPlanes;
	}

	if (type == WT_AXIAL)
	{
		if (getCZ() % 2 != 0)
		{
			mip::VECTOR3 base = mip::VECTOR3(0.0f, 0.0f, 0.5f) * getSpaceZ();
			for (int n = 0; n < 4; n++)
			{
				zeroBasePlanes[n] += base;
			}
		}
	}

	mip::VECTOR3 vNormal = ((zeroBasePlanes[0] - zeroBasePlanes[1]) ^ (zeroBasePlanes[2] - zeroBasePlanes[1])).normalize();

	mip::VECTOR3 vDistance = vNormal * depth_real;

	for (int n = 0; n < 4; ++n)
	{
		mip::VECTOR3 v = zeroBasePlanes[n] + vDistance;
		resultPlanes[n] = v;
	}

	if (trasform)
	{
		for (int n = 0; n < 4; n++)
		{
			resultPlanes[n].transform(*trasform);
		}
	}

	return resultPlanes;
}

bool VOLUME_DATA::getSegmentationPlaneSurfaceList2D(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, float depth)
{
	std::vector<mip::VECTOR3> vertexBuffer;
	vertexBuffer.reserve(4);

	vertexBuffer.push_back(planeVertex[type][0]);
	vertexBuffer.push_back(planeVertex[type][1]);
	vertexBuffer.push_back(planeVertex[type][2]);
	vertexBuffer.push_back(planeVertex[type][3]);

	bool flip = false;
	if (type == WT_AXIAL)
	{
		flip = true;
	}

	return getPlaneSurfaceFromPlaneList2D(vertexBuffer, point, texcoord, flip, depth);
}

bool VOLUME_DATA::getCullingPlaneSurface(std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, float size, mip::MATRIX44* trasform)
{
	mip::VECTOR3 v[8];
	mip::AABB box = getBoundingBox3DAABB();

	int cz = getCZ();

	float centrZ = box.min.z + getSpaceZ() * (cz / 2);


	mip::VECTOR3 min = mip::VECTOR3(box.min.x - size, box.min.y - size, centrZ);
	mip::VECTOR3 max = mip::VECTOR3(box.max.x + size, box.max.y + size, centrZ);

	v[0].set(box.min.x, box.min.y, centrZ);
	v[1].set(box.min.x, box.max.y, centrZ);
	v[2].set(box.max.x, box.max.y, centrZ);
	v[3].set(box.max.x, box.min.y, centrZ);
	v[4].set(min.x, min.y, centrZ);
	v[5].set(min.x, max.y, centrZ);
	v[6].set(max.x, max.y, centrZ);
	v[7].set(max.x, min.y, centrZ);

	mip::VECTOR3 m[16];
	m[0] = mip::VECTOR3(v[4].x, v[4].y, centrZ);
	m[1] = mip::VECTOR3(v[4].x, v[0].y, centrZ);
	m[2] = mip::VECTOR3(v[7].x, v[3].y, centrZ);
	m[3] = v[7];

	m[4] = mip::VECTOR3(v[5].x, v[1].y, centrZ);
	m[5] = mip::VECTOR3(v[5].x, v[5].y, centrZ);
	m[6] = mip::VECTOR3(v[6].x, v[5].y, centrZ);
	m[7] = mip::VECTOR3(v[6].x, v[2].y, centrZ);

	m[8] = mip::VECTOR3(v[4].x, v[0].y, centrZ);
	m[9] = mip::VECTOR3(v[4].x, v[1].y, centrZ);
	m[10] = mip::VECTOR3(v[0].x, v[1].y, centrZ);
	m[11] = mip::VECTOR3(v[0].x, v[0].y, centrZ);

	m[12] = mip::VECTOR3(v[3].x, v[3].y, centrZ);
	m[13] = mip::VECTOR3(v[3].x, v[2].y, centrZ);
	m[14] = mip::VECTOR3(v[6].x, v[2].y, centrZ);
	m[15] = mip::VECTOR3(v[6].x, v[3].y, centrZ);


	if (trasform)
	{
		for (int n = 0; n < 16; n++)
		{
			m[n].transform(*trasform);
		}
	}

	point.push_back(m[3]);
	point.push_back(m[1]);
	point.push_back(m[0]);
	point.push_back(m[2]);
	point.push_back(m[1]);
	point.push_back(m[3]);

	texcoord.push_back(mip::VECTOR2(1, 0));
	texcoord.push_back(mip::VECTOR2(0, 0.24f));
	texcoord.push_back(mip::VECTOR2(0, 0));
	texcoord.push_back(mip::VECTOR2(1, 0.24f));
	texcoord.push_back(mip::VECTOR2(0, 0.24f));
	texcoord.push_back(mip::VECTOR2(1, 0));

	point.push_back(m[7]);
	point.push_back(m[5]);
	point.push_back(m[4]);
	point.push_back(m[6]);
	point.push_back(m[5]);
	point.push_back(m[7]);

	texcoord.push_back(mip::VECTOR2(0, 0.24f));
	texcoord.push_back(mip::VECTOR2(1, 0));
	texcoord.push_back(mip::VECTOR2(1, 0.24f));
	texcoord.push_back(mip::VECTOR2(0, 0));
	texcoord.push_back(mip::VECTOR2(1, 0));
	texcoord.push_back(mip::VECTOR2(0, 0.24f));


	point.push_back(m[11]);
	point.push_back(m[9]);
	point.push_back(m[8]);
	point.push_back(m[10]);
	point.push_back(m[9]);
	point.push_back(m[11]);

	texcoord.push_back(mip::VECTOR2(0, 0.01f));
	texcoord.push_back(mip::VECTOR2(0.01f, 0));
	texcoord.push_back(mip::VECTOR2(0.01f, 0.01f));
	texcoord.push_back(mip::VECTOR2(0, 0));
	texcoord.push_back(mip::VECTOR2(0.01f, 0));
	texcoord.push_back(mip::VECTOR2(0, 0.01f));

	point.push_back(m[15]);
	point.push_back(m[13]);
	point.push_back(m[12]);
	point.push_back(m[14]);
	point.push_back(m[13]);
	point.push_back(m[15]);

	texcoord.push_back(mip::VECTOR2(0.01f, 0.01f));
	texcoord.push_back(mip::VECTOR2(0, 0));
	texcoord.push_back(mip::VECTOR2(0, 0.01f));
	texcoord.push_back(mip::VECTOR2(0.01f, 0));
	texcoord.push_back(mip::VECTOR2(0, 0));
	texcoord.push_back(mip::VECTOR2(0.01f, 0.01f));


	return true;
}

mip::VECTOR3 VOLUME_DATA::point2texcoord(const mip::VECTOR3& point)
{
	return mip::VECTOR3(point.x / getSizeX() + 0.5f, point.y / getSizeY() + 0.5f, point.z / getSizeZ() + 0.5f);
}

mip::VECTOR2 VOLUME_DATA::point2texcoord2D(const mip::VECTOR3& point)
{
	return mip::VECTOR2(point.x / getSizeX() + 0.5f, point.y / getSizeY() + 0.5f);
}

void VOLUME_DATA::setDepth(WINDOW_TYPE winType, const int depth)
{
	switch (winType)
	{
	case WT_CORONAL:
		if (coronalDepth != depth)
		{
			setPrevDepth(winType, coronalDepth);
			coronalDepth = depth;
		}
		break;
	case WT_SAGITTAL:
		if (sagittalDepth != depth)
		{
			setPrevDepth(winType, sagittalDepth);
			sagittalDepth = depth;
		}
		break;
	case WT_AXIAL:
	default:
		if (axialDepth != depth)
		{
			setPrevDepth(winType, axialDepth);
			axialDepth = depth;
		}
		break;
	}
}

int  VOLUME_DATA::getDepth(WINDOW_TYPE winType) const
{
	int depth = -1;
	switch (winType)
	{
	case WT_CORONAL:
	{
		depth = coronalDepth;
	}
	break;
	case WT_SAGITTAL:
	{
		depth = sagittalDepth;
	}
	break;
	case WT_AXIAL:
	default:
	{
		depth = axialDepth;
	}
	break;
	}

	return depth;
}

void VOLUME_DATA::setPrevDepth(WINDOW_TYPE winType, const int depth)
{
	switch (winType)
	{
	case WT_CORONAL:
		coronalPrevDepth = depth;
		break;
	case WT_SAGITTAL:
		sagittalPrevDepth = depth;
		break;
	case WT_AXIAL:
	default:
		axialPrevDepth = depth;
		break;
	}
}

int  VOLUME_DATA::getPrevDepth(WINDOW_TYPE winType) const
{
	int depth = -1;
	switch (winType)
	{
	case WT_CORONAL:
	{
		depth = coronalPrevDepth;
	}
	break;
	case WT_SAGITTAL:
	{
		depth = sagittalPrevDepth;
	}
	break;
	case WT_AXIAL:
	default:
	{
		depth = axialPrevDepth;
	}
	break;
	}

	return depth;
}

void VOLUME_DATA::getAllMaskVoxel(bool withBox)
{
	if (withBox) // default : true
	{
		for (int i = 0; i < MASK_MAX; i++) // 0 ~ 29
		{
			m_boundingBoxROI[i].reset(dataCX, dataCY, dataCZ);
			m_boundingBoxROI[i].minX = dataCX - 1;
			m_boundingBoxROI[i].minY = dataCY - 1;
			m_boundingBoxROI[i].minZ = dataCZ - 1;
			m_boundingBoxROI[i].maxX = 0;
			m_boundingBoxROI[i].maxY = 0;
			m_boundingBoxROI[i].maxZ = 0;
		}

		for (int i = 0; i < 2; i++) // seed 
		{
			seedBoundingbox[i].reset(dataCX, dataCY, dataCZ);
			seedBoundingbox[i].minX = dataCX - 1;
			seedBoundingbox[i].minY = dataCY - 1;
			seedBoundingbox[i].minZ = dataCZ - 1;
			seedBoundingbox[i].maxX = 0;
			seedBoundingbox[i].maxY = 0;
			seedBoundingbox[i].maxZ = 0;
		}
	}

	memset(m_voxelCount, 0, sizeof(m_voxelCount));
	memset(m_seedVoxel, 0, sizeof(m_seedVoxel));

	QVector<QPoint> vecMask;

	int nList = getMaskInfoListCnt(); // layer number

	vecMask.clear();
	vecMask.reserve(nList);

	for (int k = 0; k < nList; k++)
	{
		MaskInfo* info = getMaskInfo(k);
		if (info)
		{
			int uid = info->uid;

			// MASK_SECOND_MAX = 6
			mask val = uid < MASK_SECOND_MAX ? info->mask_id : info->mask_id2;

			vecMask.push_back(QPoint(uid, val));
		}
	}

	if (nList != vecMask.count())
	{
		nList = vecMask.count();
	}

	int length = getVolumeDataLength();


	for (int i = 0; i < 4; i++)
	{
		bool res = CheckDataPoint(i, true); // check only

		if (!res)
		{
			continue;
		}

		for (int z = 0; z < dataCZ; z++)
		{
			for (int y = 0; y < dataCY; y++)
			{
				for (int x = 0; x < dataCX; x++)
				{
					int index = z * dataCX * dataCY + y * dataCX + x;

					if (length <= index || pData3D_Mask[i][index] == 0)
						continue;

					if (i == 0) // 
					{
						for (int k = 0; k < 2; k++) // seed 
						{
							mask val = k + 1;

							if (pData3D_Mask[i][index] & val)
							{
								m_seedVoxel[k]++; // seedVoxel[0], seedVoxel[1]

								if (withBox)
								{
									if (x < seedBoundingbox[k].minX) seedBoundingbox[k].minX = x;
									if (x > seedBoundingbox[k].maxX) seedBoundingbox[k].maxX = x;
									if (y < seedBoundingbox[k].minY) seedBoundingbox[k].minY = y;
									if (y > seedBoundingbox[k].maxY) seedBoundingbox[k].maxY = y;
									if (z < seedBoundingbox[k].minZ) seedBoundingbox[k].minZ = z;
									if (z > seedBoundingbox[k].maxZ) seedBoundingbox[k].maxZ = z;
								}
							}
						}
					}

					for (int k = 0; k < nList; k++) // uid, mask_id or mask_id2
					{
						int uid = vecMask.at(k).x();
						int mI = uid >= MASK_SECOND_MAX ? (uid - MASK_SECOND_MAX) / 8 + 1 : 0; // mI : mask index (0 ~ 3)

						if (mI != i) continue; // matching 

						// 0 ~ 3 : mask index
						// position of volume
						if (pData3D_Mask[i][index] & vecMask.at(k).y())
						{
							// m_voxelCount[uid]
							m_voxelCount[vecMask.at(k).x()]++;

							if (withBox)
							{
								if (x < m_boundingBoxROI[uid].minX) m_boundingBoxROI[uid].minX = x;
								if (x > m_boundingBoxROI[uid].maxX) m_boundingBoxROI[uid].maxX = x;
								if (y < m_boundingBoxROI[uid].minY) m_boundingBoxROI[uid].minY = y;
								if (y > m_boundingBoxROI[uid].maxY) m_boundingBoxROI[uid].maxY = y;
								if (z < m_boundingBoxROI[uid].minZ) m_boundingBoxROI[uid].minZ = z;
								if (z > m_boundingBoxROI[uid].maxZ) m_boundingBoxROI[uid].maxZ = z;
							}
						}
					}
				}
			}
		}
	}
}

void VOLUME_DATA::updateUIDBoundingBox(int index, bool isUID, bool seed)
{
	if (seed)
	{
		seedBoundingbox[index].reset(getCX(), getCY(), getCZ());
		seedBoundingbox[index].minX = dataCX - 1;
		seedBoundingbox[index].minY = dataCY - 1;
		seedBoundingbox[index].minZ = dataCZ - 1;
		seedBoundingbox[index].maxX = 0;
		seedBoundingbox[index].maxY = 0;
		seedBoundingbox[index].maxZ = 0;

		mask val = index + 1;
		BoundingBoxI* binfo = &seedBoundingbox[index];
		for (int z = 0; z < dataCZ; z++)
		{
			for (int y = 0; y < dataCY; y++)
			{
				for (int x = 0; x < dataCX; x++)
				{
					if (pData3D_Mask[0][z * dataCX * dataCY + y * dataCX + x] == 0)continue;

					if (pData3D_Mask[0][z * dataCX * dataCY + y * dataCX + x] & val)
					{
						if (x < binfo->minX) binfo->minX = x;
						if (x > binfo->maxX) binfo->maxX = x;
						if (y < binfo->minY) binfo->minY = y;
						if (y > binfo->maxY) binfo->maxY = y;
						if (z < binfo->minZ) binfo->minZ = z;
						if (z > binfo->maxZ) binfo->maxZ = z;
					}
				}
			}
		}
	}
	else
	{
		MaskInfo* info = getMaskInfo(index, isUID);

		m_boundingBoxROI[info->uid].reset(getCX(), getCY(), getCZ());
		m_boundingBoxROI[info->uid].minX = dataCX - 1;
		m_boundingBoxROI[info->uid].minY = dataCY - 1;
		m_boundingBoxROI[info->uid].minZ = dataCZ - 1;
		m_boundingBoxROI[info->uid].maxX = 0;
		m_boundingBoxROI[info->uid].maxY = 0;
		m_boundingBoxROI[info->uid].maxZ = 0;

		int _mI = info->uid >= MASK_SECOND_MAX ?
			(info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		BoundingBoxI* binfo = &m_boundingBoxROI[info->uid];
		mask val = _mI == 0 ? info->mask_id : info->mask_id2;
		for (int z = 0; z < dataCZ; z++)
		{
			for (int y = 0; y < dataCY; y++)
			{
				for (int x = 0; x < dataCX; x++)
				{
					if (pData3D_Mask[_mI][z * dataCX * dataCY + y * dataCX + x] == 0)
						continue;

					if (pData3D_Mask[_mI][z * dataCX * dataCY + y * dataCX + x] & val)
					{
						if (x < binfo->minX) binfo->minX = x;
						if (x > binfo->maxX) binfo->maxX = x;
						if (y < binfo->minY) binfo->minY = y;
						if (y > binfo->maxY) binfo->maxY = y;
						if (z < binfo->minZ) binfo->minZ = z;
						if (z > binfo->maxZ) binfo->maxZ = z;
					}
				}
			}
		}
	}
}

void VOLUME_DATA::updateBoundingBoxByUID(int uid)
{
	bool isUID = true;
	MaskInfo* info = getMaskInfo(uid, isUID);

	m_boundingBoxROI[info->uid].reset(getCX(), getCY(), getCZ());
	m_boundingBoxROI[info->uid].minX = dataCX - 1;
	m_boundingBoxROI[info->uid].minY = dataCY - 1;
	m_boundingBoxROI[info->uid].minZ = dataCZ - 1;
	m_boundingBoxROI[info->uid].maxX = 0;
	m_boundingBoxROI[info->uid].maxY = 0;
	m_boundingBoxROI[info->uid].maxZ = 0;

	int _mI = info->uid >= MASK_SECOND_MAX ?
		(info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	BoundingBoxI* binfo = &m_boundingBoxROI[info->uid];
	mask val = _mI == 0 ? info->mask_id : info->mask_id2;
	for (int z = 0; z < dataCZ; z++)
	{
		for (int y = 0; y < dataCY; y++)
		{
			for (int x = 0; x < dataCX; x++)
			{
				if (pData3D_Mask[_mI][z * dataCX * dataCY + y * dataCX + x] == 0)
					continue;

				if (pData3D_Mask[_mI][z * dataCX * dataCY + y * dataCX + x] & val)
				{
					if (x < binfo->minX) binfo->minX = x;
					if (x > binfo->maxX) binfo->maxX = x;
					if (y < binfo->minY) binfo->minY = y;
					if (y > binfo->maxY) binfo->maxY = y;
					if (z < binfo->minZ) binfo->minZ = z;
					if (z > binfo->maxZ) binfo->maxZ = z;
				}
			}
		}
	}
}

bool VOLUME_DATA::compareBoundingBox(int index, bool seed)
{
	BoundingBoxI* box = NULL;

	if (seed)
	{
		if (!seedBoundingbox[index].isFitROI())
		{
			updateUIDBoundingBox(index, true, true);
		}

		box = &seedBoundingbox[index];
	}
	else
	{
		MaskInfo* info = getMaskInfo(index);

		if (!m_boundingBoxROI[info->uid].isFitROI())
		{
			updateUIDBoundingBox(index);
		}

		box = &m_boundingBoxROI[info->uid];

	}

	if (m_boundingBox.minX == box->minX &&
		m_boundingBox.minY == box->minY &&
		m_boundingBox.minZ == box->minZ &&
		m_boundingBox.maxX == box->maxX &&
		m_boundingBox.maxY == box->maxY &&
		m_boundingBox.maxZ == box->maxZ)
	{
		return true;
	}

	return false;
}

