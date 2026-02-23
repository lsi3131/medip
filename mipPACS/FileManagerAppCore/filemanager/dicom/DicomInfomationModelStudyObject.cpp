#include "stdafx.h"
#include "DicomInfomationModelStudyObject.h"
#include "DicomInfomationModelSeriesObject.h"
#include "DicomType.h"
#include <dcmtk/dcmdata/dcuid.h>
#include <qfileinfo>

namespace fm
{
	DicomInfomationModelStudyObject::DicomInfomationModelStudyObject(bool isEnableSaveNonePixelData) :
		m_pStudyDcmDataset(nullptr),
		m_isEnableNoneImageData(isEnableSaveNonePixelData)
	{
	}

	DicomInfomationModelStudyObject::DicomInfomationModelStudyObject(const DicomDataset& dataset, DicomInfomationModelSeriesObject** ppOutNewSeries, bool isEnableSaveNonePixelData) :
		m_pStudyDcmDataset(nullptr),
		m_isEnableNoneImageData(isEnableSaveNonePixelData)
	{
		Add(dataset, ppOutNewSeries);
	}

	DicomInfomationModelStudyObject::~DicomInfomationModelStudyObject()
	{
		Clear();
	}

	bool DicomInfomationModelStudyObject::Add(const DicomDataset& dataset, DicomInfomationModelSeriesObject** ppOutNewSeries)
	{
		if (m_pStudyDcmDataset == nullptr)
		{
			m_pStudyDcmDataset = new DicomDataset(dataset);
		}

		DicomType dcmType(dataset);
		std::wstring seriesModelID = CreateSeriesModelID(dataset);

		std::wstring imageTypeID = dcmType.GetImageTypeID();
		std::wstring imageInfoID = dcmType.GetImageInfoID();

		seriesModelID += imageTypeID;
		seriesModelID += imageInfoID;

		//mip::VECTOR3 vX;
		//mip::VECTOR3 vY;
		//if (dataset.TryGetImageOrientationPatient(&vX, &vY))
		//{
		//	QString imageOrientationPatientID =
		//		QString("%1.%2.%3/%4.%5.%6")
		//		.arg(vX.x).arg(vX.y).arg(vX.z)
		//		.arg(vY.x).arg(vY.y).arg(vY.z);
		//	seriesModelID += imageOrientationPatientID.toStdWString();
		//}

		if (!seriesModelID.empty())
		{
			auto it = m_seriesMap.find(seriesModelID);
			if (it != m_seriesMap.end())
			{
				return (*it).second->Add(dataset);
			}
			else
			{
				DicomInfomationModelSeriesObject* pNewSeries = new DicomInfomationModelSeriesObject(dataset, m_isEnableNoneImageData);
				if (ppOutNewSeries)
				{
					*ppOutNewSeries = pNewSeries;
				}

				m_seriesMap.insert(std::pair<std::wstring, DicomInfomationModelSeriesObject*>(seriesModelID, pNewSeries));
			}
		}

		return true;
	}

	void DicomInfomationModelStudyObject::Clear()
	{
		if (m_pStudyDcmDataset)
		{
			delete m_pStudyDcmDataset;
			m_pStudyDcmDataset = nullptr;
		}

		for (auto& pair : m_seriesMap)
		{
			delete pair.second;
			pair.second = nullptr;
		}
		m_seriesMap.clear();
	}

	int DicomInfomationModelStudyObject::ImageListCount()
	{
		int count = 0;
		for (auto& pair : m_seriesMap)
		{
			count += pair.second->ImageInstanceCount();
		}
		return count;
	}

	int DicomInfomationModelStudyObject::DicomSeriesCount()
	{
		int count = 0;
		for (auto& pair : m_seriesMap)
		{
			count += pair.second->DicomSeriesCount();
		}
		return count;
	}

	bool DicomInfomationModelStudyObject::GetFirstSeriesDataset(DicomDataset** ppDataset)
	{
		if (m_seriesMap.empty())
		{
			if (m_pStudyDcmDataset == nullptr)
			{
				return false;
			}

			*ppDataset = m_pStudyDcmDataset;
			return true;
		}
		else
		{
			auto it = m_seriesMap.begin();
			return (*it).second->GetFirst(ppDataset);
		}
	}

	bool DicomInfomationModelStudyObject::GetFirstSeries(DicomInfomationModelSeriesObject** ppSeries)
	{
		if (m_seriesMap.empty())
		{
			return false;
		}

		if (ppSeries)
		{
			*ppSeries = (*m_seriesMap.begin()).second;
		}

		return true;
	}

	bool DicomInfomationModelStudyObject::GetStudyDataset(DicomDataset** ppDataset)
	{
		if (m_pStudyDcmDataset == nullptr)
		{
			return false;
		}

		*ppDataset = m_pStudyDcmDataset;
		return true;
	}

	std::unordered_map<std::wstring, DicomInfomationModelSeriesObject*> DicomInfomationModelStudyObject::GetSeriesMap()
	{
		return m_seriesMap;
	}

	std::vector<DicomInfomationModelSeriesObject*> DicomInfomationModelStudyObject::GetSeriesList()
	{
		std::vector<DicomInfomationModelSeriesObject*> listSeries;

		for (auto seriesPair : m_seriesMap)
		{
			listSeries.push_back(seriesPair.second);
		}

		return listSeries;
	}

	std::wstring DicomInfomationModelStudyObject::CreateSeriesModelID(const DicomDataset& dataset)
	{
		std::wstring seriesInstanceUIDnstanceUID = dataset.GetValueWString(DicomTagID::SeriesInstanceUID);
		if (!seriesInstanceUIDnstanceUID.empty())
		{
			return seriesInstanceUIDnstanceUID;
		}
		else
		{
			qWarning() << "DicomInfomationModel series instance UID is empty. create new series model ID";
			std::wstring seriesID = dataset.GetValueWString(DicomTagID::SeriesNumber);
			std::wstring seriesDescription = dataset.GetValueWString(DicomTagID::SeriesDescription);
			std::wstring seriesDate = dataset.GetValueWString(DicomTagID::SeriesDate);
			std::wstring seriesTime = dataset.GetValueWString(DicomTagID::SeriesTime);
			std::wstring seriesModelID = seriesID + seriesDescription + seriesDate + seriesTime;
			if (!seriesModelID.empty())
			{
				return seriesModelID;
			}
			else
			{
				const std::wstring undefined_series_model_id = L"undefined-series-model-id";
				qDebug() << "DicomInfomationModel create 'undefined_series_model_id' :" << QString::fromStdWString(undefined_series_model_id);
				return undefined_series_model_id;
			}
		}
	}
}
