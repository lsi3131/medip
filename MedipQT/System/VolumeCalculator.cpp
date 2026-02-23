#include "stdafx.h"
#include "graphics/volumedata.h"
#include "Windows/windowManager.h"
#include "System/StringManager.h"
#include "VolumeCalculator.h"

//=======================================
//		MaskUtil
//=======================================
bool MaskUtil::CreateMaskInfoList(std::vector<MaskInfo>& outNewMaskInfos, VOLUME_DATA * pVolumeData)
{
	std::vector<MaskInfo*> originMaskInfos;
	std::vector<MaskInfo> newMaskInfos;
	if (GetAllMaskInfos(originMaskInfos, pVolumeData) == false)
	{
		return false;
	}

	for (int i = 0; i < originMaskInfos.size(); ++i)
	{
		if (originMaskInfos[i] != nullptr)
		{
			MaskInfo newMaskInfo;
			memcpy(&newMaskInfo, originMaskInfos[i], sizeof(MaskInfo));

			newMaskInfos.push_back(newMaskInfo);
		}
	}
	outNewMaskInfos = newMaskInfos;
	return newMaskInfos.empty() == false;
}

bool MaskUtil::GetAllMaskInfos(std::vector<MaskInfo*>& outMaskInfos, VOLUME_DATA* pVolumeData)
{
	std::vector<MaskInfo*> maskInfos;
	for (int i = 0; i < pVolumeData->getMaskInfoListCnt(); ++i)
	{
		MaskInfo* pMaskInfo = pVolumeData->getMaskInfo(i, false);
		if (pMaskInfo != nullptr)
		{
			maskInfos.push_back(pMaskInfo);
		}
	}
	outMaskInfos = maskInfos;
	return maskInfos.empty() == false;
}

bool MaskUtil::GetMaskInfos(std::vector<MaskInfo*>& outMaskInfos, VOLUME_DATA * pVolumeData, std::vector<muint32>& maskUIDs)
{
	std::vector<MaskInfo*> maskInfos;
	for (int i = 0; i < maskUIDs.size(); ++i)
	{
		MaskInfo* pMaskInfo = pVolumeData->getMaskInfo(maskUIDs[i]);
		if (pMaskInfo != nullptr)
		{
			maskInfos.push_back(pMaskInfo);
		}
	}
	outMaskInfos = maskInfos;
	return maskInfos.empty() == false;
}

int MaskUtil::GetMaskIndex(MaskInfo * pMaskInfo)
{
	int uid = pMaskInfo->uid;
	return (uid >= MASK_SECOND_MAX) ? (uid - MASK_SECOND_MAX) / 8 + 1 : 0;
}

unsigned char MaskUtil::GetMaskID(MaskInfo * pMaskInfo)
{
	int maskIndex = GetMaskIndex(pMaskInfo);
	return (0 == maskIndex) ? pMaskInfo->mask_id : pMaskInfo->mask_id2;
}


//=======================================
//		VolumeCalculator
//=======================================
VolumeCalculator::VolumeCalculator() :
	m_pOriginHUVolumeData(nullptr),
	m_cx(0),
	m_cy(0),
	m_cz(0),
	m_operator(OP_PLUS),
	m_value(1.0),
	m_isPreviewON(false)
{
}

VolumeCalculator::~VolumeCalculator()
{
	SAFE_DELETES(m_pOriginHUVolumeData);
}

bool VolumeCalculator::Initialize(VOLUME_DATA * pVolumeData)
{
	/* 원본 HU Volume Data 초기화*/
	SAFE_DELETES(m_pOriginHUVolumeData);
	m_pOriginHUVolumeData = new mint16[pVolumeData->getVolumeDataLength()];
	if (m_pOriginHUVolumeData == nullptr)
	{
		return false;
	}

	int volumeByteSize = pVolumeData->getVolumeDataLength() * sizeof(mint16);
	memcpy(m_pOriginHUVolumeData, pVolumeData->getHUDataPoint(), volumeByteSize);

	m_cx = pVolumeData->getCX();
	m_cy = pVolumeData->getCY();
	m_cz = pVolumeData->getCZ();

	return true;
}

bool VolumeCalculator::IsIntialized()
{
	return m_pOriginHUVolumeData != nullptr;
}

bool VolumeCalculator::CanCalculate(VOLUME_DATA* pVolumeData)
{
	if (IsIntialized() == false)
	{
		return false;
	}

	if (pVolumeData == nullptr)
	{
		return false;
	}

	if (m_cx != pVolumeData->getCX())
	{
		return false;
	}

	if (m_cy != pVolumeData->getCY())
	{
		return false;
	}

	if (m_cz != pVolumeData->getCZ())
	{
		return false;
	}

	return true;
}

void VolumeCalculator::SetCalcParameter(IMAGE_CALCULATOR_OPERATOR op, double value, std::vector<MaskInfo*>& maskInfos)
{
	m_operator = op;
	m_value = value;
	m_maskInfos = maskInfos;
}

