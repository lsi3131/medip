#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <qvector3d>
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/export.h"

namespace mip
{
	class VECTOR3;
}
namespace fm
{
	class DicomInfomationModelImageObject;

	class FM_CORE_EXPORT DicomInfomationModelSeriesObject
	{
	public:
		DicomInfomationModelSeriesObject(bool isEnableSaveNonePixelData);
		DicomInfomationModelSeriesObject(const DicomDataset& dataset, bool isEnableSaveNonePixelData);
		~DicomInfomationModelSeriesObject();

	public:
		bool Add(const DicomDataset& dataset);
		int ImageInstanceCount() const;
		int DicomSeriesCount() const;
		bool IsEmpty() const;
		bool Get(DicomDataset** ppDataset, int index) const;
		bool GetFirst(DicomDataset** ppDataset) const;
		std::vector<DicomDataset*> GetDatasetList() const;
		void SortImageList();
		std::wstring SeriesInstanceUID() const;
		bool GetSliceThicknessBetweenTwoImage(float& sliceThickness);

		void SetEnableNoneImageData(bool value);
	private:
		bool SetupSeriesInstanceUID(const DicomDataset& dataset);
		std::wstring CreateSOPInstanceUID(const DicomDataset& dataset);
		bool IsAllImagePatientPositionTagExist();
		bool IsAllInstanceNumberTagExist();

		bool TryGetImagePositionPatient(mip::VECTOR3* pOutVector, const DicomDataset& dataset);
		bool TryGetImageOrientationPatient(mip::VECTOR3* pOutVectorX, mip::VECTOR3* pOutVectorY, const DicomDataset& dataset);
	private:
		std::wstring m_seriesInstanceUID;
		std::vector<DicomInfomationModelImageObject*> m_dcmImageList;
		std::unordered_map<std::wstring, DicomInfomationModelImageObject*> m_dcmImageMap;
		bool m_isEnableNoneImageData;
	};

}

