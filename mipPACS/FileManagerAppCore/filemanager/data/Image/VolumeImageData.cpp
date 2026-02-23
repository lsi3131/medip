#include "stdafx.h"
#include "VolumeImageData.h"

using namespace fm;

VolumeImageData::VolumeImageData() :
	m_pData3D_HU(nullptr),
	m_cx(0),
	m_cy(0),
	m_cz(0),
	m_HUMin(0),
	m_HUMax(0)
{
}

VolumeImageData::~VolumeImageData()
{
	Clear();
}

VolumeImageData& fm::VolumeImageData::operator=(const VolumeImageData& rhs)
{
	Init(this->m_pData3D_HU, this->m_cx, this->m_cy, this->m_cz);

	return *this;
}

mint16* fm::VolumeImageData::GetPtr()
{
	return m_pData3D_HU;
}

bool VolumeImageData::Init(mint16* pData, muint32 cx, muint32 cy, muint32 cz)
{
	if (cx == 0 && cy == 0 && cz == 0)
		return false;

	m_cx = cx;
	m_cy = cy;
	m_cz = cz;

	int length = getDataLength();
	int size = getDataLength() * sizeof(mint16);
	SAFE_DELETES(m_pData3D_HU);
	m_pData3D_HU = new mint16[length];
	memcpy(m_pData3D_HU, pData, size);

	UpdateHUMinMax();
	return true;
}

bool VolumeImageData::Clear()
{
	SAFE_DELETES(m_pData3D_HU);
	m_pData3D_HU = nullptr;

	m_cx = 0;
	m_cy = 0;
	m_cz = 0;

	return true;
}

bool fm::VolumeImageData::IsValidate()
{
	return m_pData3D_HU != nullptr;
}

mint16 VolumeImageData::GetData(muint32 x, muint32 y, muint32 z)
{
	if (x >= m_cx || y >= m_cy || z >= m_cz)
		return 0;

	muint32 index = (z * m_cx * m_cy) + (y * m_cx) + x;

	if (index > (getDataLength() - 1))
		return 0;

	return m_pData3D_HU[index];
}

muint32 VolumeImageData::getCX()
{
	return m_cx;
}

muint32 VolumeImageData::getCY()
{
	return m_cy;
}

muint32 VolumeImageData::getCZ()
{
	return m_cz;
}

muint32 VolumeImageData::getDataLength()
{
	return m_cx * m_cy * m_cz;
}

bool VolumeImageData::UpdateHUMinMax()
{
	if (m_pData3D_HU == NULL)
	{
		return false;
	}

	mint16 min = INT16_MAX;
	mint16 max = INT16_MIN;

	int dataLength = getDataLength();
	for (int n = 0; n < dataLength; n++)
	{
		if (m_pData3D_HU[n] > max)
			max = m_pData3D_HU[n];

		if (m_pData3D_HU[n] < min)
			min = m_pData3D_HU[n];
	}

	m_HUMin = min;
	m_HUMax = max;

	return true;
}

mint16 VolumeImageData::HUMin()
{
	return m_HUMin;
}

mint16 VolumeImageData::HUMax()
{
	return m_HUMax;
}

