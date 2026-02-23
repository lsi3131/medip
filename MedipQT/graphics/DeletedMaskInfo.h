#pragma once

#ifndef DELETED_MASKINFO_H
#define DELETED_MASKINFO_H

#include "MaskInfo.h"
#include "BoundingBox.h"
#include <memory>
#include <vector>

class VOLUME_DATA;

struct BackupMaskInfo
{
	int LayerIndex;
	std::shared_ptr<MaskInfo> Info;
	int VoxelCount;
	BoundingBoxI BoundingBox;
	std::vector<unsigned char> AIResult;
	int AIOutset;

	BackupMaskInfo();
	BackupMaskInfo(int layerIndex, const MaskInfo& info, int voxelCount, BoundingBoxI boundingBox);

	bool HasSameMaskInfo(VOLUME_DATA* pData) const;
	bool IsAIDataExist() const;
};

class DeletedMaskInfo
{
public:
	DeletedMaskInfo();
	~DeletedMaskInfo();

	void SetMoved(bool value);
	bool IsMoved() const;

	void SetDeletedMaskInfo(const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset);
	BackupMaskInfo* GetDeletedBackupInfo() const;

	MaskInfo* GetDeletedMaskInfo() const;
	int GetDeletedMaskIndex() const;
	int GetDeletedVoxelCount() const;
	BoundingBoxI GetDeletedBoundingBox() const;

	void SetMovedMaskInfo(const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset);
	BackupMaskInfo* GetMovedBackupInfo() const;

	MaskInfo* GetMovedMaskInfo() const;
	int GetMovedMaskIndex() const;
	int GetMovedVoxelCount() const;
	BoundingBoxI GetMovedBoundingBox() const;

private:
	void setBackupMaskInfo(BackupMaskInfo* pInfo, const MaskInfo& info, int layerIndex, int voxelCount, BoundingBoxI boundingBox, const std::vector<unsigned char>& AIResult, int AIOutset);

private:
	bool m_isMoved;
	std::shared_ptr<BackupMaskInfo> m_pDeletedMaskInfo;
	std::shared_ptr<BackupMaskInfo> m_pMovedMaskInfo;
};

#endif