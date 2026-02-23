#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <qvector3d>
#include <memory>
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomInfomationModelImageObject.h"
#include "filemanager/dicom/DicomInfomationModelSeriesObject.h"
#include "filemanager/dicom/DicomInfomationModelStudyObject.h"
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT DicomInfomationModel
	{
	public:
		static std::shared_ptr<DicomInfomationModel> CreateOnlyImageData();
		static std::shared_ptr<DicomInfomationModel> CreateDefault();
	public:
		DicomInfomationModel(bool isEnableNoneImageData);
		~DicomInfomationModel();

	private:
		DicomInfomationModel(const DicomInfomationModel&) {};
		void operator=(const DicomInfomationModel&) {}

	public:
		void SetEnableSaveNonePixelData(bool value);
		bool IsEnableSaveNonePixelData() const;

		void SortAllImageList();
		bool Add(const DicomDataset& dataset, DicomInfomationModelStudyObject** ppOutNewStudy = nullptr, DicomInfomationModelSeriesObject** ppOutNewSeries = nullptr);
		void Clear();
		bool IsEmpty();

		bool GetFirstStudy(DicomInfomationModelStudyObject** ppStudy);
		std::vector<DicomInfomationModelStudyObject*> GetStudyList();
		std::vector<DicomInfomationModelSeriesObject*> GetSeriesList();
		DicomInfomationModelStudyObject* GetStudy(std::wstring studyInstanceUID);

	private:
		std::wstring CreateStudyModelID(const DicomDataset& dataset);

	private:
		std::unordered_map<std::wstring, DicomInfomationModelStudyObject*> m_studyMap;
		bool m_isEnableNoneImageData;
	};
}
