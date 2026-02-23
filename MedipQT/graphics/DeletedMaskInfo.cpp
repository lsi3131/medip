#include "stdafx.h"
#include "DeletedMaskInfo.h"
#include "volumedata.h"

/*
	BackupMaskInfo
*/
BackupMaskInfo::BackupMaskInfo()
{
	Info = std::make_shared<MaskInfo>();
	AIOutset = 0;
}

BackupMaskInfo::BackupMaskInfo(int layerIndex, const MaskInfo& info, int voxelCount, BoundingBoxI boundingBox) :
	LayerIndex(layerIndex),
	VoxelCount(voxelCount),
	BoundingBox(boundingBox)
{
	Info = info.Clone();
	AIOutset = 0;
}

bool BackupMaskInfo::HasSameMaskInfo(VOLUME_DATA* pData) const
{
	MaskInfo* pInfo = pData->getMaskInfoByIndex(LayerIndex);
	if (pInfo == nullptr)
	{
		return false;
	}
	return
		*Info == *pInfo &&
		VoxelCount == pData->getVoxelCount(pInfo->uid) &&
		BoundingBox == pData->getBoundingBox(pInfo->uid);
}

bool BackupMaskInfo::IsAIDataExist() const
{
	return !AIResult.empty();
}

/* 
	DeletedMaskInfo 
*/
DeletedMaskInfo::DeletedMaskInfo() :
	m_isMoved(false),
	m_pDeletedMaskInfo(nullptr),
	m_pMovedMaskInfo(nullptr)
{
}

DeletedMaskInfo::~DeletedMaskInfo()
{
}

void DeletedMaskInfo::SetMoved(bool value)
{
	m_isMoved = value;
}

bool DeletedMaskInfo::IsMoved() const
{
	return m_isMoved;
}

void DeletedMaskInfo::SetDeletedMaskInfo(const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset)
{
	m_pDeletedMaskInfo = std::make_shared<BackupMaskInfo>();
	setBackupMaskInfo(
		m_pDeletedMaskInfo.get(),
		info,
		layerIndex,
		voxelCount,
		boundingBox,
		AIResult,
		AIOutset
	);
}

BackupMaskInfo* DeletedMaskInfo::GetDeletedBackupInfo() const
{
	return m_pDeletedMaskInfo.get();
}

MaskInfo* DeletedMaskInfo::GetDeletedMaskInfo() const
{
	return m_pDeletedMaskInfo->Info.get();
}

int DeletedMaskInfo::GetDeletedMaskIndex() const
{
	return m_pDeletedMaskInfo->LayerIndex;
}

int DeletedMaskInfo::GetDeletedVoxelCount() const
{
	return m_pDeletedMaskInfo->VoxelCount;
}

BoundingBoxI DeletedMaskInfo::GetDeletedBoundingBox() const
{
	return m_pDeletedMaskInfo->BoundingBox;
}

void DeletedMaskInfo::SetMovedMaskInfo(const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset)
{
	m_pMovedMaskInfo = std::make_shared<BackupMaskInfo>();
	setBackupMaskInfo(
		m_pMovedMaskInfo.get(),
		info,
		layerIndex,
		voxelCount,
		boundingBox,
		AIResult,
		AIOutset
	);

}

BackupMaskInfo* DeletedMaskInfo::GetMovedBackupInfo() const
{
	return m_pMovedMaskInfo.get();
}

MaskInfo* DeletedMaskInfo::GetMovedMaskInfo() const
{
	return m_pMovedMaskInfo->Info.get();
}

int DeletedMaskInfo::GetMovedMaskIndex() const
{
	return m_pMovedMaskInfo->LayerIndex;
}

int DeletedMaskInfo::GetMovedVoxelCount() const
{
	return m_pMovedMaskInfo->VoxelCount;
}

BoundingBoxI DeletedMaskInfo::GetMovedBoundingBox() const
{
	return m_pMovedMaskInfo->BoundingBox;
}

void DeletedMaskInfo::setBackupMaskInfo(BackupMaskInfo* pInfo, const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset)
{
	pInfo->LayerIndex = layerIndex;
	pInfo->VoxelCount = voxelCount;
	pInfo->BoundingBox = boundingBox;
	pInfo->AIResult = AIResult;
	pInfo->AIOutset = AIOutset;
	info.Copy(pInfo->Info.get());
}
