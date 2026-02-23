#include "stdafx.h"
#include "DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/data/Image/VolumeUtility.h"
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <QElapsedTimer>

namespace fm
{
	//=================================================================
	//		DicomDataImage_sint16
	//=================================================================
	DicomDataImage_sint16::DicomDataImage_sint16() :
		m_pData(nullptr), m_pDcmDataset(nullptr),
		m_HUMin(0), m_HUMax(0),
		m_rescaleIntercept(0.0f),
		m_rescaleSlope(1.0f),
		m_pixelRepresentation(1),
		m_dcmModalType(DCM_MODAL_TYPE::DCM_CT)
	{
	}

	DicomDataImage_sint16::DicomDataImage_sint16(DicomDataset* pDcmDataset) :
		m_pData(nullptr), m_pDcmDataset(pDcmDataset),
		m_HUMin(0), m_HUMax(0),
		m_rescaleIntercept(0.0f),
		m_rescaleSlope(1.0f),
		m_pixelRepresentation(1),
		m_dcmModalType(DCM_MODAL_TYPE::DCM_CT)
	{
		Init(pDcmDataset);
	}

	DicomDataImage_sint16::~DicomDataImage_sint16()
	{
		SAFE_DELETES(m_pData);
	}

	bool DicomDataImage_sint16::Init(DicomDataset* pDcmDataset)
	{
		m_pDcmDataset = pDcmDataset;
		m_width = m_pDcmDataset->GetValueInt(DicomTagID::Columns);
		m_height = m_pDcmDataset->GetValueInt(DicomTagID::Rows);

		m_bitsStored = m_pDcmDataset->GetValueInt(DicomTagID::BitsStored);
		m_bitsAllocated = m_pDcmDataset->GetValueInt(DicomTagID::BitsAllocated);
		m_dataSize = m_width * m_height * sizeof(mint16);

		m_rescaleIntercept = m_pDcmDataset->GetValueFloat(DicomTagID::RescaleIntercept);
		m_rescaleSlope = m_pDcmDataset->GetValueFloat(DicomTagID::RescaleSlope);

		m_pixelRepresentation = 0;
		m_pDcmDataset->TryGetValueInt(DicomTagID::PixelRepresentation, &m_pixelRepresentation);
		m_dcmModalType = m_pDcmDataset->GetDcmModalityType();

		int resultBufferSize = m_width * m_height * sizeof(mint16);
		SAFE_DELETES(m_pData);
		m_pData = new mint16[resultBufferSize];

		//#define BUG_REDMINE_1333
#ifdef BUG_REDMINE_1333
		//REDMINE_1333이슈 재현을 위한 코드
		//http ://www.medicalip.net:30002/issues/1333
		//mint16* pSourceData = (mint16*)m_pDcmDataset->GetValuePtr(fm::DicomTagID::PixelData, m_width, m_height);
		//memcpy(m_pData, pSourceData, bufferSize);
#else
		DcmDataset* pData = m_pDcmDataset->Data();
		E_TransferSyntax xferSyntax = pData->getCurrentXfer();

		DicomImage image(pData, xferSyntax, m_rescaleSlope, m_rescaleIntercept);
		image.hideAllOverlays();
		EI_Status imageStatus = image.getStatus();
		if (imageStatus != EIS_Normal)
		{
			qWarning() << "invalid image status : " << imageStatus;
			return false;
		}

		int bitsAllocated = m_pDcmDataset->GetValueInt(DicomTagID::BitsAllocated);
		if (bitsAllocated == 8)
		{
			char* pData_8bit = new char[m_width * m_height];
			int bufferSize_8bit = m_width * m_height * sizeof(char);
			memset(pData_8bit, 0, bufferSize_8bit);
			if (image.getOutputData(pData_8bit, bufferSize_8bit, m_bitsStored) == false)
			{
				qWarning() << "fail to get image output data";
				return false;
			}

			for (int x = 0; x < m_width; ++x)
			{
				for (int y = 0; y < m_height; ++y)
				{
					int idx = x + m_width * y;
					m_pData[idx] = pData_8bit[idx];
				}
			}
			SAFE_DELETES(pData_8bit);
		}
		else if (bitsAllocated == 16)
		{
			if (image.getOutputData(m_pData, resultBufferSize, m_bitsStored) == false)
			{
				qWarning() << "fail to get image output data";
				return false;
			}
		}
		else
		{
			qWarning() << "not supported bit allocated : " << bitsAllocated;
			return false;
		}

		applyHUOffset();

		float windowingLevel = 0.f;
		float windowingWidth = 0.f;
		if (
			m_pDcmDataset->TryGetWindowingLevel(&windowingLevel) &&
			m_pDcmDataset->TryGetWindowingWidth(&windowingWidth)
			)
		{
			m_windowingInfo.Level = windowingLevel;
			m_windowingInfo.Width = windowingWidth;
		}
		else
		{
			UpdateMinMax();
			m_windowingInfo.InitByHU(m_HUMin, m_HUMax);
		}
#endif

		return m_pData != nullptr;
	}