bool VolumeCalculator::CalcHU_Volume(VOLUME_DATA* pVolumeData)
{
	if (CanCalculate(pVolumeData) == false)
	{
		return false;
	}

	for (int i = 0; i < m_maskInfos.size(); ++i)
	{
		BoundingBoxI boundingBox = pVolumeData->getBoundingBox(m_maskInfos[i]->uid);
		int xMin = boundingBox.getMinX();
		int yMin = boundingBox.getMinY();
		int zMin = boundingBox.getMinZ();

		int xMax = boundingBox.getMaxX();
		int yMax = boundingBox.getMaxY();
		int zMax = boundingBox.getMaxZ();

		CalcHU_Range(pVolumeData, nullptr, m_maskInfos[i], m_value, m_operator,
			xMin, xMax,
			yMin, yMax,
			zMin, zMax
		);
	}

	return true;
}

bool VolumeCalculator::CalcHU_Plane(VOLUME_DATA * pVolumeData, WINDOW_TYPE type, int depth)
{
	if (CanCalculate(pVolumeData) == false)
	{
		return false;
	}

	for (int i = 0; i < m_maskInfos.size(); ++i)
	{
		BoundingBoxI boundingBox = pVolumeData->getBoundingBox(m_maskInfos[i]->uid);
		int xMin = boundingBox.getMinX();
		int yMin = boundingBox.getMinY();
		int zMin = boundingBox.getMinZ();

		int xMax = boundingBox.getMaxX();
		int yMax = boundingBox.getMaxY();
		int zMax = boundingBox.getMaxZ();

		if (type == WT_AXIAL)
		{
			CalcHU_Range(pVolumeData, nullptr, m_maskInfos[i], m_value, m_operator,
				xMin, xMax,
				yMin, yMax,
				depth, depth
			);
		}
		else if (type == WT_CORONAL)
		{
			CalcHU_Range(pVolumeData, nullptr, m_maskInfos[i], m_value, m_operator,
				xMin, xMax,
				depth, depth,
				zMin, zMax
			);
		}
		else if (type == WT_SAGITTAL)
		{
			CalcHU_Range(pVolumeData, nullptr, m_maskInfos[i], m_value, m_operator,
				depth, depth,
				yMin, yMax,
				zMin, zMax
			);
		}
	}

	return true;
}

bool VolumeCalculator::Reset(VOLUME_DATA* pVolumeData)
{
	if (CanCalculate(pVolumeData) == false)
	{
		return false;
	}

	pVolumeData->setHUData(m_pOriginHUVolumeData,
		pVolumeData->getCX(),
		pVolumeData->getCY(),
		pVolumeData->getCZ()
	);

	return true;
}


void VolumeCalculator::CalcHU_Range(VOLUME_DATA* pTargetVolumeData, bool* pDirtVolume, MaskInfo* pMaskInfo, double value, IMAGE_CALCULATOR_OPERATOR op,
	int xMin, int xMax,
	int yMin, int yMax,
	int zMin, int zMax)
{
	int maskIndex = MaskUtil::GetMaskIndex(pMaskInfo);
	int maskID = MaskUtil::GetMaskID(pMaskInfo);

	mint16* pHUVolume = pTargetVolumeData->getHUDataPoint();
	int cx = pTargetVolumeData->getCX();
	int cy = pTargetVolumeData->getCY();
	int cz = pTargetVolumeData->getCZ();

	for (int z = zMin; z <= zMax; ++z)
	{
		for (int y = yMin; y <= yMax; ++y)
		{
			for (int x = xMin; x <= xMax; ++x)
			{
				if (pTargetVolumeData->isMaskBit(x, y, z, maskID, maskIndex))
				{
					/* DirtVolume 사용 시 해당 Voxel이 Dirt일 경우 Skip한다.*/
					if (pDirtVolume != nullptr)
					{
						if (pDirtVolume[x + cx * y + cx * cy * z] == true)
						{
							continue;
						}
						pDirtVolume[x + cx * y + cx * cy * z] = true;
					}

					//float calc_value = pTargetVolumeData->getData(x, y, z);
					float calc_value = m_pOriginHUVolumeData[x + cx * y + cx* cy* z];
					if (op == OP_PLUS)
					{
						calc_value += value;
					}
					else if (op == OP_MINUS)
					{
						calc_value -= value;
					}
					else if (op == OP_MULTIPLY)
					{
						calc_value *= value;
					}
					else if (op == OP_DIVIDE)
					{
						if (value != 0)
							calc_value /= value;
					}
					else if (op == OP_REPLACE)
					{
						calc_value = value;
					}

					pTargetVolumeData->setData(x, y, z, (mint16)calc_value);
				}
			}
		}
	}
}

bool* VolumeCalculator::CreateDirtVolume(VOLUME_DATA* pVolumeData)
{
	int dirtVolumeDataSize = pVolumeData->getVolumeDataLength();
	bool* pDirtVolume = new bool[dirtVolumeDataSize];
	if (pDirtVolume == nullptr)
	{
		return nullptr;
	}
	memset(pDirtVolume, 0, dirtVolumeDataSize);
	return pDirtVolume;
}

