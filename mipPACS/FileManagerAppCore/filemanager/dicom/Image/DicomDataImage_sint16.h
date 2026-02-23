#pragma once

#include <mip/core.h>
#include <vector>
#include "filemanager/export.h"
#include "filemanager/data/Image/WindowingInfo.h"
#include "DicomReader.h"

class DicomImage;

namespace fm
{
	class DicomDataset;
	class DicomInfomationModelSeriesObject;

	class FM_CORE_EXPORT DicomDataImage_sint16
	{
	public:
		DicomDataImage_sint16();
		DicomDataImage_sint16(DicomDataset* pDcmDataset);
		~DicomDataImage_sint16();

		bool Init(DicomDataset* pDcmDataset);
		bool IsValidate();

		int Width();
		int Height();

		int Length();
		int Size();

		int GetData(int x, int y);
		int GetDataWithRescale(int x, int y);
		void* Ptr();

		void UpdateMinMax();

		int HUMin();
		int HUMax();

		WindowingInfo GetWindowingInfo();
	private:
		void applyHUOffset();
	private:
		DicomDataset* m_pDcmDataset;
		mint16* m_pData;
		int m_width;
		int m_height;
		int m_bitsStored;
		int m_bitsAllocated;
		int m_dataSize;

		int m_HUMin;
		int m_HUMax;

		float m_rescaleIntercept;
		float m_rescaleSlope;
		int m_pixelRepresentation; /* 0 : unsigned int, 1 : signed int*/

		DCM_MODAL_TYPE m_dcmModalType;
		WindowingInfo m_windowingInfo;
	};
}
