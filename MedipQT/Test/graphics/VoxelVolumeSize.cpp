#include "stdafx.h"
#include "VoxelVolumeSize.h"

#define INDEX_3D(x, y, z, cx, cy, cz) ((x) + ((cx) * (y)) + ((cx) * (cy) * (z)))

static bool IsValidVolumeSize(int cx, int cy, int cz)
{
	return !(cx <= 0 || cy <= 0 || cz <= 0);
}

VoxelVolumeSize::VoxelVolumeSize() :
	m_cx(0),
	m_cy(0),
	m_cz(0)
{
}

VoxelVolumeSize::VoxelVolumeSize(int cx, int cy, int cz) :
	m_cx(cx),
	m_cy(cy),
	m_cz(cz)
{
}

bool VoxelVolumeSize::operator==(const VoxelVolumeSize& rhs) const
{
	return
		(m_cx == rhs.m_cx) &&
		(m_cy == rhs.m_cy) &&
		(m_cz == rhs.m_cz);
}

bool VoxelVolumeSize::operator!=(const VoxelVolumeSize& rhs) const
{
	return !(*this == rhs);
}

bool VoxelVolumeSize::Init(int cx, int cy, int cz)
{
	if (IsValidVolumeSize(cx, cy, cz) == false)
	{
		return false;
	}

	m_cx = cx;
	m_cy = cy;
	m_cz = cz;
	return true;
}

void VoxelVolumeSize::SetEmpty()
{
	m_cx = 0;
	m_cy = 0;
	m_cz = 0;
}

bool VoxelVolumeSize::IsValid() const
{
	return IsValidVolumeSize(m_cx, m_cy, m_cz);
}

bool VoxelVolumeSize::IsInRange(int x, int y, int z) const
{
	return
		(0 <= x && x < m_cx) &&
		(0 <= y && y < m_cy) &&
		(0 <= z && z < m_cz);
}

int VoxelVolumeSize::GetIndex(int x, int y, int z) const
{
	return INDEX_3D(x, y, z, m_cx, m_cy, m_cz);
}

int VoxelVolumeSize::Length() const
{
	return m_cx * m_cy * m_cz;
}

int VoxelVolumeSize::CX() const
{
	return m_cx;
}

int VoxelVolumeSize::CY() const
{
	return m_cy;
}

int VoxelVolumeSize::CZ() const
{
	return m_cz;
}

