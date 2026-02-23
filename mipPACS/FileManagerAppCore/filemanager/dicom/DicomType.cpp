#include "stdafx.h"
#include "DicomType.h"
#include "filemanager/dicom/DicomDataset.h"
#include <dcmtk/dcmdata/dcuid.h>

namespace fm
{
	DicomType::DicomType(DicomDataset* pDcmDataset) :
		DicomType(*pDcmDataset)
	{
	}

	DicomType::DicomType(const DicomDataset& dataset)
	{
		m_hasImagePixelData = hasImageData(dataset);
		m_sopClass = dataset.GetValueString(fm::DicomTagID::SOPClassUID);
		m_imageTypeList = dataset.GetValueWStringList(fm::DicomTagID::ImageType);

		int bitAllocated = 16;
		int samplePerPixel = 1;
		if (dataset.TryGetValueInt(fm::DicomTagID::BitsAllocated, &bitAllocated) == false)
		{
			if (dataset.TryGetValueInt(fm::DicomTagID::BitsStored, &bitAllocated) == false)
			{
				bitAllocated = 16;
				qWarning() << "bits allocated, bits stored not exist. set to default value : " << bitAllocated;
			}
		}

		if (dataset.TryGetValueInt(fm::DicomTagID::SamplesPerPixel, &samplePerPixel) == false)
		{
			samplePerPixel = 1;
			qWarning() << "sample per pixel not exist. set to default value : " << samplePerPixel;
		}

		m_bitPerPixel = bitAllocated * samplePerPixel;

		m_columns = dataset.GetValueInt(fm::DicomTagID::Columns);
		m_row = dataset.GetValueInt(fm::DicomTagID::Rows);
		m_spacingX = dataset.GetValueFloat(fm::DicomTagID::PixelSpacing, 0);
		m_spacingY = dataset.GetValueFloat(fm::DicomTagID::PixelSpacing, 1);

		m_secondaryCaptureDeviceID = dataset.GetValueWString(fm::DicomTagID::SecondaryCaptureDeviceID);
	}

	bool DicomType::IsNotSupported() const
	{
		return 
			IsCTImage() == false && 
			IsPDF() == false && 
			IsSecondaryImage() == false;
	}

	bool DicomType::IsCTImage() const
	{
		return m_hasImagePixelData && !IsSecondaryImage();
	}

	bool DicomType::IsPDF() const
	{
		return (m_sopClass == UID_EncapsulatedPDFStorage);
	}

	bool DicomType::IsOriginalImage() const
	{
		std::wstring str = GetImageTypePixelDataCharacteristic();
		return QString::fromStdWString(str).toUpper() == "ORIGINAL";
	}

	bool DicomType::IsDerivedImage() const
	{
		std::wstring str = GetImageTypePixelDataCharacteristic();
		return QString::fromStdWString(str).toUpper() == "DERIVED";
	}

	bool DicomType::IsPrimaryImage() const
	{
		std::wstring str = GetImageTypePatientExamination();
		return QString::fromStdWString(str).toUpper() == "PRIMARY";
	}

	bool DicomType::IsSecondaryImage() const
	{
		if (m_sopClass == UID_SecondaryCaptureImageStorage)
		{
			return true;
		}
		return !m_secondaryCaptureDeviceID.empty();
	}

	int DicomType::GetBitPerPixel() const
	{
		return m_bitPerPixel;
	}

	bool DicomType::IsCompatibleImageToMEDIP() const
	{
		bool result = IsCTImage() && Is_8Bit_Or_16Bit_Image();
			
		return result;
	}

	bool DicomType::Is_8Bit_Or_16Bit_Image() const
	{
		return m_hasImagePixelData &&
			(m_bitPerPixel == 8) ||
			(m_bitPerPixel == 16);
	}

	bool DicomType::Is_24Bit_Or_32Bit_Image() const
	{
		return m_hasImagePixelData &&
			(m_bitPerPixel == 24) ||
			(m_bitPerPixel == 32);
	}

	std::wstring DicomType::GetImageTypeID() const
	{
		std::wstring id;
		for (std::wstring s : m_imageTypeList)
		{
			id += (s + L"\\");
		}
		return id;
	}

	std::wstring DicomType::GetImageInfoID() const
	{
		return QString("%1\\%2\\%3\\%4")
			.arg(m_columns)
			.arg(m_row)
			.arg(m_spacingX)
			.arg(m_spacingY).toStdWString();
	}

	std::wstring DicomType::GetImageGroupID() const
	{
		return GetImageTypeID() + GetImageInfoID();
	}

	std::wstring DicomType::GetImageTypePixelDataCharacteristic() const
	{
		std::wstring str;
		if (m_imageTypeList.size() >= 1)
		{
			str = m_imageTypeList[0];
		}
		return str;
	}

	std::wstring DicomType::GetImageTypePatientExamination() const
	{
		std::wstring str;
		if (m_imageTypeList.size() >= 2)
		{
			str = m_imageTypeList[1];
		}
		return str;
	}

	std::wstring DicomType::GetImageTypeIOD() const
	{
		std::wstring str;
		if (m_imageTypeList.size() >= 3)
		{
			str = m_imageTypeList[2];
		}
		return str;
	}

	bool DicomType::hasImageData(const DicomDataset& dcmDataset) const
	{
		int width = dcmDataset.GetValueInt(DicomTagID::Columns);
		if (width <= 0)
		{
			return false;
		}

		int height = dcmDataset.GetValueInt(DicomTagID::Rows);
		if (height <= 0)
		{
			return false;
		}

		void* pData = dcmDataset.GetValuePtr(fm::DicomTagID::PixelData);
		if (pData == nullptr)
		{
			return false;
		}

		return true;
	}
}
