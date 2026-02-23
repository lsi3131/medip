#include "stdafx.h"
#include "DicomDatasetIO.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include <qfileinfo>
#include <qdir>

namespace fm
{
	//======================================
	//	DicomDatasetCallBackInfo	
	//======================================
	DicomDatasetCallBackInfo::DicomDatasetCallBackInfo(EDicomDatasetIOStatus status) :
		m_status(status),
		m_progressCount(0),
		m_maxProgressCount(1),
		m_pDicomDataset(nullptr)
	{
	}

	DicomDatasetCallBackInfo::DicomDatasetCallBackInfo(EDicomDatasetIOStatus status, int progressCount, int maxProgressCount, DicomDataset* pDcmDataset) :
		m_status(status),
		m_progressCount(progressCount),
		m_maxProgressCount(maxProgressCount),
		m_pDicomDataset(pDcmDataset)
	{
	}

	EDicomDatasetIOStatus DicomDatasetCallBackInfo::GetStatus() const
	{
		return m_status;
	}

	int DicomDatasetCallBackInfo::GetProgressCount() const
	{
		return m_progressCount;
	}

	int DicomDatasetCallBackInfo::GetMaxProgressCount() const
	{
		return m_maxProgressCount;
	}

	float DicomDatasetCallBackInfo::GetProgressRate() const
	{
		if (m_maxProgressCount == 0)
		{
			return 0.0f;
		}
		return (float)m_progressCount / (float)m_maxProgressCount;
	}

	const DicomDataset* DicomDatasetCallBackInfo::GetDicomDataset() const
	{
		return m_pDicomDataset;
	}

	//=====================================
	//	DicomDatasetIO	
	//========================================
	DicomDatasetIO::DicomDatasetIO() :
		m_isIOInProgress(false),
		m_maxCount(1),
		m_index(0),
		m_pUpdateDicomDataset(nullptr),
		m_pOutDicomDatasetList(nullptr),
		m_pCallBackFunction(nullptr),
		m_pCallBackContext(nullptr)
	{
		m_filters = QDir::Filter::NoDotAndDotDot | QDir::Filter::NoSymLinks | QDir::Filter::AllEntries;
	}

	DicomDatasetIO::~DicomDatasetIO()
	{
	}

	bool DicomDatasetIO::Load(std::vector<std::shared_ptr<DicomDataset>>* outDicomDatasetList, QStringList filePathList, DicomDatasetIOCallBackFunc pCallbackFunc, void* pCallbackContext)
	{
		m_pOutDicomDatasetList = outDicomDatasetList;
		m_pCallBackFunction = pCallbackFunc;
		m_pCallBackContext = pCallbackContext;

		FilePathManager::Instance()->ClearTempDir();
		return LoadFileList_Loop(filePathList, nullptr);
	}


	bool DicomDatasetIO::LoadDatasetList(std::vector<std::shared_ptr<DicomDataset>>* outDicomDatasetList, const std::vector<DicomDataset*>& datasetList, DicomDatasetIOCallBackFunc pCallbackFunc, void* pCallbackContext)
	{
		FilePathManager::Instance()->ClearTempDir();

		m_pOutDicomDatasetList = outDicomDatasetList;
		m_pCallBackFunction = pCallbackFunc;
		m_pCallBackContext = pCallbackContext;

		UpdateStarted();

		qInfo() << "dicom loading dataset list is started";

		m_isIOInProgress = true;
		int maxCount = datasetList.size();
		for (int i = 0; i < datasetList.size(); ++i)
		{
			if (m_isIOInProgress == false)
			{
				qInfo() << "dicom loading file list is aborted";
				UpdateAborted();
				return false;
			}

			UpdateInProgress(i, maxCount, datasetList[i]);
			/* == Debug ==*/
			//QThread::msleep(50);
		}

		m_isIOInProgress = false;

		qInfo() << "dicom loading file list is finished";
		UpdateFinished();

		return true;
	}

	bool DicomDatasetIO::Update(QStringList filePathList, DicomDataset* pUpdateDicomDataset)
	{
		FilePathManager::Instance()->ClearTempDir();
		return LoadFileList_Loop(filePathList, pUpdateDicomDataset);
	}

	bool DicomDatasetIO::LoadFromDirectory(const std::wstring& dirpath)
	{
		QFileInfo fileInfo(QString::fromStdWString(dirpath));
		if (fileInfo.isDir() == false)
		{
			return false;
		}

		QString rootDirpath = fileInfo.absoluteFilePath();
		QDir dir(rootDirpath);
		LoadFromDirectory_Recursive(rootDirpath, dir.entryList(m_filters));

		return true;
	}

	bool DicomDatasetIO::IsIOInProgress()
	{
		return m_isIOInProgress;
	}

	void DicomDatasetIO::AbortIOProgress()
	{
		if (m_isIOInProgress)
		{
			qWarning() << "load is aborted";
			m_isIOInProgress = false;
		}
	}

