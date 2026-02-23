#pragma once

#include "filemanager/export.h"
#include "filemanager/data/Image/WindowingInfo.h"

namespace fm
{
	class FM_CORE_EXPORT VolumeImageData
	{
	public:
		VolumeImageData();
		~VolumeImageData();

		VolumeImageData& operator=(const VolumeImageData& rhs);
		
	public:
		mint16* GetPtr();
		

		bool Init(mint16* pData, muint32 cx, muint32 cy, muint32 cz);
		bool Clear();
		bool IsValidate();

		mint16 VolumeImageData::GetData(muint32 x, muint32 y, muint32 z);

		muint32 getCX();
		muint32 getCY();
		muint32 getCZ();

		muint32 getDataLength();

		bool UpdateHUMinMax();
		mint16 HUMin();
		mint16 HUMax();

	private:
		mint16* m_pData3D_HU;

		muint32 m_cx;
		muint32 m_cy;
		muint32 m_cz;

		mint16 m_HUMin;
		mint16 m_HUMax;
	};
}
