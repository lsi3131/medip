#pragma once

#include "filemanager/export.h"
#include <string>
#include <vector>

namespace fm
{
	class DicomDataset;

	class FM_CORE_EXPORT DicomType
	{
	public:
		DicomType(DicomDataset* pDcmDataset);
		DicomType(const DicomDataset& dataset);

	public:
		bool IsNotSupported() const;
		bool IsCTImage() const;
		bool IsPDF() const;

		bool IsOriginalImage() const;
		bool IsDerivedImage() const;

		bool IsPrimaryImage() const;
		bool IsSecondaryImage() const;

		int GetBitPerPixel() const;

		/**
		 * MEDIP에서 사용 가능한 이미지 포맷
		 * \return 
		 */
		bool IsCompatibleImageToMEDIP() const;
		bool Is_8Bit_Or_16Bit_Image() const;
		bool Is_24Bit_Or_32Bit_Image() const;

		std::wstring GetImageTypeID() const;
		std::wstring GetImageInfoID() const;

		/**
		 * Group ID : image type + image info 
		 * \return 
		 */
		std::wstring GetImageGroupID() const;

		std::wstring GetImageTypePixelDataCharacteristic() const;
		std::wstring GetImageTypePatientExamination() const;
		std::wstring GetImageTypeIOD() const;

	private:
		bool hasImageData(const DicomDataset& dcmDataset) const;

	private:
		std::vector<std::wstring> m_imageTypeList;
		std::string m_sopClass;
		bool m_hasImagePixelData;
		int m_bitPerPixel;

		int m_columns;
		int m_row;
		float m_spacingX;
		float m_spacingY;

		std::wstring m_secondaryCaptureDeviceID;
	};
}