	bool DicomDatasetIO::LoadFileList_Loop(QStringList filePathList, DicomDataset* pUpdateDicomDataset)
	{
		m_pUpdateDicomDataset = pUpdateDicomDataset;

		UpdateStarted();

		m_isIOInProgress = true;
		m_index = 0;

		CalculateMaxFileCount(filePathList);

		for (auto& filePath : filePathList)
		{
			QFileInfo fileInfo(filePath);
			if (fileInfo.isDir())
			{
				LoadFromDirectory(filePath.toStdWString());
			}
			else if (fileInfo.isFile())
			{
				LoadDcmFile_And_EmitInProgress(filePath);
			}

			if (m_isIOInProgress == false)
			{
				UpdateAborted();
				return false;
			}
		}

		m_isIOInProgress = false;

		UpdateFinished();
		return true;
	}

	void DicomDatasetIO::CalculateMaxFileCount(QStringList filePathList)
	{
		m_maxCount = 0;

		/* Max Count 계산 */
		for (auto& filePath : filePathList)
		{
			QFileInfo fileInfo(filePath);
			if (fileInfo.isDir())
			{
				QString rootDirpath = fileInfo.absoluteFilePath();
				QDir dir(rootDirpath);

				m_maxCount += GetFileCount_Recursive(rootDirpath, dir.entryList(m_filters));
			}
			else if (fileInfo.isFile())
			{
				m_maxCount++;
			}
		}
	}

	int DicomDatasetIO::GetFileCount_Recursive(QString dirpath, QStringList fileNameList)
	{
		int count = 0;
		for (auto& fileName : fileNameList)
		{
			QString filePath = dirpath + "/" + fileName;
			QFileInfo fileInfo(filePath);
			if (fileInfo.isDir())
			{
				QDir dirChild(filePath);
				QString childDirPath = dirChild.absolutePath();
				QStringList fileNameList = dirChild.entryList(m_filters);
				count += GetFileCount_Recursive(childDirPath, fileNameList);
			}
			else if (fileInfo.isFile())
			{
				count++;
			}
		}

		return count;
	}

	bool DicomDatasetIO::LoadDcmFile_And_EmitInProgress(QString filePath)
	{
		m_index++;
		std::shared_ptr<DicomDataset> dcmDataset = std::make_shared<DicomDataset>();
		if (dcmDataset->LoadFromFile(filePath))
		{
			/* Import Dicom File List 처리를 위해 File Load 경로를 추가한다. */
			dcmDataset->SetFilePath_Extension(filePath.toStdWString());
			m_pOutDicomDatasetList->push_back(dcmDataset);
			if (m_pUpdateDicomDataset)
			{
				/*
					DCM dataset file을 load 후 바로 Save할 경우 정상적으로 저장되지 않는 문제 발생
					임시파일에 저장 후 원본에 Copy하도록 적용
				*/
				QString filePathTemp = filePath + "temp.dcm";
				dcmDataset->AppendDataset(m_pUpdateDicomDataset);
				dcmDataset->SaveToFile(filePathTemp);

				QFile::remove(filePath);
				QFile::rename(filePathTemp, filePath);
			}
			UpdateInProgress(m_index, m_maxCount, dcmDataset.get());
		}
		return true;
	}

	bool DicomDatasetIO::LoadFromDirectory_Recursive(QString dirpath, QStringList fileNameList)
	{
		int count = fileNameList.count();
		for (int i = 0; i < count; ++i)
		{
			QString filepath = dirpath + "/" + fileNameList[i];
			if (m_isIOInProgress == false)
			{
				return false;
			}

			QFileInfo fileInfo(filepath);
			if (fileInfo.isDir())
			{
				QDir dir(filepath);
				QString subDirPath = dir.absolutePath();
				LoadFromDirectory_Recursive(subDirPath, dir.entryList(m_filters));
			}
			else
			{
				LoadDcmFile_And_EmitInProgress(filepath);
			}
		}
		return true;
	}

	void DicomDatasetIO::UpdateStarted()
	{
		if (m_pCallBackFunction)
		{
			m_pCallBackFunction(m_pCallBackContext, DicomDatasetCallBackInfo(EDicomDatasetIOStatus::started));
		}
	}

	void DicomDatasetIO::UpdateInProgress(int progressCount, int maxCount, DicomDataset* dcmDataset)
	{
		if (m_pCallBackFunction)
		{
			m_pCallBackFunction(m_pCallBackContext, DicomDatasetCallBackInfo(EDicomDatasetIOStatus::in_progress, progressCount, maxCount, dcmDataset));
		}
	}

	void DicomDatasetIO::UpdateFinished()
	{
		if (m_pCallBackFunction)
		{
			m_pCallBackFunction(m_pCallBackContext, DicomDatasetCallBackInfo(EDicomDatasetIOStatus::finished));
		}
	}

	void DicomDatasetIO::UpdateAborted()
	{
		if (m_pCallBackFunction)
		{
			m_pCallBackFunction(m_pCallBackContext, DicomDatasetCallBackInfo(EDicomDatasetIOStatus::aborted));
		}
	}

}
