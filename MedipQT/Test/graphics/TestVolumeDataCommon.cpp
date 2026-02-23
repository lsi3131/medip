#include "stdafx.h"
#include "TestVolumeDataCommon.h"

/* MaskBitData */
MaskBitData::MaskBitData() :
	Info(nullptr),
	VolumeData(nullptr)
{
}

MaskBitData::MaskBitData(VOLUME_DATA* pVolumeData, MaskInfo* pInfo)
{
	VolumeData = pVolumeData;
	Info = pInfo;
}

int MaskBitData::GetMaskByteIndex()
{
	return VolumeData->GetMaskByteIndex(Info);
}

mask MaskBitData::GetMaskBitFlag()
{
	return VolumeData->GetMaskBitFlag(Info);
}

int MaskBitData::GetVoxelCount()
{
	return VolumeData->getVoxelCount(Info->uid);
}

BoundingBoxI MaskBitData::GetBoundingBox()
{
	return VolumeData->getBoundingBox(Info->uid);
}

void MaskBitData::SetBitList(std::vector<mip::VECTOR3> posList)
{
	for (auto pos : posList)
	{
		VolumeData->setMaskBit(pos.x, pos.y, pos.z, GetMaskBitFlag(), GetMaskByteIndex());
	}
	VolumeData->updateBoundingBoxByUID(Info->uid);
	VolumeData->updateVoxelCount(Info->uid, GetMaskBitFlag(), GetMaskByteIndex());
}

bool MaskBitData::IsBit(int x, int y, int z)
{
	return VolumeData->isMaskBit(x, y, z, GetMaskBitFlag(), GetMaskByteIndex());
}

bool MaskBitData::IsBitList(std::vector<mip::VECTOR3> posList)
{
	for (auto pos : posList)
	{
		if (IsBit(pos.x, pos.y, pos.z) == false)
		{
			return false;
		}
	}
	return true;
}

bool MaskBitData::IsAIDataExist()
{
	return VolumeData->IsAIMaskByUID(Info->uid);
}

VoxelVolumeData<unsigned char> MaskBitData::GetAIResult()
{
	return GetAIResultVolumeDataByUID(VolumeData, Info->uid);
}

/* Utility */
MaskBitData AddNewMaskInfo(VOLUME_DATA* pVolumeData)
{
	if (pVolumeData->createMaskInfo() == false)
	{
		return MaskBitData(nullptr, nullptr);
	}

	return MaskBitData(pVolumeData, pVolumeData->getCurrentMaskInfo());
}

MaskBitData AddNewMaskInfoWithName(VOLUME_DATA* pVolumeData, const QString& name)
{
	if (pVolumeData->createMaskInfoWithName(name) == false)
	{
		return MaskBitData(nullptr, nullptr);
	}

	return MaskBitData(pVolumeData, pVolumeData->getCurrentMaskInfo());
}

std::vector<MaskBitData> AddNewMaskInfoList(VOLUME_DATA* pVolumeData, int count)
{
	std::vector<MaskBitData> bitDataList;
	for (int i = 0; i < count; ++i)
	{
		bitDataList.push_back(AddNewMaskInfo(pVolumeData));
	}
	return bitDataList;
}

std::vector<MaskBitData> GetMaskBitDataList(VOLUME_DATA* pVolumeData)
{
	std::vector<MaskBitData> bitDataList;
	for (int i = 0; i < pVolumeData->getMaskInfoListCnt(); ++i)
	{
		bitDataList.push_back(MaskBitData(pVolumeData, pVolumeData->getMaskInfoByIndex(i)));
	}
	return bitDataList;
}

MaskBitData AddNewMaskAndAISegData(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, string name)
{
	if (name.empty())
	{
		pVolumeData->createMaskInfo();
	}
	else
	{
		pVolumeData->createMaskInfoWithName(name.c_str());
	}
	MaskBitData maskBitData(pVolumeData, pVolumeData->getCurrentMaskInfo());

	int uid = maskBitData.Info->uid;
	int maskByteIndex = maskBitData.GetMaskByteIndex();
	mask maskBitFlag = maskBitData.GetMaskBitFlag();

	std::pair<int, std::vector<unsigned char>> pairAIResultData = std::make_pair(uid, aiSegData.GetRaw());
	pVolumeData->m_vecAIResultData.push_back(pairAIResultData);
	int voxelCount = pVolumeData->applyTempMaskBitOutset(aiSegData.GetRaw(), maskBitFlag, maskByteIndex, threshold);
	pVolumeData->setVoxelCount(uid, voxelCount);
	pVolumeData->updateBoundingBoxByUID(uid);

	std::pair<int, int> pairAIOutset = std::make_pair(uid, threshold);
	pVolumeData->m_vecAIOutset.push_back(pairAIOutset);

	return maskBitData;
}

std::vector<MaskBitData> AddNewMaskAndAISegDataList(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, int count)
{
	std::vector<MaskBitData> bitDataList;
	for (int i = 0; i < count; ++i)
	{
		bitDataList.push_back(AddNewMaskAndAISegData(pVolumeData, aiSegData, threshold));
	}
	return bitDataList;
}

std::unordered_map<std::string, MaskBitData> AddNewMaskAndAISegDataTable(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, vector<string> names)
{
	std::unordered_map<std::string, MaskBitData> result;
	for (std::string name : names)
	{
		result[name] = AddNewMaskAndAISegData(pVolumeData, aiSegData, threshold, name);
	}
	return result;
}

VoxelVolumeData<unsigned char> GetAIResultVolumeDataByUID(VOLUME_DATA* pVolumeData, int UID)
{
	VoxelVolumeSize size(pVolumeData->getCX(), pVolumeData->getCY(), pVolumeData->getCZ());
	return VoxelVolumeData<unsigned char>(pVolumeData->GetAIResultByUID(UID), size);
}



