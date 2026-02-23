#pragma once

#ifndef VOXEL_VOLUMEDATA_H
#define VOXEL_VOLUMEDATA_H

#include "Test/graphics/VoxelVolumeSize.h"

template<typename T>
class VoxelVolumeData
{
public:
	VoxelVolumeData()
	{
	}

	VoxelVolumeData(int cx, int cy, int cz)
	{
		m_volumeSize.Init(cx, cy, cz);
		m_volumeData.resize(m_volumeSize.Length());
	}

	VoxelVolumeData(const VoxelVolumeSize& volumeSize)
	{
		m_volumeSize = volumeSize;
		m_volumeData.resize(m_volumeSize.Length());
	}

	VoxelVolumeData(const std::vector<T>& volumeData, const VoxelVolumeSize& volumeSize)
	{
		if (volumeData.size() != volumeSize.Length())
		{
			//Q_ASSERT(false);
			qCritical() << "can't volume data length = " << volumeData.size() <<", but volume size length : " << volumeSize.Length();
			return;
		}
		m_volumeSize = volumeSize;
		m_volumeData = volumeData;
	}

public:
	bool SetData(int x, int y, int z, T value)
	{
		if (m_volumeSize.IsInRange(x, y, z) == false)
		{
			return false;
		}

		m_volumeData[m_volumeSize.GetIndex(x, y, z)] = value;
		return true;
	}

	T GetData(int x, int y, int z) const
	{
		if (m_volumeSize.IsInRange(x, y, z) == false)
		{
			return 0;
		}

		return m_volumeData[m_volumeSize.GetIndex(x, y, z)];
	}

	const T* GetPtr() const
	{
		return m_volumeData.data();
	}

	const std::vector<T>& GetRaw() const
	{
		return m_volumeData;
	}

	bool IsValid() const
	{
		return m_volumeSize.IsValid();
	}

private:
	VoxelVolumeSize m_volumeSize;
	std::vector<T> m_volumeData;
};
#endif