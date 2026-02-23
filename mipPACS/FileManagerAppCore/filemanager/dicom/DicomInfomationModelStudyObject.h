#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <qvector3d>
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/export.h"

namespace fm
{
	class DicomInfomationModelSeriesObject;

	class FM_CORE_EXPORT DicomInfomationModelStudyObject
	{
	public:
		DicomInfomationModelStudyObject(bool isEnableSaveNonePixelData);
		DicomInfomationModelStudyObject(const DicomDataset& dataset, DicomInfomationModelSeriesObject** ppOutNewSeries, bool isEnableSaveNonePixelData);
		~DicomInfomationModelStudyObject();

	public:
		bool Add(const DicomDataset& dataset, DicomInfomationModelSeriesObject** ppOutNewSeries = nullptr);
		void Clear();
		int ImageListCount();
		int DicomSeriesCount();

		bool GetFirstSeriesDataset(DicomDataset** ppDataset);
		bool GetFirstSeries(DicomInfomationModelSeriesObject** ppSeries);
		bool GetStudyDataset(DicomDataset** ppDataset);

		std::unordered_map<std::wstring, DicomInfomationModelSeriesObject*> GetSeriesMap();
		std::vector<DicomInfomationModelSeriesObject*> GetSeriesList();

	private:
		std::wstring CreateSeriesModelID(const DicomDataset& dataset);

	private:
		DicomDataset* m_pStudyDcmDataset;
		std::unordered_map<std::wstring, DicomInfomationModelSeriesObject*> m_seriesMap;
		bool m_isEnableNoneImageData;
	};
}