	bool DicomDataImage_sint16::IsValidate()
	{
		return m_pData != nullptr;
	}

	int DicomDataImage_sint16::Width()
	{
		return m_width;
	}

	int DicomDataImage_sint16::Height()
	{
		return m_height;
	}

	int DicomDataImage_sint16::Length()
	{
		return m_width * m_height;
	}

	int DicomDataImage_sint16::Size()
	{
		return m_width * m_height * sizeof(mint16);
	}

	int DicomDataImage_sint16::GetData(int x, int y)
	{
		if ((m_pData == nullptr) || (x < 0) || (y < 0))
		{
			return 0;
		}

		if ((x >= m_width) || (y >= m_height))
		{
			return 0;
		}

		if (m_pixelRepresentation == 0)
		{
			return ((muint16*)m_pData)[x + m_width * y];
		}
		else
		{
			return ((mint16*)m_pData)[x + m_width * y];
		}
	}

	int DicomDataImage_sint16::GetDataWithRescale(int x, int y)
	{
		return ((float)GetData(x, y) * m_rescaleSlope) + m_rescaleIntercept;
	}

	void* DicomDataImage_sint16::Ptr()
	{
		return m_pData;
	}

	void DicomDataImage_sint16::UpdateMinMax()
	{
		if (IsValidate() == false)
		{
			return;
		}

		int min = 0;
		int max = 0;
		if (m_pixelRepresentation == 0)
		{
			min = UINT16_MAX;
			max = 0;
		}
		else
		{
			min = INT16_MAX;
			max = INT16_MIN;
		}

		int dataLength = m_width * m_height;
		for (int n = 0; n < dataLength; n++)
		{
			if (m_pixelRepresentation == 0)
			{
				muint16* pData_uint16 = (muint16*)m_pData;

				if (pData_uint16[n] > max)
					max = pData_uint16[n];

				if (pData_uint16[n] < min)
					min = pData_uint16[n];
			}
			else
			{
				mint16* pData_int16 = (mint16*)m_pData;

				if (pData_int16[n] > max)
					max = pData_int16[n];

				if (pData_int16[n] < min)
					min = pData_int16[n];
			}
		}

		m_HUMin = min;
		m_HUMax = max;
	}

	int DicomDataImage_sint16::HUMin()
	{
		return m_HUMin;
	}

	int DicomDataImage_sint16::HUMax()
	{
		return m_HUMax;
	}

	void DicomDataImage_sint16::applyHUOffset()
	{
		ApplyHUOffsetToImage(
			m_pData, m_width, m_height,
			m_dcmModalType,
			m_pixelRepresentation,
			m_rescaleIntercept,
			m_rescaleSlope,
			m_bitsStored
		);
	}

	WindowingInfo DicomDataImage_sint16::GetWindowingInfo()
	{
		return m_windowingInfo;
	}
}
