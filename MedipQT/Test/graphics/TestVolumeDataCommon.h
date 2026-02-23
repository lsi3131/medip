#pragma once

#ifndef TEST_VOLUME_DATA_COMMON_H
#define TEST_VOLUME_DATA_COMMON_H

#include "graphics/VolumeData.h"
#include "Test/graphics/VoxelVolumeData.h"
#include "ActionManager.h"
#include "stringManager.h"
#include <unordered_map>
#include <string>

struct MaskBitData
{
	MaskInfo* Info;
	VOLUME_DATA* VolumeData;

	MaskBitData();
	MaskBitData(VOLUME_DATA* pVolumeData, MaskInfo* pInfo);

	int GetMaskByteIndex();
	mask GetMaskBitFlag();
	int GetVoxelCount();

	BoundingBoxI GetBoundingBox();

	void SetBitList(std::vector<mip::VECTOR3> posList);
	bool IsBit(int x, int y, int z);
	bool IsBitList(std::vector<mip::VECTOR3> posList);

	bool IsAIDataExist();

	VoxelVolumeData<unsigned char> GetAIResult();
};

MaskBitData AddNewMaskInfo(VOLUME_DATA* pVolumeData);
MaskBitData AddNewMaskInfoWithName(VOLUME_DATA* pVolumeData, const QString& name);

std::vector<MaskBitData> AddNewMaskInfoList(VOLUME_DATA* pVolumeData, int count);
std::vector<MaskBitData> GetMaskBitDataList(VOLUME_DATA* pVolumeData);

MaskBitData AddNewMaskAndAISegData(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, std::string name = "");
std::vector<MaskBitData> AddNewMaskAndAISegDataList(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, int count);
std::unordered_map<std::string, MaskBitData> AddNewMaskAndAISegDataTable(VOLUME_DATA* pVolumeData, const VoxelVolumeData<unsigned char>& aiSegData, int threshold, std::vector<std::string> names);

VoxelVolumeData<unsigned char> GetAIResultVolumeDataByUID(VOLUME_DATA* pVolumeData, int UID);


#endif