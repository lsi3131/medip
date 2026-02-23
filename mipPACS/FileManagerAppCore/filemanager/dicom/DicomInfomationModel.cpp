#include "stdafx.h"
#include "DicomInfomationModel.h"
#include "DicomInfomationModelSeriesObject.h"
#include "DicomInfomationModelStudyObject.h"
#include <dcmtk/dcmdata/dcuid.h>
#include <qfileinfo>

namespace fm
{
	std::shared_ptr<DicomInfomationModel> DicomInfomationModel::CreateOnlyImageData()
	{
		bool isEnableNoneImageData = false;
		return std::make_shared<DicomInfomationModel>(isEnableNoneImageData);
	}
	std::shared_ptr<DicomInfomationModel> DicomInfomationModel::CreateDefault()
	{
		bool isEnableNoneImageData = true;
		return std::make_shared<DicomInfomationModel>(isEnableNoneImageData);
	}
	//===========================================
	//		DicomInfomationModel
	//===========================================
	DicomInfomationModel::DicomInfomationModel(bool isEnableNoneImageData) :
		m_isEnableNoneImageData(isEnableNoneImageData)
	{
	}

	DicomInfomationModel::~DicomInfomationModel()
	{
		Clear();
	}

	void DicomInfomationModel::SetEnableSaveNonePixelData(bool value)
	{
		m_isEnableNoneImageData = true;
	}

	bool DicomInfomationModel::IsEnableSaveNonePixelData() const
	{
		return m_isEnableNoneImageData;
	}

	void DicomInfomationModel::SortAllImageList()
	{
		std::vector<DicomInfomationModelSeriesObject*> seriesList = GetSeriesList();
		for (DicomInfomationModelSeriesObject* pSeries : seriesList)
		{
			pSeries->SortImageList();
		}
	}

	bool DicomInfomationModel::Add(const DicomDataset& dataset, DicomInfomationModelStudyObject** ppOutNewStudy, DicomInfomationModelSeriesObject** ppOutNewSeries)
	{
		std::wstring studyModeID = CreateStudyModelID(dataset);
		if (studyModeID.empty())
		{
			qWarning() << "study instance UID is empty. ";
			return false;
		}

		DicomInfomationModelStudyObject* pStudy = GetStudy(studyModeID);
		if (pStudy)
		{
			return pStudy->Add(dataset, ppOutNewSeries);
		}
		else
		{
			DicomInfomationModelStudyObject* pNewStudy = new DicomInfomationModelStudyObject(dataset, ppOutNewSeries, m_isEnableNoneImageData);
			if (ppOutNewStudy)
			{
				*ppOutNewStudy = pNewStudy;
			}
			m_studyMap.insert(std::pair<std::wstring, DicomInfomationModelStudyObject*>(studyModeID, pNewStudy));
		}
		return true;
	}

	void DicomInfomationModel::Clear()
	{
		for (auto& pair : m_studyMap)
		{
			delete pair.second;
			pair.second = nullptr;
		}
		m_studyMap.clear();
	}

	bool DicomInfomationModel::IsEmpty()
	{
		return m_studyMap.empty();
	}

	bool DicomInfomationModel::GetFirstStudy(DicomInfomationModelStudyObject** ppStudy)
	{
		if (m_studyMap.empty())
		{
			return false;
		}

		if (ppStudy)
		{
			*ppStudy = (*m_studyMap.begin()).second;
		}

		return true;
	}

	std::vector<DicomInfomationModelStudyObject*> DicomInfomationModel::GetStudyList()
	{
		std::vector<DicomInfomationModelStudyObject*> listStudy;

		for (auto pair : m_studyMap)
		{
			listStudy.push_back(pair.second);
		}

		return listStudy;
	}

	std::vector<DicomInfomationModelSeriesObject*> DicomInfomationModel::GetSeriesList()
	{
		std::vector<DicomInfomationModelSeriesObject*> listSeriesResult;
		std::vector<DicomInfomationModelStudyObject*> listStudy = GetStudyList();
		for (auto& study : listStudy)
		{
			std::vector<DicomInfomationModelSeriesObject*> listSeries = study->GetSeriesList();
			for (auto& series : listSeries)
			{
				listSeriesResult.push_back(series);
			}
		}
		return listSeriesResult;
	}

	DicomInfomationModelStudyObject* DicomInfomationModel::GetStudy(std::wstring studyInstanceUID)
	{
		auto it = m_studyMap.find(studyInstanceUID);
		if (it != m_studyMap.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	/*
		DatasetInfomationModel에서 StudyID는 다음과 같은 순서로 정의한다.
		1. StudyInstanceUID
		2. StudyID
		3. Study Description
		4. Undefined Study
		Study의 경우에는 임의의 ID를 사용해도 Series를 분류하는데는 문제가 없다.
		다만 PACS 업로드 등 DICOM 통신을 할 때는 필요하다.
	*/
	std::wstring DicomInfomationModel::CreateStudyModelID(const DicomDataset& dataset)
	{
		std::wstring studyInstanceUIDnstanceUID = dataset.GetValueWString(DicomTagID::StudyInstanceUID);
		if (!studyInstanceUIDnstanceUID.empty())
		{
			return studyInstanceUIDnstanceUID;
		}
		else
		{
			qWarning() << "DicomInfomationModel study instance UID is empty. create new study model ID";
			std::wstring studyID = dataset.GetValueWString(DicomTagID::StudyID);
			std::wstring studyDescription = dataset.GetValueWString(DicomTagID::StudyDescription);
			std::wstring studyDate = dataset.GetValueWString(DicomTagID::StudyDate);
			std::wstring studyTime = dataset.GetValueWString(DicomTagID::StudyDate);
			std::wstring studyModelID = studyID + studyDescription + studyDate + studyTime;
			if (!studyModelID.empty())
			{
				return studyModelID;
			}
			else
			{
				const std::wstring undefined_study_model_id = L"undefined-study-model-id";
				qDebug() << "DicomInfomationModel create 'undefined_study_model_id' :" << QString::fromStdWString(undefined_study_model_id);
				return undefined_study_model_id;
			}
		}
	}
}
