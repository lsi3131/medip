#pragma once

#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/dicom/DicomDataset_define.h"
#include "DicomReader.h"
#include <qstring>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <vector>

class DcmDataset;
class DcmFileFormat;
class DcmTagKey;
class DcmtkSeriesInfo;
class DicomVolumeInfo;

namespace gdcm
{
	class Pixmap;
	class File;
	class Image;
	class PixmapReader;
}

namespace fm
{
	class DicomDataset;
	class DicomDatasetSaveParameter;
	class DicomDatasetLoadParameter;
	class DicomDataImage_sint16;

	class FM_CORE_EXPORT DicomDataset
	{
	public:
		enum class EType
		{
			NotSupported,
			Image_8bit,
			Image_16bit,
			Image_24bit,
			Image_32bit,
			Image_SecondaryCapture_8bit,
			Image_SecondaryCapture_16bit,
			Image_SecondaryCapture_24bit,
			Image_SecondaryCapture_32bit,
			EncapsulatedPDFStorage,
		};
	public:
		static bool IsDicomFile(QString filepath);
		static bool LoadList(std::vector<DicomDataset>& dcmDataList, QString filepath);

	public:
		DicomDataset();
		~DicomDataset();

		DicomDataset(DcmtkSeriesInfo* pDcmtkSeriesInfo);
		DicomDataset(DcmDataset* pDcmDataset);
		DicomDataset(const DicomDataset& rhs);
		DicomDataset& operator=(const DicomDataset& rhs);

	public:
		bool Init(DcmtkSeriesInfo* pDcmtkSeriesInfo);
		bool SetNewStudyInstanceUID();

		bool SaveToFile(QString filepath, DicomDatasetSaveParameter* pParam = nullptr);
		bool LoadFromFile(QString filepath, const DicomDatasetLoadParameter* pParam = nullptr);

		bool SetTagValue(DicomTagID tagID, std::wstring value);
		bool SetTagValue(DicomTagID tagID, std::string value);
		bool IsTagExists(DicomTagID tagID) const;

		bool TryGetValueText(DicomTagID tagID, std::wstring* pValue, int pos = 0) const;
		std::wstring GetValueWString(DicomTagID tagID, int pos = 0) const;
		std::string GetValueString(DicomTagID tagID, int pos = 0) const;

		std::vector<std::wstring> GetValueWStringList(DicomTagID tagID, int pos = 0) const;

		void* GetValuePtr(DicomTagID tagID, int* pCount = nullptr) const;
		void* GetValuePtr(DicomTagID tagID, int width, int height) const;

		bool TryGetValueInt(DicomTagID tagID, int* pOutValue, int pos = 0) const;
		int GetValueInt(DicomTagID tagID, int pos = 0) const;

		bool TryGetValueFloat(DicomTagID tagID, float* pOutValue, int pos = 0) const;
		float GetValueFloat(DicomTagID tagID, int pos = 0) const;

		bool TryGetImagePositionPatient(mip::VECTOR3* pOutVector) const;
		bool TryGetImageOrientationPatient(mip::VECTOR3* pOutVectorX, mip::VECTOR3* pOutVectorY) const;

		void SetValue_Extension(std::wstring id, std::wstring data);
		void SetValue_Extension(std::wstring id, std::string data);
		std::wstring GetValue_Extension(std::wstring id) const;

		void SetFilePath_Extension(std::wstring data);
		std::wstring GetFilePath_Extension() const;

		void AppendDataset(DicomDataset* dataset);
		std::vector<TagValuePair> GetAllTagValues() const;

		DICOM_HEADER_INFO ToDcmHeaderInfo() const;
		DcmtkSeriesInfo ToDcmtkSeriesInfo() const;
		DicomVolumeInfo ToDicomVolumeInfo(int sliceCount, float sliceThickness = 0.0f) const;

		DCM_MODAL_TYPE GetDcmModalityType() const;
		bool TryGetWindowingWidth(float *pOutValue) const;
		bool TryGetWindowingLevel(float *pOutValue) const;

		EType GetDicomType() const;

		DcmDataset* Data() const;

		bool HasImageData() const;

	private:
		bool LoadDcmFileFormat(DcmFileFormat* pOutDcmFileFormat, const QString& dcmFilePath, const DicomDatasetLoadParameter* pParam);
		bool ShouldDecompress(const DcmDataset& dcmDataset);
		bool DecompressDicomFileFormat(DcmFileFormat* pOutDcmFileFormat, const QString& dcmFilePath, const DicomDatasetLoadParameter* pParam);

		bool LoadDicomFile_GDCM(gdcm::PixmapReader* outPixmapReader, const QString& dcmFilePath);
		bool Decompress_GDCMPixmap(gdcm::Pixmap* outPixmap);
		bool SaveDicomFile_GDCM(const QString& dcmFilePath, const gdcm::Pixmap& pmap, const gdcm::File& file);

		EType getTypeAsBitData() const;
		EType getTypeAsBitDataSecondaryImage() const;

	private:
		DcmDataset* m_pDcmDataset;
		std::unordered_map<std::wstring, std::wstring> m_extensionDataMap;
	};
}
