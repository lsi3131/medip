#pragma once

#ifndef VOXEL_VOLUMESIZE_H
#define VOXEL_VOLUMESIZE_H

class VoxelVolumeSize
{
public:
	VoxelVolumeSize();
	VoxelVolumeSize(int cx, int cy, int cz);

public:
	bool operator==(const VoxelVolumeSize& rhs) const;
	bool operator!=(const VoxelVolumeSize& rhs) const;

public:
	bool Init(int cx, int cy, int cz);
	void SetEmpty();

	bool IsValid() const;
	bool IsInRange(int x, int y, int z) const;

	int GetIndex(int x, int y, int z) const;

	int Length() const;
	int CX() const;
	int CY() const;
	int CZ() const;

private:
	int m_cx;
	int m_cy;
	int m_cz;
};
#endif